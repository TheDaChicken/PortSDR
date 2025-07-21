#ifndef PORTSDR_LIBRARY_H
#define PORTSDR_LIBRARY_H

#include <memory>
#include <string>
#include <vector>
#include <cstdint>

#include "Host.h"

namespace PortSDR
{
    class PortSDR
    {
    public:
        static std::string GetVersion();

        PortSDR();

        std::shared_ptr<Device> GetFirstAvailableSDR();

        std::vector<std::shared_ptr<Host>> GetHosts();
        std::vector<std::shared_ptr<Device>> GetDevices();

        std::shared_ptr<Host> GetHost(Host::HostType name);
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
}
#endif //PORTSDR_LIBRARY_H
