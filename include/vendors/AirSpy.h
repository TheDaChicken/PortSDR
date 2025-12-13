//
// Created by TheDaChicken on 12/19/2024.
//

#ifndef AIRSPY_H
#define AIRSPY_H

#include "../PortSDR.h"
#include "libairspy/airspy.h"

namespace PortSDR
{
    class AirSpyHost final : public Host
    {
    public:
        AirSpyHost();

        [[nodiscard]] std::vector<Device> AvailableDevices() const override;
        [[nodiscard]] std::unique_ptr<Stream> CreateStream() const override;
    };

    class AirSpyStream final : public Stream
    {
    public:
        ~AirSpyStream() override;

        ErrorCode Initialize(std::string_view index) override;

        DeviceInfo GetUSBStrings() override;

        ErrorCode Start() override;
        ErrorCode Stop() override;
        ErrorCode SetCenterFrequency(uint32_t freq) override;
        ErrorCode SetSampleRate(uint32_t sampleRate) override;
        ErrorCode SetSampleFormat(SampleFormat format) override;

        ErrorCode SetRegularGain(double gain);
        ErrorCode SetGain(double gain, std::string_view name) override;
        ErrorCode SetGainMode(GainMode mode) override;

        ErrorCode SetLnaGain(double gain);
        ErrorCode SetMixGain(double gain);
        ErrorCode SetIfGain(double gain);

        [[nodiscard]] std::vector<uint32_t> GetSampleRates() const override;
        [[nodiscard]] std::vector<SampleFormat> GetSampleFormats() const override;

        [[nodiscard]] std::vector<GainMode> GetGainModes() const override;
        [[nodiscard]] std::vector<Gain> GetGainStages() const override;
        [[nodiscard]] std::vector<Gain> GetGainStages(GainMode mode) const override;

        [[nodiscard]] uint32_t GetCenterFrequency() const override;
        [[nodiscard]] uint32_t GetSampleRate() const override;
        [[nodiscard]] double GetGain(std::string_view name) const override;
        [[nodiscard]] GainMode GetGainMode() const override;

    private:
        static int AirSpySDRCallback(airspy_transfer* transfer);
        static airspy_sample_type ConvertToSampleType(SampleFormat format) ;

    private:
        airspy_device* m_device = nullptr;
        SampleFormat m_sampleType = SAMPLE_FORMAT_IQ_FLOAT32;
        uint32_t m_sampleRate = 0;
        uint32_t m_freq = 0;
        uint8_t m_gain = 0;
        uint8_t m_lnaGain = 0;
        uint8_t m_mixGain = 0;
        uint8_t m_ifGain = 0;
        GainMode m_gainMode = GAIN_MODE_LINEARITY;
    };
}
#endif //AIRSPY_H
