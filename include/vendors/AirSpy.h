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

        void RefreshDevices() override;

        [[nodiscard]] const std::vector<std::shared_ptr<Device>>& Devices() const override;
        [[nodiscard]] std::unique_ptr<Stream> CreateStream() const override;

    private:
        std::vector<std::shared_ptr<Device>> devices_;
    };

    class AirSpyStream final : public Stream
    {
    public:
        ~AirSpyStream() override;

        int Initialize(const std::shared_ptr<Device>& device) override;

        int Start() override;
        int Stop() override;
        int SetCenterFrequency(uint32_t freq, int stream) override;
        int SetSampleRate(uint32_t sampleRate) override;
        int SetGain(double gain) override;
        int SetSampleFormat(SampleFormat format) override;

        int SetGain(double gain, std::string_view name) override;
        int SetGainModes(std::string_view name) override;

        int SetLnaGain(double gain);
        int SetMixGain(double gain);
        int SetIfGain(double gain);

        [[nodiscard]] std::vector<uint32_t> GetSampleRates() const override;
        [[nodiscard]] std::vector<SampleFormat> GetSampleFormats() const override;

        [[nodiscard]] std::vector<std::string> GetGainModes() const override;
        [[nodiscard]] Gain GetGainStage() const override;
        [[nodiscard]] std::vector<Gain> GetGainStages() const override;

        [[nodiscard]] uint32_t GetCenterFrequency() const override;
        [[nodiscard]] uint32_t GetSampleRate() const override;
        [[nodiscard]] double GetGain() const override;
        [[nodiscard]] double GetGain(std::string_view name) const override;
        [[nodiscard]] const std::string GetGainMode() const override;

    private:
        static int AirSpySDRCallback(airspy_transfer* transfer);
        static airspy_sample_type ConvertToSampleType(SampleFormat format) ;

        airspy_device* m_device = nullptr;
        SampleFormat m_sampleType = SAMPLE_FORMAT_IQ_FLOAT32;
        uint32_t m_sampleRate = 0;
        uint32_t m_freq = 0;
        uint8_t m_gain = 0;
        uint8_t m_lnaGain = 0;
        uint8_t m_mixGain = 0;
        uint8_t m_ifGain = 0;

        enum GainMode
        {
            LINEARITY,
            SENSITIVITY
        } m_gainMode = LINEARITY;
    };
}
#endif //AIRSPY_H
