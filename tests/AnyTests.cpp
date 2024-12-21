//
// Created by TheDaChicken on 12/19/2024.
//

#include <PortSDR.h>
#include <gtest/gtest.h>

TEST(AnyTest, Stream)
{
    PortSDR::PortSDR sdr;

    // Get the first available SDR device
    const std::shared_ptr<PortSDR::Device> device = sdr.GetFirstAvailableSDR();

    ASSERT_TRUE(device) << "No devices found";

    std::cout << "Device: " << device->name << std::endl;
    std::cout << "Serial: " << device->serial << std::endl;

    // Open the device
    std::shared_ptr<PortSDR::Stream> stream;

    int ret = device->CreateStream(stream);
    if (ret != 0)
    {
        std::cerr << "Failed to open the device" << std::endl;
        return;
    }

    // Print the capabilities
    std::cout << "Sample rates: ";
    for (auto rate : stream->GetSampleRates())
    {
        std::cout << rate << " ";
    }
    std::cout << std::endl;

    std::cout << "Gains: ";
    for (auto gain : stream->GetGainRanges())
    {
        std::cout << gain.stage << " ";
        std::cout << gain.range.min() << " ";
        std::cout << gain.range.max() << " ";
    }
    std::cout << std::endl;

    ASSERT_TRUE(device);
}
