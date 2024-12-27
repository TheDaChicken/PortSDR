#include "PortSDR.h"
#include "PortSDRVersion.cpp"

#include <algorithm>
#include <iostream>

#ifdef RTLSDR_SUPPORT
#include "RTLSDR.h"
#endif

#ifdef AIRSPY_SUPPORT
#include "AirSpy.h"
#endif

std::vector<std::shared_ptr<PortSDR::Host>> m_hosts;

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
}

std::vector<std::shared_ptr<PortSDR::Host>> PortSDR::PortSDR::GetHosts()
{
    return m_hosts;
}

std::shared_ptr<PortSDR::Host> PortSDR::PortSDR::FindHost(std::string_view name)
{
    const auto iter = std::find_if(m_hosts.begin(), m_hosts.end(),
                                   [name](const std::shared_ptr<Host>& key)
                                   {
                                       return key->name == name;
                                   });

    if (iter != m_hosts.end())
    {
        return *iter;
    }

    return {};
}

std::optional<PortSDR::Device> PortSDR::PortSDR::GetFirstAvailableSDR()
{
    for (const auto& host : m_hosts)
    {
        const std::vector<Device>& devices = host->Devices();
        if (!devices.empty())
        {
            return devices.front();
        }
    }
    return {};
}

std::vector<PortSDR::Device> PortSDR::PortSDR::GetDevices()
{
    std::vector<Device> devices;
    for (const auto& host : m_hosts)
    {
        const std::vector<Device>& host_devices = host->Devices();
        devices.insert(devices.end(), host_devices.begin(), host_devices.end());
    }

    return devices;
}

double PortSDR::MetaRange::max() const
{
    double max_stop = this->front().stop;
    for (const Range& r : (*this))
    {
        max_stop = std::max(max_stop, r.stop);
    }
    return max_stop;
}

double PortSDR::MetaRange::min() const
{
    double min_start = this->front().start;
    for (const Range& r : *this)
    {
        min_start = std::min(min_start, r.start);
    }
    return min_start;
}

int PortSDR::Device::CreateStream(std::unique_ptr<Stream>& stream) const
{
    if (!host)
        return {};

    return host->CreateAndInitializeStream(*this, stream);
}

double PortSDR::MetaRange::step() const
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

