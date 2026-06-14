#ifndef PORTSDR_LIBRARY_H
#define PORTSDR_LIBRARY_H

#include <memory>
#include <string>
#include <vector>
#include <optional>

#include "Device.h"
#include "HostType.h"
#include "Stream.h"

namespace PortSDR
{
    class Host;

    class PortSDR
    {
    public:
        /**
         * Gets the current git commit version of PortSDR.
         * @return version.
         */
        static std::string GetVersion();

        /**
         * Constructs a new PortSDR instance with all available hosts.
         */
        PortSDR();

        ~PortSDR();

        /**
         * Gathers the first available SDR device based on the first host.
         * @return SDR device, or empty if no devices are found.
         */
        [[nodiscard]] std::optional<Device> GetFirstAvailableSDR() const;

        /**
         * Gathers all available SDR devices.
         * @return SDR devices.
         */
        [[nodiscard]] std::vector<Device> GetDevices() const;

        /**
         * Gets list of devices for a specific Host API.
         * @param type type of Host API.
         * @return list of devices.
         */
        [[nodiscard]] std::vector<Device> GetHostDevices(HostType type) const;

        /**
         * Creates stream from device.
         * @param device SDR device.
         * @param stream stream.
         * @return error code {@link ErrorCode}.
         */
        [[nodiscard]] ErrorCode CreateStream(const Device& device, std::unique_ptr<Stream>& stream) const;
    private:
        [[nodiscard]] const Host* GetHost(HostType type) const;
        std::vector<std::unique_ptr<Host>> m_hosts;
    };
}
#endif //PORTSDR_LIBRARY_H
