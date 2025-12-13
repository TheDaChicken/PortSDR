//
// Created by TheDaChicken on 7/10/2025.
//

#ifndef AIRSPYHF_H
#define AIRSPYHF_H

#include "Host.h"
#include <libairspyhf/airspyhf.h>

namespace PortSDR
{
    class AirSpyHfHost final : public Host
    {
    public:
        AirSpyHfHost();

        [[nodiscard]] std::vector<Device> AvailableDevices() const override;
        [[nodiscard]] std::unique_ptr<Stream> CreateStream() const override;
    };

    class AirSpyHfStream final : public Stream
    {
    public:
        ~AirSpyHfStream() override;

        ErrorCode Initialize(std::string_view index) override;
        DeviceInfo GetUSBStrings() override;

        ErrorCode Start() override;
        ErrorCode Stop() override;

        ErrorCode SetCenterFrequency(uint32_t freq) override;
        ErrorCode SetSampleRate(uint32_t sampleRate) override;
        ErrorCode SetSampleFormat(SampleFormat format) override;

        ErrorCode SetGain(double gain, std::string_view name) override;
        ErrorCode SetGainMode(GainMode mode) override;

        ErrorCode SetAttenuation(double attenuation);

        [[nodiscard]] std::vector<uint32_t> GetSampleRates() const override;
        [[nodiscard]] std::vector<SampleFormat> GetSampleFormats() const override;

        [[nodiscard]] std::vector<GainMode> GetGainModes() const override;
        [[nodiscard]] std::vector<Gain> GetGainStages(GainMode mode) const override;

        [[nodiscard]] uint32_t GetCenterFrequency() const override;
        [[nodiscard]] uint32_t GetSampleRate() const override;
        [[nodiscard]] double GetGain(std::string_view name) const override;
        [[nodiscard]] GainMode GetGainMode() const override;

    private:
        [[nodiscard]] static SampleFormat getNativeSampleFormat();

        static int AirSpySDRCallback(airspyhf_transfer_t* transfer);

    private:
        airspyhf_device *m_device = nullptr;

        uint32_t m_freq = 0;
        uint32_t m_sampleRate = 0;
    };
}

#endif //AIRSPYHF_H
