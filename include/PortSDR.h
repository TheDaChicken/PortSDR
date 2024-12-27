#ifndef PORTSDR_LIBRARY_H
#define PORTSDR_LIBRARY_H

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace PortSDR
{
    class Host;
    class Stream;

    struct Device
    {
        Host* host; // TODO: Change to std::shared_ptr<Host> or std::weak_ptr<Host> instead to avoid issues

        std::string name;
        std::string serial;
        uint64_t index;

        int CreateStream(std::unique_ptr<Stream>& stream) const;
    };

    struct Range
    {
        explicit Range(double value = 0)
            : start(value), stop(value), step(0)
        {
        }

        Range(double start, double stop, double step)
            : start(start), stop(stop), step(step)
        {
        }

        double start, stop, step;
    };

    /*!
     * A meta-range object holds a list of individual ranges.
     */
    class MetaRange : public std::vector<Range>
    {
    public:
        MetaRange() : std::vector<Range>()
        {
        }

        MetaRange(double start, double stop, double step)
            : std::vector<Range>(1, Range(start, stop, step))
        {
        }

        /*!
        * Input iterator constructor:
        * Makes boost::assign::list_of work.
        * \param first the beginning iterator
        * \param last the end iterator
        */
        template <typename InputIterator>
        MetaRange(InputIterator first, InputIterator last):
            std::vector<Range>(first, last)
        {
            /* NOP */
        }

        [[nodiscard]] double step() const;
        [[nodiscard]] double min() const;
        [[nodiscard]] double max() const;
    };

    struct Gain
    {
        Gain(std::string stage, MetaRange range)
            : stage(std::move(stage)), range(std::move(range))
        {
        };

        Gain() = default;

        std::string stage;
        MetaRange range;
    };

    enum SampleFormat
    {
        SAMPLE_FORMAT_UINT8,
        SAMPLE_FORMAT_INT16,
        SAMPLE_FORMAT_FLOAT32,
    };

    class IQConverterUINT8ToINT16
    {
    public:
        IQConverterUINT8ToINT16();

        void Process(
            const uint8_t* data,
            int16_t* output, std::size_t elementSize) const;

    private:
        std::vector<int16_t> m_lut;
    };

    class IQConverterINT16ToFLOAT
    {
    public:
        IQConverterINT16ToFLOAT();

        void Process(const uint8_t* data, float* output, std::size_t elementSize) const;
    private:
        std::vector<float> m_lut;
    };

    class PortSDR
    {
    public:
        static std::string GetVersion();

        PortSDR();

        std::vector<std::shared_ptr<Host>> GetHosts();
        std::vector<Device> GetDevices();

        std::shared_ptr<Host> FindHost(std::string_view name);

        std::optional<Device> GetFirstAvailableSDR();
    };

    class Stream
    {
    public:
        using SDR_CALLBACK = std::function<void(const void* data, std::size_t elementSize)>;

        Stream() = default;
        virtual ~Stream() = default;

        virtual int Initialize(const Device& device) = 0;

        virtual int Start() = 0;
        virtual int Stop() = 0;

        virtual int SetSampleRate(uint32_t sampleRate) = 0;
        virtual int SetCenterFrequency(uint32_t freq, int stream) = 0;
        virtual int SetSampleFormat(SampleFormat format) = 0;

        virtual int SetGain(int gain) = 0;
        virtual int SetGain(int gain, std::string_view name) = 0;
        virtual int SetGainModes(std::string_view name) = 0;

        virtual std::vector<uint32_t> GetSampleRates() const = 0;
        virtual std::vector<std::string> GetGainModes() const = 0;
        virtual std::vector<SampleFormat> GetSampleFormats() const = 0;

        [[nodiscard]] virtual Gain GetGainRange() const = 0;
        [[nodiscard]] virtual std::vector<Gain> GetGainRanges() const = 0;

        [[nodiscard]] virtual uint32_t GetCenterFrequency() const = 0;
        [[nodiscard]] virtual uint32_t GetSampleRate() const = 0;
        [[nodiscard]] virtual int GetGain() const = 0;

        int SetCallback(SDR_CALLBACK sdr_callback)
        {
            m_callback = std::move(sdr_callback);
            return 0;
        }

    protected:
        SDR_CALLBACK m_callback;
    };

    class Host
    {
    public:
        virtual ~Host() = default;

        virtual void RefreshDevices() = 0;

        [[nodiscard]] virtual const std::vector<Device>& Devices() const = 0;
        [[nodiscard]] virtual std::unique_ptr<Stream> CreateStream() const = 0;

        int CreateAndInitializeStream(const Device& device,
                                      std::unique_ptr<Stream>& stream) const
        {
            auto newStream = CreateStream();
            const int ret = newStream->Initialize(device);

            if (ret < 0)
            {
                return ret; // Initialization failed, stream is not assigned
            }

            stream = std::move(newStream); // Transfer ownership to the caller
            return ret;
        }

        std::string name;
    };
}
#endif //PORTSDR_LIBRARY_H
