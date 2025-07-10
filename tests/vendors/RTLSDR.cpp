//
// Created by TheDaChicken on 12/15/2024.
//

#include <algorithm>
#include <thread>
#include <gtest/gtest.h>

#include "PortSDR.h"

TEST(RTLSDR, Devices)
{
    PortSDR::PortSDR portSDR;
    std::shared_ptr<PortSDR::Host> sdrHost = portSDR.GetHost(PortSDR::Host::RTL_SDR);

    ASSERT_TRUE(sdrHost);

    const std::vector<std::shared_ptr<PortSDR::Device>>& devices = sdrHost->Devices();

    ASSERT_FALSE(devices.empty()) << "No devices found";

    std::cout << "Found ";
    std::cout << devices.size();
    std::cout << " devices" << std::endl;

    for (const auto& device : devices)
    {
        std::cout << device->name << std::endl;
    }
}

TEST(RTLSDR, Stream)
{
    PortSDR::PortSDR portSDR;
    std::shared_ptr<PortSDR::Host> sdrHost = portSDR.GetHost(PortSDR::Host::RTL_SDR);

    ASSERT_TRUE(sdrHost);

    const std::vector<std::shared_ptr<PortSDR::Device>>& devices = sdrHost->Devices();

    ASSERT_TRUE(!devices.empty());

    const auto& device = devices.front();
    auto stream = sdrHost->CreateStream();

    ASSERT_EQ(stream->Initialize(device), 0) << "Failed to initialize stream";

    // Test callback
    ASSERT_EQ(stream->SetCallback([](const PortSDR::SDRTransfer& sdr)
                  {
                  std::cout << "Received " << sdr.frame_size << " samples" << std::endl;
                  }),
              0) << "Failed to set callback";


    ASSERT_EQ(stream->SetSampleFormat(PortSDR::SAMPLE_FORMAT_IQ_INT16), 0) << "Failed to set sample format";
    ASSERT_EQ(stream->Start(), 0) << "Failed to start stream";

    std::this_thread::sleep_for(std::chrono::seconds{1});

    ASSERT_EQ(stream->Stop(), 0) << "Failed to stop stream";
}
