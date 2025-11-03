//
// Created by TheDaChicken on 7/1/2025.
//

#ifndef PORTSDR_HOST_H
#define PORTSDR_HOST_H

#include <memory>
#include <string_view>

#include "Device.h"
#include "Stream.h"

namespace PortSDR
{
    class Host
    {
    public:
        enum HostType
        {
            RTL_SDR,
            AIRSPY,
            AIRSPY_HF,
        };

        explicit Host(const HostType host_type) : type_(host_type)
        {
        }

        virtual ~Host() = default;

        [[nodiscard]] virtual std::vector<Device> AvailableDevices() const = 0;
        [[nodiscard]] virtual std::unique_ptr<Stream> CreateStream() const = 0;

        [[nodiscard]] HostType GetType() const
        {
            return type_;
        }

        [[nodiscard]] std::string_view GetTypeName() const
        {
            switch (type_)
            {
            case RTL_SDR: return "RTL-SDR";
            case AIRSPY: return "AirSpy";
            default:
                return "Unknown"; // Should not happen
            }
        }

        int CreateAndInitializeStream(const Device& device,
                                      std::unique_ptr<Stream>& stream) const;

    private:
        HostType type_;
    };
}

#endif //PORTSDR_HOST_H
