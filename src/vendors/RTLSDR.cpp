//
// Created by TheDaChicken on 12/15/2024.
//

#include "vendors/RTLSDR.h"
#include "rtl-sdr.h"

#include <cassert>
#include <cstring>
#include <map>
#include <numeric>
#include <thread>

#include <Utils.h>

#include "Ranges.h"

#define MAX_STR_SIZE 256
#define BUF_NUM  64
#define BUF_LEN  (4 * 32 * 512) /* must be multiple of 512 */

PortSDR::RTLHost::RTLHost() : Host(RTL_SDR)
{
}

std::vector<PortSDR::Device> PortSDR::RTLHost::AvailableDevices() const
{
    std::vector<Device> devices;
    const uint32_t device_count = rtlsdr_get_device_count();

    devices.resize(device_count);

    int dev_id = 0;

    for (int i = 0; i < device_count; i++)
    {
        char serial[MAX_STR_SIZE];

        const int ret = rtlsdr_get_device_usb_strings(
            i, nullptr, nullptr,
            serial);
        if (ret != 0)
            continue;

        auto& device = devices[dev_id++];

        device.serial = serial;
        device.host = this;
    }

    devices.resize(dev_id);

    return devices;
}

PortSDR::DeviceInfo PortSDR::RTLStream::GetUSBStrings()
{
    DeviceInfo device;

    char manufact[MAX_STR_SIZE];
    char product[MAX_STR_SIZE];
    char serial[MAX_STR_SIZE];

    memset(manufact, 0, sizeof(manufact));
    memset(product, 0, sizeof(product));
    memset(serial, 0, sizeof(serial));

    if (rtlsdr_get_usb_strings(m_dev, manufact, product, serial) == 0)
    {
        device.serial = serial;
        device.name = string_format("%s %s SN: %s", manufact, product, serial);
    }

    return device;
}

std::unique_ptr<PortSDR::Stream> PortSDR::RTLHost::CreateStream() const
{
    return std::make_unique<RTLStream>();
}

PortSDR::RTLStream::~RTLStream()
{
    if (!m_dev)
        return;

    Stop();

    rtlsdr_close(m_dev);
    m_dev = nullptr;
}

int PortSDR::RTLStream::Initialize(const std::string_view serial)
{
    int ret = 0;

    if (m_dev)
        return ret;

    const int index = rtlsdr_get_index_by_serial(serial.data());
    if (index < 0)
        return ret;

    ret = rtlsdr_open(&m_dev, index);
    if (ret < 0)
        return ret;

    ret = rtlsdr_set_offset_tuning(m_dev, 1);
    if (ret != 0 && ret != -2)
        return ret;

    ret = rtlsdr_reset_buffer(m_dev);
    if (ret < 0)
        return ret;

    return 0;
}

int PortSDR::RTLStream::Start()
{
    if (!m_dev)
        return -1;

    if (m_thread.joinable())
        return 0;

    m_thread = std::thread(&RTLStream::Process, this);
    return 0;
}

int PortSDR::RTLStream::Stop()
{
    if (m_dev && m_thread.joinable())
    {
        rtlsdr_cancel_async(m_dev);
        m_thread.join();
    }
    return 0;
}

int PortSDR::RTLStream::SetCenterFrequency(uint32_t freq)
{
    return rtlsdr_set_center_freq(m_dev, freq);
}

int PortSDR::RTLStream::SetSampleRate(uint32_t freq)
{
    if (!m_dev)
        return -1;

    return rtlsdr_set_sample_rate(m_dev, freq);
}

int PortSDR::RTLStream::SetSampleFormat(SampleFormat type)
{
    if (type == SAMPLE_FORMAT_IQ_UINT8)
        return 0;
    return -1;
}

int PortSDR::RTLStream::SetIfGain(double gain)
{
    if (!m_dev)
        return 0;

    if (rtlsdr_get_tuner_type(m_dev) != RTLSDR_TUNER_E4000)
    {
        return 0;
    }

    std::vector<MetaRange> if_gains;

    if_gains.emplace_back(-3, 6, 9);
    if_gains.emplace_back(0, 9, 3);
    if_gains.emplace_back(0, 9, 3);
    if_gains.emplace_back(0, 2, 1);
    if_gains.emplace_back(3, 15, 3);
    if_gains.emplace_back(3, 15, 3);

    std::map<int, double> gains;

    /* initialize with min gains */
    for (int i = 0; i < if_gains.size(); i++)
    {
        gains[i + 1] = if_gains[i].Min();
    }

    for (int i = static_cast<int>(if_gains.size() - 1); i >= 0; i--)
    {
        const MetaRange& range = if_gains[i];
        double error = gain;
        double best_gain = range.Min();

        for (double g = range.Min(); g <= range.Max(); g += range.Step())
        {
            // Test the gain for this stage and calculate the error
            gains[i + 1] = g;

            // Calculate the sum of all gains
            double sum = std::accumulate(gains.begin(), gains.end(), 0.0,
                                         [](double acc, const std::pair<int, double>& p)
                                         {
                                             return acc + p.second;
                                         });

            double err = std::abs(gain - sum);
            if (err < error)
            {
                error = err;
                best_gain = g;
            }
        }
        gains[i + 1] = best_gain;
    }

    for (int stage = 1; stage <= gains.size(); stage++)
    {
        int ret = rtlsdr_set_tuner_if_gain(m_dev, stage, static_cast<int>(gains[stage] * 10.0));
        if (ret < 0)
            return ret;
    }
    return 0;
}

