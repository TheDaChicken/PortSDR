PortSDR: Interfacing with SDR devices
========================================
PortSDR is a C++ library for interfacing with SDR devices. It is designed to be simple and easy to use. The library is
designed to support many SDR vendors.

## Why not use SoapySDR instead?

Since SoapySDR is based on user installed plugins, this may bring complexity to the user. Users are required to compile a plugin for each vendor.

PortSDR is meant to be a lot easier to quickly bring support for a ton of vendors.

## Supported Devices

- RTL-SDR (with librtlsdr)
- AIRSPY Mini / R2 (with libairspy)
- AIRSPY HF+ Discovery (with libairspyhf)

## Building

### Dependencies

- CMake
- libusb
- librtlsdr 
- libairspy (builds from source if not found)

### Building

#### Ubuntu

```bash
sudo apt-get install cmake librtlsdr-dev
git clone 
cd portsdr
mkdir build
cd build
cmake ..
make
sudo make install
```

## API Usage

### Opening the first device

```cpp
#include <iostream>
#include <portsdr/portsdr.h>

int main()
{
    PortSDR::PortSDR sdr;

    // Get the first available SDR device
    const std::optional<PortSDR::Device> device = sdr.GetFirstAvailableSDR();
    if (!device)
    {
        std::cerr << "There are no devices" << std::endl;
        return 1;
    }
    
    std::cout << "Serial: " << device->serial << std::endl;
    
    // Open the stream from the device
    std::unique_ptr<PortSDR::Stream> stream;
    
    auto ret = device->CreateStream(stream);
    if (ret != PortSDR::ErrorCode::OK)
    {
        std::cerr << "Failed to open the device" << std::endl;
        return 1;
    }
    
    std::cout << "Name: " << stream->GetUSBStrings().name << std::endl;
    
    return 0;
}
```

### Listing the capabilities of the stream

The stream object allows you to get the capabilities of the device.

`GetGainModes` is used to get the available gain modes.

`GetGainStage` function returns a vector of all gain stages which can be used by `SetGain`.

`GetSampleRates` returns a vector of all available sample rates.

`GetSampleFormats` returns a vector of all available sample formats.

### Example usage of the capabilities
```cpp
int main()
{
    PortSDR::PortSDR sdr;
    
    // Get the first available SDR device
    const std::optional<PortSDR::Device> device = sdr.GetFirstAvailableSDR();
    if (!device)
    {
        std::cerr << "There are no devices" << std::endl;
        return 1;
    }
    
    // Open the device
    std::unique_ptr<PortSDR::Stream> stream;
    
    auto ret = device->CreateStream(stream);
    if (ret != PortSDR::ErrorCode::OK)
    {
        std::cerr << "Failed to open the device" << std::endl;
        return 1;
    }
    
    // Print the capabilities
    std::cout << "Sample rates: ";
    for (auto rate : stream->GetSampleRates())
    {
        std::cout << rate << " ";
    }
    std::cout << std::endl;
    
    std::cout << "Gains Stages: ";
    for (const auto& gain : stream->GetGainStage())
    {
        std::cout << gain.stage << " ";
        std::cout << gain.range.min() << " ";
        std::cout << gain.range.max() << " ";
    }
    std::cout << std::endl;
}
```

### Gain Control

- `SetGain(double gain, std::string_view stage)` which allows you to freely control gain given a gain stage.

Depending on the device, there are also different gain modes with different stages.

For AirSpy Mini/R2 devices gain modes:
- Linearity
- Sensitivity

`SetGainMode` is used to set the gain mode.

### Example usage of `PortSDR::Stream`

```cpp
#include <iostream>
#include <portsdr/portsdr.h>

int main()
{
    // ... etc Open the device
    
    // Create Stream
    std::unique_ptr<PortSDR::Stream> stream = device.CreateStream();
    
    // Set callback
    stream->SetCallback([](PortSDR::SDRTransfer& transfer)
    {
        // Do something with the samples
    });
    
    // Set the sample rate
    stream->SetSampleRate(2.4e6);
    
    // Set the center frequency
    stream->SetCenterFrequency(100e6);
    
    // Set the gain
    stream->SetGain(20);
    
    // Start the stream
    stream->Start(); 
    
    // End the stream
    stream->Stop();
    return 0;
}

```

## Goals 
- Do I want to create a class to automatically do quantization
      - Maybe use libvolk optionally for SIMD optimizations of automatic converters.
- Add more SDR vendors support.

## Remarks

This library was created to be used in my projects.

If the API design has flaws, please let me know. I am open to suggestions.