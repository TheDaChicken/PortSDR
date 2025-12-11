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

        [[nodiscard]] std::vector<Device> AvailableDevices() const override;
        [[nodiscard]] std::unique_ptr<Stream> CreateStream() const override;
    };

    class RTLStream final : public Stream
    {
    public:
        ~RTLStream() override;

        ErrorCode Initialize(std::string_view serial) override;
        DeviceInfo GetUSBStrings() override;

        ErrorCode Start() override;
        ErrorCode Stop() override;

        ErrorCode SetCenterFrequency(uint32_t freq) override;
        ErrorCode SetSampleRate(uint32_t freq) override;
        ErrorCode SetSampleFormat(SampleFormat type) override;

        ErrorCode SetGain(double gain, std::string_view name) override;
        ErrorCode SetGain(double gain) override;
        ErrorCode SetGainModes(std::string_view mode) override;

        ErrorCode SetIfGain(double gain);

        [[nodiscard]] std::vector<uint32_t> GetSampleRates() const override;
        [[nodiscard]] std::vector<std::string> GetGainModes() const override;
        [[nodiscard]] std::vector<SampleFormat> GetSampleFormats() const override;

        [[nodiscard]] uint32_t GetCenterFrequency() const override;
        [[nodiscard]] uint32_t GetSampleRate() const override;
        [[nodiscard]] double GetGain() const override;
        [[nodiscard]] double GetGain(std::string_view name) const override;
        [[nodiscard]] std::string GetGainMode() const override;

        [[nodiscard]] Gain GetGainStage() const override;
        [[nodiscard]] std::vector<Gain> GetGainStages() const override;

    private:
        static void RTLSDRCallback(unsigned char* buf, uint32_t len, void* ctx);
        void Process();

    private:
        rtlsdr_dev_t* m_dev{nullptr};
        std::thread m_thread;
    };
}

#endif //RTLSDR_H
