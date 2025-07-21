//
// Created by TheDaChicken on 7/1/2025.
//

#ifndef PORTSDR_DEVICE_H
#define PORTSDR_DEVICE_H

#include <string>
#include <cstdint>

namespace PortSDR
{
    class Host;
    class Stream;

    struct Device
    {
        std::string name;
        std::string serial;
        uint64_t index;

        Host* host; // TODO: Change to std::shared_ptr<Host> or std::weak_ptr<Host> instead to avoid issues
        bool unavailable;

        int CreateStream(std::unique_ptr<Stream>& stream) const;
    };
}

#endif //PORTSDR_DEVICE_H
