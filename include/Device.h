//
// Created by TheDaChicken on 7/1/2025.
//

#ifndef PORTSDR_DEVICE_H
#define PORTSDR_DEVICE_H

#include <cstdint>
#include <string>

namespace PortSDR
{
    class Host;
    class Stream;

    struct Device
    {
        uint32_t index;
        const Host* host;

        int CreateStream(std::unique_ptr<Stream>& stream) const;
    };

    struct DeviceInfo
    {
        std::string name;
        std::string serial;
    };
}

#endif //PORTSDR_DEVICE_H
