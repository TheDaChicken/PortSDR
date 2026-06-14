//
// Created by TheDaChicken on 7/1/2025.
//

#ifndef PORTSDR_HOST_H
#define PORTSDR_HOST_H

#include <memory>

#include "Device.h"
#include "Stream.h"

namespace PortSDR
{
    class StreamImpl : public Stream
    {
    public:
        virtual ErrorCode Initialize(const Device& device) = 0;
    };

    class Host
    {
    public:
        explicit Host(const HostType hostType) : type_(hostType)
        {
        }

        virtual ~Host() = default;

        [[nodiscard]] virtual std::vector<Device> AvailableDevices() const = 0;
        [[nodiscard]] virtual std::unique_ptr<StreamImpl> CreateStream() const = 0;

        [[nodiscard]] HostType GetType() const
        {
            return type_;
        }

        ErrorCode CreateAndInitializeStream(
            const Device& device,
            std::unique_ptr<Stream>& stream) const;

    private:
        HostType type_;
    };
}

#endif //PORTSDR_HOST_H
