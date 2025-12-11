#include "PortSDR.h"
#include "PortSDRVersion.cpp"

#include <algorithm>
#include <iostream>

#include "Error.h"

#ifdef RTLSDR_SUPPORT
#include "vendors/RTLSDR.h"
#endif

#ifdef AIRSPY_SUPPORT
#include "vendors/AirSpy.h"
#endif

#ifdef AIRSPYHF_SUPPORT
#include "vendors/AirSpyHf.h"
#endif

std::string PortSDR::PortSDR::GetVersion()
{
    return kGitHash;
}

PortSDR::PortSDR::PortSDR()
{
    // Store all our supported SDR vendors
#ifdef RTLSDR_SUPPORT
    m_hosts.emplace_back(std::make_shared<RTLHost>());
#endif

#ifdef AIRSPY_SUPPORT
    m_hosts.emplace_back(std::make_shared<AirSpyHost>());
#endif

#ifdef AIRSPYHF_SUPPORT
    m_hosts.emplace_back(std::make_shared<AirSpyHfHost>());
#endif
}

std::vector<std::shared_ptr<PortSDR::Host>> PortSDR::PortSDR::GetHosts()
{
    return m_hosts;
}

std::shared_ptr<PortSDR::Host> PortSDR::PortSDR::GetHost(Host::HostType name)
{
    const auto iter =
        std::find_if(m_hosts.begin(), m_hosts.end(),
                     [name](const std::shared_ptr<Host>& key)
                     {
                         return key->GetType() == name;
                     });

    if (iter != m_hosts.end())
    {
        return *iter;
    }

    return {};
}

std::optional<PortSDR::Device> PortSDR::PortSDR::GetFirstAvailableSDR() const
{
    for (const auto& host : m_hosts)
    {
        const std::vector<Device> devices = host->AvailableDevices();
        if (!devices.empty())
        {
            return devices.front();
        }
    }
    return {};
}

std::vector<PortSDR::Device> PortSDR::PortSDR::GetDevices()
{
    std::vector<Device> total_devices;
    for (const auto& host : m_hosts)
    {
        const auto host_devices = host->AvailableDevices();
        total_devices.insert(
            total_devices.end(),
            host_devices.begin(), host_devices.end());
    }

    return total_devices;
}

ErrorCode PortSDR::Device::CreateStream(std::unique_ptr<Stream>& stream) const
{
    const auto h = host.lock();
    if (!h)
        return ErrorCode::HOST_UNAVAILABLE;

    return h->CreateAndInitializeStream(serial, stream);
}

ErrorCode PortSDR::Host::CreateAndInitializeStream(
    const std::string_view serial,
    std::unique_ptr<Stream>& stream) const
{
    auto new_stream = CreateStream();
    const ErrorCode ret = new_stream->Initialize(serial);

    if (ret < ErrorCode::OK)
    {
        return ret;
    }

    stream = std::move(new_stream);
    return ret;
}

double PortSDR::MetaRange::Max() const
{
    double max_stop = front().stop;
    for (const Range& r : (*this))
    {
        max_stop = std::max(max_stop, r.stop);
    }
    return max_stop;
}

double PortSDR::MetaRange::Min() const
{
    double min_start = front().start;
    for (const Range& r : *this)
    {
        min_start = std::min(min_start, r.start);
    }
    return min_start;
}

double PortSDR::MetaRange::Step() const
{
    std::vector<double> non_zero_steps;
    Range last = this->front();
    for (const Range& r : *this)
    {
        //steps at each range
        if (r.step > 0) non_zero_steps.push_back(r.step);
        //and steps in-between ranges
        double ibtw_step = r.start - last.stop;
        if (ibtw_step > 0) non_zero_steps.push_back(ibtw_step);
        //store ref to last
        last = r;
    }
    if (non_zero_steps.empty()) return 0; //all zero steps, its zero...
    return *std::min_element(non_zero_steps.begin(), non_zero_steps.end());
}
