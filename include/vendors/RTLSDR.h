//
// Created by TheDaChicken on 12/19/2024.
//

#ifndef RTLSDR_H
#define RTLSDR_H

#include "PortSDR.h"
#include "rtl-sdr.h"

#include <thread>

namespace PortSDR
{
    class RTLHost final : public Host
    {
    public:
        RTLHost();

        void RefreshDevices() override;

        [[nodiscard]] const std::vector<std::shared_ptr<Device>>& Devices() const override;
        [[nodiscard]] std::unique_ptr<Stream> CreateStream() const override;

    private:
        std::vector<std::shared_ptr<Device>> devices_;
    };

    class RTLStream final : public Stream
    {
    public:
        ~RTLStream() override;

        int Initialize(const std::shared_ptr<Device>& device) override;
        int Start() override;
        int Stop() override;

        int SetCenterFrequency(uint32_t freq, int stream) override;
        int SetSampleRate(uint32_t freq) override;
        int SetSampleFormat(SampleFormat type) override;

        int SetGain(int gain, std::string_view name) override;
        int SetGain(int gain) override;
        int SetGainModes(std::string_view mode) override;

        int SetIfGain(int gain);

        [[nodiscard]] std::vector<uint32_t> GetSampleRates() const override;
        [[nodiscard]] std::vector<std::string> GetGainModes() const override;
        [[nodiscard]] std::vector<SampleFormat> GetSampleFormats() const override;

        [[nodiscard]] uint32_t GetCenterFrequency() const override;
        [[nodiscard]] uint32_t GetSampleRate() const override;
        [[nodiscard]] int GetGain() const override;
        [[nodiscard]] const std::string GetGainMode() const override;

        [[nodiscard]] Gain GetGainStage() const override;
        [[nodiscard]] std::vector<Gain> GetGainStages() const override;

    private:
        static void RTLSDRCallback(unsigned char* buf, uint32_t len, void* ctx);
        void Process();

        rtlsdr_dev_t* m_dev{nullptr};

        std::thread m_thread;

        SampleFormat m_sampleFormat{SAMPLE_FORMAT_IQ_UINT8};
        IQConverterUINT8ToINT16 uint8ToInt16;

        std::vector<char> m_outputBuffer;
    };
}

#endif //RTLSDR_H
