//
// Created by TheDaChicken on 7/10/2025.
//

#include "vendors/AirSpyHf.h"

#include "Utils.h"

PortSDR::AirSpyHfHost::AirSpyHfHost() : Host(AIRSPY_HF)
{
    RefreshDevices();
}

void PortSDR::AirSpyHfHost::RefreshDevices()
{
    const int device_count = airspyhf_list_devices(nullptr, 0);
    if (device_count == 0)
        return;

    uint64_t serials[device_count];
    int ret = airspyhf_list_devices(serials, device_count);
    if (ret < 0)
        return;

    devices_.clear();
    devices_.resize(device_count);

    for (int i = 0; i < device_count; ++i)
    {
        airspyhf_device* dev = nullptr;
        airspyhf_read_partid_serialno_t read_partid_serialno;

        devices_[i] = std::make_shared<Device>();
        devices_[i]->index = serials[i];
        devices_[i]->serial.clear();
        devices_[i]->name = "AIRSPYHF";
        devices_[i]->host = this;
        devices_[i]->unavailable = false;

        if (airspyhf_open_sn(&dev, serials[i]) != AIRSPYHF_SUCCESS)
        {
            devices_[i]->unavailable = true;
            devices_[i]->name += " (Unavailable)";
            continue;
        }

        if (airspyhf_board_partid_serialno_read(dev, &read_partid_serialno) == AIRSPYHF_SUCCESS)
        {
            devices_[i]->serial = string_format("%08X%08X",
                                                read_partid_serialno.serial_no[2],
                                                read_partid_serialno.serial_no[3]);

            // FIXME: Why is .c_str() / .data() needed here?
            // If not, the string memory is messed up.
            devices_[i]->name += string_format(" SN: %s", devices_[i]->serial.c_str());
        }

        airspyhf_close(dev);
    }
}

const std::vector<std::shared_ptr<PortSDR::Device>>& PortSDR::AirSpyHfHost::Devices() const
{
    return devices_;
}

std::unique_ptr<PortSDR::Stream> PortSDR::AirSpyHfHost::CreateStream() const
{
    return std::make_unique<AirSpyHfStream>();
}

PortSDR::AirSpyHfStream::~AirSpyHfStream()
{
    if (m_device)
    {
        airspyhf_close(m_device);
    }
}

int PortSDR::AirSpyHfStream::Initialize(const std::shared_ptr<Device>& device)
{
    if (m_device)
        return 0;

    int ret = airspyhf_open_sn(&m_device, device->index);
    if (ret != AIRSPYHF_SUCCESS)
    {
        return ret;
    }

    return 0;
}

int PortSDR::AirSpyHfStream::Start()
{
    if (!m_device)
        return -1;

    return airspyhf_start(m_device, AirSpySDRCallback, this);
}

int PortSDR::AirSpyHfStream::Stop()
{
    if (!m_device)
        return -1;

    return airspyhf_stop(m_device);
}

int PortSDR::AirSpyHfStream::SetCenterFrequency(uint32_t freq, int stream)
{
    if (!m_device)
        return -1;

    int ret = airspyhf_set_freq(m_device, freq);
    if (ret == AIRSPYHF_SUCCESS)
    {
        m_freq = freq;
    }

    return ret;
}

int PortSDR::AirSpyHfStream::SetSampleRate(uint32_t sampleRate)
{
    if (!m_device)
        return -1;

    int ret = airspyhf_set_samplerate(m_device, sampleRate);
    if (ret == AIRSPYHF_SUCCESS)
    {
        m_sampleRate = sampleRate;
    }

    return 0;
}

int PortSDR::AirSpyHfStream::SetGain(double gain)
{
    return -1; // AirSpy HF does not support gain control
}

int PortSDR::AirSpyHfStream::SetSampleFormat(SampleFormat format)
{
    if (!m_device)
        return -1;

    if (format != getNativeSampleFormat())
        return -1; // AirSpy HF does not support sample format changes

    return 0;
}

int PortSDR::AirSpyHfStream::SetGain(double gain, std::string_view name)
{
    if (!m_device)
        return -1;

    if (name == "ATT")
    {
        return SetAttenuation(gain);
    }

    return 0;
}

int PortSDR::AirSpyHfStream::SetGainModes(std::string_view name)
{
    return 0;
}

int PortSDR::AirSpyHfStream::SetAttenuation(double attenuation)
{
    if (!m_device)
        return -1;

    return airspyhf_set_hf_att(m_device, static_cast<int>(attenuation));
}

std::vector<uint32_t> PortSDR::AirSpyHfStream::GetSampleRates() const
{
    std::vector<uint32_t> sampleRates;
    uint32_t count;

    int ret = airspyhf_get_samplerates(m_device, &count, 0);
    if (ret < 0)
        return {};

    if (count > 0)
    {
        sampleRates.resize(count);
        airspyhf_get_samplerates(m_device, sampleRates.data(), count);
    }

    return sampleRates;
}

std::vector<PortSDR::SampleFormat> PortSDR::AirSpyHfStream::GetSampleFormats() const
{
    return {getNativeSampleFormat()};
}

std::vector<std::string> PortSDR::AirSpyHfStream::GetGainModes() const
{
    return {};
}

PortSDR::Gain PortSDR::AirSpyHfStream::GetGainStage() const
{
    return {};
}

std::vector<PortSDR::Gain> PortSDR::AirSpyHfStream::GetGainStages() const
{
    std::vector<Gain> gains;

    gains.emplace_back("ATT", MetaRange{0, 8, 1});

    return gains;
}

uint32_t PortSDR::AirSpyHfStream::GetCenterFrequency() const
{
    return m_freq;
}

uint32_t PortSDR::AirSpyHfStream::GetSampleRate() const
{
    return m_sampleRate;
}

double PortSDR::AirSpyHfStream::GetGain() const
{
    return 0;
}

double PortSDR::AirSpyHfStream::GetGain(std::string_view name) const
{
    return 0;
}

const std::string PortSDR::AirSpyHfStream::GetGainMode() const
{
    return ""; // AirSpy HF does not support gain modes
}

PortSDR::SampleFormat PortSDR::AirSpyHfStream::getNativeSampleFormat()
{
    return SAMPLE_FORMAT_IQ_FLOAT32; // AirSpy HF only supports this format
}

int PortSDR::AirSpyHfStream::AirSpySDRCallback(airspyhf_transfer_t* transfer)
{
    const auto* obj = static_cast<AirSpyHfStream*>(transfer->ctx);

    SDRTransfer sdr_transfer{};
    sdr_transfer.data = transfer->samples;
    sdr_transfer.frame_size = transfer->sample_count;
    sdr_transfer.dropped_samples = transfer->dropped_samples;
    sdr_transfer.format = getNativeSampleFormat();

    obj->m_callback(sdr_transfer);
    return 0;
}
