//
// Created by TheDaChicken on 12/17/2024.
//

#include "vendors/AirSpy.h"
#include "libairspy/airspy.h"

#include <Utils.h>

#define AIRSPY_MAX_DEVICE 32

PortSDR::AirSpyHost::AirSpyHost() : Host(AIRSPY)
{
}

std::vector<PortSDR::Device> PortSDR::AirSpyHost::AvailableDevices() const
{
    std::vector<Device> devices;

    uint64_t serials[AIRSPY_MAX_DEVICE + 1];
    int device_count = airspy_list_devices(serials, AIRSPY_MAX_DEVICE);
    if (device_count < 0)
        return {};

    devices.resize(device_count);

    for (int i = 0; i < device_count; i++)
    {
        auto& device = devices[i];

        device.serial = string_format("%016llX",
                                      serials[i]);
        device.host = this;
    }
    return devices;
}

std::unique_ptr<PortSDR::Stream> PortSDR::AirSpyHost::CreateStream() const
{
    return std::make_unique<AirSpyStream>();
}

PortSDR::AirSpyStream::~AirSpyStream()
{
    if (m_device)
        airspy_close(m_device);
}

int PortSDR::AirSpyStream::Initialize(const std::string_view index)
{
    if (m_device)
        return 0;

    if (index.size() != 16)
        return -1;

    const uint64_t num = strtoull(
        index.data(),
        nullptr,
        16);

    int ret = airspy_open_sn(&m_device, num);
    if (ret != AIRSPY_SUCCESS)
        return ret;

    ret = SetSampleFormat(SAMPLE_FORMAT_IQ_INT16);
    if (ret != AIRSPY_SUCCESS)
    {
        return ret;
    }

    return 0;
}

PortSDR::DeviceInfo PortSDR::AirSpyStream::GetUSBStrings()
{
    DeviceInfo device;

    uint8_t board_id;
    airspy_read_partid_serialno_t read_partid_serialno;

    if (airspy_board_id_read(
        m_device, &board_id) == AIRSPY_SUCCESS)
    {
        device.name = airspy_board_id_name(static_cast<airspy_board_id>(board_id));
    }

    if (airspy_board_partid_serialno_read(
        m_device, &read_partid_serialno) == AIRSPY_SUCCESS)
    {
        device.serial = string_format("%08X%08X",
                                      read_partid_serialno.serial_no[2],
                                      read_partid_serialno.serial_no[3]);
        device.name += string_format(" SN: %s", device.serial.c_str());
    }

    return device;
}

int PortSDR::AirSpyStream::Start()
{
    if (!m_device)
        return -1;

    return airspy_start_rx(m_device, AirSpySDRCallback, this);
}

int PortSDR::AirSpyStream::Stop()
{
    if (!m_device)
        return -1;

    if (airspy_is_streaming(m_device) != AIRSPY_TRUE)
        return -2;

    return airspy_stop_rx(m_device);
}

int PortSDR::AirSpyStream::SetCenterFrequency(uint32_t freq)
{
    if (!m_device)
        return -1;

    int ret = airspy_set_freq(m_device, freq);
    if (ret == AIRSPY_SUCCESS)
    {
        m_freq = freq;
    }
    return ret;
}

int PortSDR::AirSpyStream::SetSampleRate(uint32_t sampleRate)
{
    if (!m_device)
        return -1;

    int ret = airspy_set_samplerate(m_device, sampleRate);
    if (ret == AIRSPY_SUCCESS)
    {
        m_sampleRate = sampleRate;
    }

    return ret;
}

int PortSDR::AirSpyStream::SetGain(double gain)
{
    if (!m_device)
        return -1;

    int ret = AIRSPY_ERROR_INVALID_PARAM;
    if (m_gainMode == LINEARITY)
    {
        ret = airspy_set_linearity_gain(m_device, static_cast<uint8_t>(gain));
    }
    else if (m_gainMode == SENSITIVITY)
    {
        ret = airspy_set_sensitivity_gain(m_device, static_cast<uint8_t>(gain));
    }

    if (ret == AIRSPY_SUCCESS)
    {
        m_gain = gain;
    }
    return ret;
}

int PortSDR::AirSpyStream::SetSampleFormat(SampleFormat format)
{
    if (!m_device)
        return -1;

    const airspy_sample_type sampleType = ConvertToSampleType(format);
    if (sampleType == AIRSPY_SAMPLE_END)
        return -1; // Invalid sample format

    const int ret = airspy_set_sample_type(m_device, sampleType);
    if (ret != AIRSPY_SUCCESS)
        return ret;

    m_sampleType = format;
    return 0;
}

