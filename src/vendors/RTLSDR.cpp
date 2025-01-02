//
// Created by TheDaChicken on 12/15/2024.
//

#include "RTLSDR.h"
#include "rtl-sdr.h"

#include <cassert>
#include <cstring>
#include <map>
#include <numeric>
#include <thread>

#include <Utils.h>

#define MAX_STR_SIZE 256
#define BUF_NUM  64
#define BUF_LEN  (4 * 32 * 512) /* must be multiple of 512 */

PortSDR::RTLHost::RTLHost() : Host()
{
    name = "RTL-SDR";

    RefreshDevices();
}

void PortSDR::RTLHost::RefreshDevices()
{
    const uint32_t device_count = rtlsdr_get_device_count();

    devices_.clear();
    devices_.resize(device_count);

    for (int i = 0; i < device_count; i++)
    {
        char manufact[MAX_STR_SIZE];
        char product[MAX_STR_SIZE];
        char serial[MAX_STR_SIZE];

        memset(manufact, 0, sizeof(manufact));
        memset(product, 0, sizeof(product));
        memset(serial, 0, sizeof(serial));

        devices_[i] = std::make_shared<Device>();
        devices_[i]->index = i;
        devices_[i]->host = this;
        devices_[i]->serial.clear();
        devices_[i]->unavailable = false;

        // Check if the device is available
        rtlsdr_dev_t *dev = nullptr;
        if (rtlsdr_open(&dev, i) != 0)
        {
            devices_[i]->unavailable = true;
            continue;
        }

        if (rtlsdr_get_device_usb_strings(i, manufact, product, serial) == 0)
        {
            devices_[i]->serial = serial;
            devices_[i]->name = string_format("%s %s SN: %s", manufact, product, serial);
        }
        else
        {
            devices_[i]->name = rtlsdr_get_device_name(device_count);
        }

        rtlsdr_close(dev);
    }
}

const std::vector<std::shared_ptr<PortSDR::Device>>& PortSDR::RTLHost::Devices() const
{
    return devices_;
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

int PortSDR::RTLStream::Initialize(const std::shared_ptr<Device>& device)
{
    int ret = 0;

    if (m_dev)
        return ret;

    ret = rtlsdr_open(&m_dev, device->index);
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

int PortSDR::RTLStream::SetCenterFrequency(uint32_t freq, int stream)
{
    if (stream > 0)
        return -1;

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
    if (type == SAMPLE_FORMAT_INT16)
    {
        m_outputBuffer.resize(BUF_LEN * sizeof(int16_t));
        m_sampleFormat = type;
    }

    return 0;
}

int PortSDR::RTLStream::SetIfGain(int gain)
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
        gains[i + 1] = if_gains[i].min();
    }

    for (int i = static_cast<int>(if_gains.size() - 1); i >= 0; i--)
    {
        const MetaRange& range = if_gains[i];
        double error = gain;
        double best_gain = range.min();

        for (double g = range.min(); g <= range.max(); g += range.step())
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
        int ret = rtlsdr_set_tuner_if_gain(m_dev, stage, int(gains[stage] * 10.0));
        if (ret < 0)
            return ret;
    }
    return 0;
}

int PortSDR::RTLStream::SetGain(int gain, std::string_view name)
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

int PortSDR::RTLStream::SetGain(int gain)
{
    if (!m_dev)
        return 0;

    return rtlsdr_set_tuner_gain(m_dev, gain);
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

int PortSDR::RTLStream::GetGain() const
{
    if (!m_dev)
        return 0;

    return rtlsdr_get_tuner_gain(m_dev);
}

const std::string PortSDR::RTLStream::GetGainMode() const
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
        range.emplace_back(gains[i]);

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
    return {SAMPLE_FORMAT_UINT8, SAMPLE_FORMAT_INT16};
}

void PortSDR::RTLStream::RTLSDRCallback(unsigned char* buf, uint32_t len, void* ctx)
{
    auto* obj = static_cast<RTLStream*>(ctx);
    assert(obj != nullptr);

    if (obj->m_sampleFormat == SAMPLE_FORMAT_UINT8)
    {
        obj->m_callback(buf, len);
    }
    else if (obj->m_sampleFormat == SAMPLE_FORMAT_INT16)
    {
        obj->uint8ToInt16.Process(buf,
                                  reinterpret_cast<int16_t*>(obj->m_outputBuffer.data()),
                                  len);
        obj->m_callback(obj->m_outputBuffer.data(), len);
    }
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
