//
// Created by TheDaChicken on 12/19/2024.
//

#include <PortSDR.h>
#include <gtest/gtest.h>

TEST(AnyTest, Devices)
{
    PortSDR::PortSDR sdr;

    // Get the first available SDR device
    const std::optional<PortSDR::Device> device = sdr.GetFirstAvailableSDR();

    ASSERT_TRUE(device) << "No devices found";

    std::cout << "Serial: " << device->serial << std::endl;

    // Open the device
    std::unique_ptr<PortSDR::Stream> stream;

    PortSDR::ErrorCode ret = device->CreateStream(stream);
    if (ret != PortSDR::ErrorCode::OK)
    {
        std::cerr << "Failed to open the device" << std::endl;
        return;
    }

    // Print the Serial
    std::cout << "Name: " << stream->GetUSBStrings().name << std::endl;

    // Print the capabilities
    std::cout << "Sample rates: ";
    for (auto rate : stream->GetSampleRates())
    {
        std::cout << rate << " ";
    }
    std::cout << std::endl;

    std::cout << "Gains: ";
    for (const auto& gain : stream->GetGainStages())
    {
        std::cout << gain.stage << " ";
        std::cout << gain.range.Min() << " ";
        std::cout << gain.range.Max() << " ";
    }
    std::cout << std::endl;

    ASSERT_TRUE(device);
}