int PortSDR::AirSpyStream::SetLnaGain(double gain)
{
    if (!m_device)
        return -1;

    int ret = airspy_set_lna_gain(m_device, static_cast<uint8_t>(gain));
    if (ret == AIRSPY_SUCCESS)
    {
        m_lnaGain = static_cast<uint8_t>(gain);
    }
    return ret;
}

int PortSDR::AirSpyStream::SetMixGain(double gain)
{
    if (!m_device)
        return -1;

    int ret = airspy_set_mixer_gain(m_device, static_cast<uint8_t>(gain));
    if (ret == AIRSPY_SUCCESS)
    {
        m_mixGain = static_cast<uint8_t>(gain);
    }
    return ret;
}

int PortSDR::AirSpyStream::SetIfGain(double gain)
{
    if (!m_device)
        return -1;

    int ret = airspy_set_vga_gain(m_device, static_cast<uint8_t>(gain));
    if (ret == AIRSPY_SUCCESS)
    {
        m_ifGain = static_cast<uint8_t>(gain);
    }
    return ret;
}

int PortSDR::AirSpyStream::SetGain(double gain, std::string_view name)
{
    if ("LNA" == name)
    {
        return SetLnaGain(gain);
    }
    if ("MIX" == name)
    {
        return SetMixGain(gain);
    }
    if ("IF" == name)
    {
        return SetIfGain(gain);
    }
    return -1;
}

int PortSDR::AirSpyStream::SetGainModes(std::string_view name)
{
    if ("LINEARITY" == name)
    {
        m_gainMode = LINEARITY;
        return 0;
    }
    if ("SENSITIVITY" == name)
    {
        m_gainMode = SENSITIVITY;
        return 0;
    }
    return -1;
}

std::vector<uint32_t> PortSDR::AirSpyStream::GetSampleRates() const
{
    if (!m_device)
        return {};

    std::vector<uint32_t> sampleRates;
    uint32_t count;

    int ret = airspy_get_samplerates(m_device, &count, 0);
    if (ret != AIRSPY_SUCCESS)
    {
        return {};
    }

    if (count > 0)
    {
        sampleRates.resize(count);
        airspy_get_samplerates(m_device, sampleRates.data(), count);
    }

    return sampleRates;
}

std::vector<std::string> PortSDR::AirSpyStream::GetGainModes() const
{
    return {"LINEARITY", "SENSITIVITY"};
}

PortSDR::Gain PortSDR::AirSpyStream::GetGainStage() const
{
    return {"Gain", MetaRange(0, 21, 1)};
}

std::vector<PortSDR::Gain> PortSDR::AirSpyStream::GetGainStages() const
{
    std::vector<Gain> ranges;

    ranges.emplace_back("LNA", MetaRange{0, 15, 1});
    ranges.emplace_back("MIX", MetaRange{0, 15, 1});
    ranges.emplace_back("IF", MetaRange{0, 15, 1});

    return ranges;
}

std::vector<PortSDR::SampleFormat> PortSDR::AirSpyStream::GetSampleFormats() const
{
    return {SAMPLE_FORMAT_IQ_INT16, SAMPLE_FORMAT_IQ_FLOAT32};
}

uint32_t PortSDR::AirSpyStream::GetCenterFrequency() const
{
    return m_freq;
}

uint32_t PortSDR::AirSpyStream::GetSampleRate() const
{
    return m_sampleRate;
}

double PortSDR::AirSpyStream::GetGain() const
{
    return m_gain;
}

double PortSDR::AirSpyStream::GetGain(std::string_view name) const
{
    if (name == "LNA")
        return m_lnaGain;
    if (name == "MIX")
        return m_mixGain;
    if (name == "IF")
        return m_ifGain;
    return 0.0;
}

std::string PortSDR::AirSpyStream::GetGainMode() const
{
    return m_gainMode == LINEARITY ? "LINEARITY" : "SENSITIVITY";
}

airspy_sample_type PortSDR::AirSpyStream::ConvertToSampleType(const SampleFormat format)
{
    switch (format)
    {
    case SAMPLE_FORMAT_IQ_INT16:
        return AIRSPY_SAMPLE_INT16_IQ;
    case SAMPLE_FORMAT_IQ_FLOAT32:
        return AIRSPY_SAMPLE_FLOAT32_IQ;
    default:
        return AIRSPY_SAMPLE_END;
    }
}

int PortSDR::AirSpyStream::AirSpySDRCallback(airspy_transfer* transfer)
{
    const auto* obj = static_cast<AirSpyStream*>(transfer->ctx);

    SDRTransfer sdr_transfer{};
    sdr_transfer.data = transfer->samples;
    sdr_transfer.frame_size = transfer->sample_count;
    sdr_transfer.dropped_samples = transfer->dropped_samples;
    sdr_transfer.format = obj->m_sampleType;

    obj->m_callback(sdr_transfer);
    return 0;
}

