#ifndef PORTSDR_LIBRARY_H
#define PORTSDR_LIBRARY_H

#include <memory>
#include <string>
#include <vector>
#include <cstdint>
#include <optional>

#include "Host.h"

namespace PortSDR
{
    class PortSDR
    {
    public:
        static std::string GetVersion();

        PortSDR();

        std::optional<Device> GetFirstAvailableSDR() const;

        std::vector<std::shared_ptr<Host>> GetHosts();
        std::vector<Device> GetDevices();

        std::shared_ptr<Host> GetHost(Host::HostType name);
    private:
        std::vector<std::shared_ptr<Host>> m_hosts;
    };
}
#endif //PORTSDR_LIBRARY_H
