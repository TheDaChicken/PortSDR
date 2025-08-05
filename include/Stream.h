//
// Created by TheDaChicken on 7/1/2025.
//

#ifndef PORTSDR_STREAM_H
#define PORTSDR_STREAM_H

#include <functional>

#include "Ranges.h"
#include <cstdint>

namespace PortSDR
{
    struct Device;

    enum SampleFormat
    {
        SAMPLE_FORMAT_IQ_UINT8,
        SAMPLE_FORMAT_IQ_INT16,
        SAMPLE_FORMAT_IQ_FLOAT32,
    };

    struct SDRTransfer
    {
        void* data;
        std::size_t frame_size;
        std::size_t dropped_samples; // Some APIs may report dropped samples
        SampleFormat format; // Sample format of the data
    };

    class Stream
    {
    public:
        using SDR_CALLBACK = std::function<void(SDRTransfer& transfer)>;

        Stream() = default;
        virtual ~Stream() = default;

        virtual int Initialize(const std::shared_ptr<Device>& device) = 0;

        virtual int Start() = 0;
        virtual int Stop() = 0;

        virtual int SetSampleRate(uint32_t sampleRate) = 0;
        virtual int SetCenterFrequency(uint32_t freq, int stream) = 0;
        virtual int SetSampleFormat(SampleFormat format) = 0;

        virtual int SetGain(double gain) = 0;
        virtual int SetGain(double gain, std::string_view name) = 0;
        virtual int SetGainModes(std::string_view name) = 0;

        virtual std::vector<uint32_t> GetSampleRates() const = 0;
        virtual std::vector<std::string> GetGainModes() const = 0;
        virtual std::vector<SampleFormat> GetSampleFormats() const = 0;

        [[nodiscard]] virtual Gain GetGainStage() const = 0;
        [[nodiscard]] virtual std::vector<Gain> GetGainStages() const = 0;

        [[nodiscard]] virtual uint32_t GetCenterFrequency() const = 0;
        [[nodiscard]] virtual uint32_t GetSampleRate() const = 0;
        [[nodiscard]] virtual int GetGain() const = 0;
        [[nodiscard]] virtual const std::string GetGainMode() const = 0;

        int SetCallback(SDR_CALLBACK sdr_callback)
        {
            m_callback = std::move(sdr_callback);
            return 0;
        }

    protected:
        SDR_CALLBACK m_callback;
    };
}

#endif //PORTSDR_STREAM_H
