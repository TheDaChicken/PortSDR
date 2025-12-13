//
// Created by TheDaChicken on 7/10/2025.
//

#include "vendors/AirSpyHf.h"

#include "../Utils.h"

PortSDR::AirSpyHfHost::AirSpyHfHost() : Host(AIRSPY_HF)
{
}

std::vector<PortSDR::Device> PortSDR::AirSpyHfHost::AvailableDevices() const
{
    std::vector<Device> devices;

    const int device_count = airspyhf_list_devices(nullptr, 0);
    if (device_count == 0)
        return {};

    uint64_t serials[device_count];
    int ret = airspyhf_list_devices(serials, device_count);
    if (ret < 0)
        return {};

    devices.resize(device_count);

    for (int i = 0; i < device_count; ++i)
    {
        auto& device = devices[i];

        device.serial = string_format("%016llX",
                                      serials[i]);
        device.host = shared_from_this();
    }
    return devices;
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

PortSDR::ErrorCode PortSDR::AirSpyHfStream::Initialize(const std::string_view index)
{
    if (m_device)
        return ErrorCode::INVALID_ARGUMENT;

    if (index.size() != 16)
        return ErrorCode::INVALID_ARGUMENT;

    const uint64_t num = strtoull(
        index.data(),
        nullptr,
        16);
    const int ret = airspyhf_open_sn(&m_device, num);
    if (ret != AIRSPYHF_SUCCESS)
    {
        return ErrorCode::UNKNOWN;
    }

    return ErrorCode::OK;
}

PortSDR::DeviceInfo PortSDR::AirSpyHfStream::GetUSBStrings()
{
    DeviceInfo device;
    airspyhf_read_partid_serialno_t read_partid_serialno;

    if (airspyhf_board_partid_serialno_read(
        m_device,
        &read_partid_serialno) == AIRSPYHF_SUCCESS)
    {
        device.serial = string_format("%08X%08X",
                                      read_partid_serialno.serial_no[2],
                                      read_partid_serialno.serial_no[3]);
        device.name += string_format(" SN: %s", device.serial.c_str());
    }

    return device;
}

PortSDR::ErrorCode PortSDR::AirSpyHfStream::Start()
{
    if (!m_device)
        return ErrorCode::INVALID_ARGUMENT;

    const int ret = airspyhf_start(m_device, AirSpySDRCallback, this);
    if (ret != AIRSPYHF_SUCCESS)
        return ErrorCode::UNKNOWN;
    return ErrorCode::OK;
}

PortSDR::ErrorCode PortSDR::AirSpyHfStream::Stop()
{
    if (!m_device)
        return ErrorCode::INVALID_ARGUMENT;

    const int ret = airspyhf_stop(m_device);
    if (ret != AIRSPYHF_SUCCESS)
        return ErrorCode::UNKNOWN;
    return ErrorCode::OK;
}

PortSDR::ErrorCode PortSDR::AirSpyHfStream::SetCenterFrequency(uint32_t freq)
{
    if (!m_device)
        return ErrorCode::INVALID_ARGUMENT;

    int ret = airspyhf_set_freq(m_device, freq);
    if (ret != AIRSPYHF_SUCCESS)
    {
        return ErrorCode::UNKNOWN;
    }

    m_freq = freq;
    return ErrorCode::OK;
}

PortSDR::ErrorCode PortSDR::AirSpyHfStream::SetSampleRate(uint32_t sampleRate)
{
    if (!m_device)
        return ErrorCode::INVALID_ARGUMENT;

    int ret = airspyhf_set_samplerate(m_device, sampleRate);
    if (ret != AIRSPYHF_SUCCESS)
    {
        return ErrorCode::UNKNOWN;
    }

    m_sampleRate = sampleRate;
    return ErrorCode::OK;
}

PortSDR::ErrorCode PortSDR::AirSpyHfStream::SetSampleFormat(SampleFormat format)
{
    if (!m_device)
        return ErrorCode::INVALID_ARGUMENT;

    if (format != getNativeSampleFormat())
        return ErrorCode::INVALID_ARGUMENT;
    // AirSpy HF does not support sample format changes

    return ErrorCode::OK;
}

PortSDR::ErrorCode PortSDR::AirSpyHfStream::SetGain(double gain, std::string_view name)
{
    if (!m_device)
        return ErrorCode::INVALID_ARGUMENT;

    if (name == "ATT")
    {
        return SetAttenuation(gain);
    }

    return ErrorCode::INVALID_ARGUMENT;
}

PortSDR::ErrorCode PortSDR::AirSpyHfStream::SetGainMode(const GainMode mode)
{
    if (mode == GAIN_MODE_FREE)
        return ErrorCode::OK;

    return ErrorCode::INVALID_ARGUMENT;
}

PortSDR::ErrorCode PortSDR::AirSpyHfStream::SetAttenuation(double attenuation)
{
    if (!m_device)
        return ErrorCode::INVALID_ARGUMENT;

    const int ret = airspyhf_set_hf_att(m_device, static_cast<int>(attenuation));
    if (ret != AIRSPYHF_SUCCESS)
        return ErrorCode::UNKNOWN;
    return ErrorCode::OK;
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

std::vector<PortSDR::Gain> PortSDR::AirSpyHfStream::GetGainStages(GainMode mode) const
{
    std::vector<Gain> gains;

    if (GAIN_MODE_FREE == mode)
    {
        gains.emplace_back("ATT", MetaRange{0, 8, 1});
    }

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

double PortSDR::AirSpyHfStream::GetGain(std::string_view name) const
{
    return 0;
}

PortSDR::GainMode PortSDR::AirSpyHfStream::GetGainMode() const
{
    return GAIN_MODE_FREE; // AirSpy HF does not support gain modes
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
