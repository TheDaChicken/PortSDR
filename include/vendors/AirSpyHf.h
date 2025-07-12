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

        void RefreshDevices() override;

        [[nodiscard]] const std::vector<std::shared_ptr<Device>>& Devices() const override;
        [[nodiscard]] std::unique_ptr<Stream> CreateStream() const override;

    private:
        std::vector<std::shared_ptr<Device>> devices_;
    };

    class AirSpyHfStream final : public Stream
    {
    public:
        ~AirSpyHfStream() override;

        int Initialize(const std::shared_ptr<Device>& device) override;

        int Start() override;
        int Stop() override;
        int SetCenterFrequency(uint32_t freq, int stream) override;
        int SetSampleRate(uint32_t sampleRate) override;
        int SetGain(int gain) override;
        int SetSampleFormat(SampleFormat format) override;

        int SetGain(int gain, std::string_view name) override;
        int SetGainModes(std::string_view name) override;

        int SetAttenuation(int attenuation);

        [[nodiscard]] std::vector<uint32_t> GetSampleRates() const override;
        [[nodiscard]] std::vector<SampleFormat> GetSampleFormats() const override;

        [[nodiscard]] std::vector<std::string> GetGainModes() const override;
        [[nodiscard]] Gain GetGainStage() const override;
        [[nodiscard]] std::vector<Gain> GetGainStages() const override;

        [[nodiscard]] uint32_t GetCenterFrequency() const override;
        [[nodiscard]] uint32_t GetSampleRate() const override;
        [[nodiscard]] int GetGain() const override;
        [[nodiscard]] const std::string GetGainMode() const override;

    private:
        [[nodiscard]] static SampleFormat getNativeSampleFormat();

        static int AirSpySDRCallback(airspyhf_transfer_t* transfer);

        airspyhf_device *m_device = nullptr;

        uint32_t m_freq = 0;
        uint32_t m_sampleRate = 0;
    };
}

#endif //AIRSPYHF_H
