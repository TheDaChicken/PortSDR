//
// Created by TheDaChicken on 12/17/2024.
//

#include <PortSDR.h>
#include <thread>
#include <gtest/gtest.h>

TEST(AirSpy, Devices)
{
    PortSDR::PortSDR portSDR;
    const std::shared_ptr<PortSDR::Host> sdrHost = portSDR.GetHost(PortSDR::Host::AIRSPY);

    ASSERT_TRUE(sdrHost);

    const auto devices = sdrHost->AvailableDevices();

    ASSERT_FALSE(devices.empty()) << "No devices found";

    std::cout << "Found ";
    std::cout << devices.size();
    std::cout << " devices" << std::endl;

    for (const auto& device : devices)
    {
        std::cout << device.index << std::endl;
    }
}

TEST(AirSpy, Stream)
{
    PortSDR::PortSDR portSDR;
    std::shared_ptr<PortSDR::Host> sdrHost = portSDR.GetHost(PortSDR::Host::AIRSPY);

    ASSERT_TRUE(sdrHost);

    const std::vector<PortSDR::Device>& devices = sdrHost->AvailableDevices();

    ASSERT_TRUE(!devices.empty());

    const auto& device = devices.front();
    auto stream = sdrHost->CreateStream();

    ASSERT_EQ(stream->Initialize(device), 0) << "Failed to initialize stream";

    // Test sample rates
    const auto sampleRates = stream->GetSampleRates();
    ASSERT_FALSE(sampleRates.empty()) << "No sample rates found";

    // Test callback
    ASSERT_EQ(stream->SetCallback([](const PortSDR::SDRTransfer& sdr)
                  {
                  std::cout << "Received " << sdr.frame_size << " samples" << std::endl;
                  }),
              0) << "Failed to set callback";

    ASSERT_EQ(stream->SetSampleRate(sampleRates.front()), 0) << "Failed to set sample rate";
    ASSERT_EQ(stream->SetSampleFormat(PortSDR::SAMPLE_FORMAT_IQ_FLOAT32), 0) << "Failed to set sample format";
    ASSERT_EQ(stream->Start(), 0) << "Failed to start stream";

    // Test Gain
    const auto gainRange = stream->GetGainStage();

    ASSERT_EQ(stream->SetGain(gainRange.range.Min()), 0) << "Failed to set gain";
    ASSERT_EQ(stream->SetGain(gainRange.range.Max()), 0) << "Failed to set gain";

    // Test Frequency. Tune to some rando frequency.
    ASSERT_EQ(stream->SetCenterFrequency(100.7e6), 0) << "Failed to set frequency";

    std::this_thread::sleep_for(std::chrono::seconds{1});

    ASSERT_EQ(stream->Stop(), 0) << "Failed to stop stream";
}
