//
// Created by TheDaChicken on 7/1/2025.
//

#ifndef PORTSDR_DEVICE_H
#define PORTSDR_DEVICE_H

#include <string>

#include "HostType.h"

namespace PortSDR
{
    class Host;
    class Stream;

    struct Device
    {
        HostType type;
        std::string serial;
    };

    struct DeviceInfo
    {
        std::string name;
        std::string serial;
    };
}

#endif //PORTSDR_DEVICE_H
