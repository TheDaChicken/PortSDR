//
// Created by TheDaChicken on 6/13/2026.
//

#ifndef PORTSDR_HOSTTYPE_H
#define PORTSDR_HOSTTYPE_H

#include <string_view>

namespace PortSDR
{
    enum class HostType
    {
        RTL_SDR,
        AIRSPY,
        AIRSPY_HF,
    };


    constexpr std::string_view ToString(const HostType type)
    {
        switch (type)
        {
        case HostType::RTL_SDR:
            return "RTL-SDR";

        case HostType::AIRSPY:
            return "AirSpy";

        case HostType::AIRSPY_HF:
            return "AirSpy HF";
        }

        return "Unknown";
    }
}

#endif //PORTSDR_HOSTTYPE_H
