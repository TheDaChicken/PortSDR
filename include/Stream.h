//
// Created by TheDaChicken on 7/1/2025.
//

#ifndef PORTSDR_STREAM_H
#define PORTSDR_STREAM_H

#include <cstdint>
#include <functional>

#include "Ranges.h"

namespace PortSDR
{
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

        /**
         * Opens device
         * @param serial serial of the device.
         * @return status code.
         */
        virtual int Initialize(std::string_view serial) = 0;

        /**
         * Opens device
         * @param device device container
         * @return status code.
         */
        int Initialize(const Device& device)
        {
            return Initialize(device.serial);
        };

        /**
         * Gets
         * @return
         */
        virtual DeviceInfo GetUSBStrings() = 0;

        /**
         * Starts streaming of the SDR samples
         * This creates an underlying thread.
         * @return ret code
         */
        virtual int Start() = 0;
        virtual int Stop() = 0;

        /**
         * Sets sample rate of the given SDR hardware
         * @param sampleRate new sample rate
         * @return ret code
         */
        virtual int SetSampleRate(uint32_t sampleRate) = 0;

        /**
         * Sets current center frequency for given hardware
         * @param freq in Hz
         * @return ret code
         */
        virtual int SetCenterFrequency(uint32_t freq) = 0;
        virtual int SetSampleFormat(SampleFormat format) = 0;

        virtual int SetGain(double gain) = 0;
        virtual int SetGain(double gain, std::string_view name) = 0;
        virtual int SetGainModes(std::string_view name) = 0;

        /**
         * Gets all sample rates supported by given SDR hardware.
         * @return vector of sample rates
         */
        [[nodiscard]] virtual std::vector<uint32_t> GetSampleRates() const = 0;

        /**
         * Gets all possible sample rates given by API.
         * TODO: is this needed? Some APIs may not have downsampling etc
         *  and we may still want to downsample.
         * @return vector of sample rates
         */
        [[nodiscard]] virtual std::vector<SampleFormat> GetSampleFormats() const = 0;

        [[nodiscard]] virtual std::vector<std::string> GetGainModes() const = 0;

        /**
         * Gets default gain stage used in SetGain()
         * @return gain
         */
        [[nodiscard]] virtual Gain GetGainStage() const = 0;

        /**
         * Gets all gain stages for the underlying radio hardware.
         * @return vector of gains containing the name of the gain and its range.
         */
        [[nodiscard]] virtual std::vector<Gain> GetGainStages() const = 0;

        [[nodiscard]] virtual uint32_t GetCenterFrequency() const = 0;
        [[nodiscard]] virtual uint32_t GetSampleRate() const = 0;

        /**
         * Gives the current gain of SDR device.
         * @return the actual gain in dB
         */
        [[nodiscard]] virtual double GetGain() const = 0;

        /**
         * Gives current gain of SDR device.
         * @param name Name of gain stage
         * @return the actual gain in dB
         */
        [[nodiscard]] virtual double GetGain(std::string_view name) const = 0;

        [[nodiscard]] virtual std::string GetGainMode() const = 0;

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