int PortSDR::RTLStream::SetGain(double gain, std::string_view name)
{
    if ("IF" == name)
    {
        return SetIfGain(gain);
    }
    if ("LNA" == name)
    {
        return SetGain(gain);
    }
    return -1;
}

int PortSDR::RTLStream::SetGain(double gain)
{
    if (!m_dev)
        return 0;

    return rtlsdr_set_tuner_gain(m_dev, gain * 10.0);
}

int PortSDR::RTLStream::SetGainModes(std::string_view mode)
{
    return -1;
}

uint32_t PortSDR::RTLStream::GetCenterFrequency() const
{
    if (!m_dev)
        return -1;

    return rtlsdr_get_center_freq(m_dev);
}

uint32_t PortSDR::RTLStream::GetSampleRate() const
{
    if (!m_dev)
        return -1;

    return rtlsdr_get_sample_rate(m_dev);
}

double PortSDR::RTLStream::GetGain() const
{
    if (!m_dev)
        return 0;

    return rtlsdr_get_tuner_gain(m_dev) / 10.0;
}

double PortSDR::RTLStream::GetGain(std::string_view name) const
{
    if ("LNA" == name)
    {
        return GetGain();
    }

    return 0;
}

std::string PortSDR::RTLStream::GetGainMode() const
{
    return "";
}

PortSDR::Gain PortSDR::RTLStream::GetGainStage() const
{
    MetaRange range;

    if (!m_dev)
        return {};

    int count = rtlsdr_get_tuner_gains(m_dev, nullptr);
    if (count <= 0)
    {
        return {};
    }

    std::vector<int> gains(count);

    count = rtlsdr_get_tuner_gains(m_dev, gains.data());
    for (int i = 0; i < count; i++)
        range.emplace_back(static_cast<double>(gains[i]) / 10.0f);

    return {"LNA", range};
}

std::vector<PortSDR::Gain> PortSDR::RTLStream::GetGainStages() const
{
    std::vector<Gain> gains;

    if (rtlsdr_get_tuner_type(m_dev) == RTLSDR_TUNER_E4000)
    {
        gains.emplace_back("IF", MetaRange{3, 56, 1});
    }

    gains.emplace_back(GetGainStage());
    return gains;
}

std::vector<uint32_t> PortSDR::RTLStream::GetSampleRates() const
{
    std::vector<uint32_t> range;

    range.emplace_back(250000); // known to work
    range.emplace_back(1000000); // known to work
    range.emplace_back(1024000); // known to work
    range.emplace_back(1800000); // known to work
    range.emplace_back(1920000); // known to work
    range.emplace_back(2000000); // known to work
    range.emplace_back(2048000); // known to work
    range.emplace_back(2400000); // known to work
    range.emplace_back(2560000); // known to work
    range.emplace_back(2600000); // may work
    range.emplace_back(2800000); // may work
    range.emplace_back(3000000); // may work
    range.emplace_back(3200000); // max rate

    return range;
}

std::vector<std::string> PortSDR::RTLStream::GetGainModes() const
{
    return {};
}

std::vector<PortSDR::SampleFormat> PortSDR::RTLStream::GetSampleFormats() const
{
    return {SAMPLE_FORMAT_IQ_UINT8};
}

void PortSDR::RTLStream::RTLSDRCallback(unsigned char* buf, uint32_t len, void* ctx)
{
    const auto* obj = static_cast<RTLStream*>(ctx);
    assert(obj != nullptr);

    SDRTransfer transfer{};

    transfer.format = SAMPLE_FORMAT_IQ_UINT8;
    transfer.data = buf;
    transfer.frame_size = len / 2;

    obj->m_callback(transfer);
}

void PortSDR::RTLStream::Process()
{
    int ret = rtlsdr_read_async(m_dev, RTLSDRCallback, this, BUF_NUM, BUF_LEN);
    if (ret != 0)
    {
        // TODO: handle error
        m_dev = nullptr;
    }
}
