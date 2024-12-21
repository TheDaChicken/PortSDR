PortSDR: Interfacing with SDR devices
========================================
PortSDR is a C++ library for interfacing with SDR devices. It is designed to be simple and easy to use. The library is
designed to support many SDR vendors.

## Why not use SoapySDR instead?

SoapySDR is a great library for interfacing with SDR devices.
However, it is not very easy to use:

- it's a plugin based system.
- It requires compiling/installing a plugin for each vendor you want to use.

PortSDR is meant to be a lot easier to use

## Supported Devices

- RTL-SDR (with librtlsdr)
- AIRSPY Mini / R2 (with libairspy)

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
    std::shared_ptr<PortSDR::Device> device = sdr.GetFirstAvailableSDR();
    if (device == nullptr)
    {
        std::cerr << "There are no devices" << std::endl;
        return 1;
    }
    
    std::cout << "Device: " << device->name << std::endl;
    std::cout << "Serial: " << device->serial << std::endl;
    
    // Open the stream from the device
    std::shared_ptr<PortSDR::Stream> stream;
    
    int ret = device->CreateStream(stream);
    if (ret != 0)
    {
        std::cerr << "Failed to open the device" << std::endl;
        return 1;
    }
    
    return 0;
}
```

### Listing the capabilities of the stream

The stream object allows you to get the capabilities of the device.

`GetGainModes` is used to get the available gain modes.

`GetGainRanges` function returns a vector of all gain stages which can be used by `SetGain`.

`GetSampleRates` returns a vector of all available sample rates.

`GetSampleFormats` returns a vector of all available sample formats.

### Example usage of the capabilities
```cpp
int main()
{
    PortSDR::PortSDR sdr;
    
    // Get the first available SDR device
    std::shared_ptr<PortSDR::Device> device = sdr.GetFirstAvailableSDR();
    
    // Open the device
    std::shared_ptr<PortSDR::Stream> stream;
    
    int ret = device->CreateStream(stream);
    if (ret != 0)
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
    
    std::cout << "Gains: ";
    for (auto gain : stream->GetGainRanges())
    {
        std::cout << gain.stage << " ";
        std::cout << gain.range.min() << " ";
        std::cout << gain.range.max() << " ";
    }
    std::cout << std::endl;
}
```

### Gain Control

There are two ways to control the gain:
- `SetGain(int)` which allows you to set the gain of all stages at once.
- `SetGain(int, std::string_view)` which allows you to freely control gain given a gain stage.

Depending on the device, there are also different gain modes for `SetGain(int)`

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
    // Open the device
    std::shared_ptr<PortSDR::Stream> stream = device->CreateStream();
    
    // Set callback
    stream->SetCallback([](const void *buffer, std::size_t elements)
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

## Converters

Some SDR libraries support different sample formats. Some don't.
This library contains automatic converters to convert the samples to any format you want.

### Automatic conversion

```cpp
// ... Get the device 

// Open the device
std::shared_ptr<PortSDR::Stream> stream = device->CreateStream();

// Set the sample format to 16-bit
stream->SetSampleFormat(PortSDR::SAMPLE_FORMAT_INT16);
```

Currently supported formats from SDR vendors:

- RTLSDR: 8-bit (uint8_t), 16-bit (with automatic conversion)
- AIRSPY: 16-bit (int16_t), 32-bit (float)

## Goals 
- Add more automatic type converters for different SDR devices.
- Maybe use libvolk optionally for SIMD optimizations of automatic converters.
- Add more SDR vendors support.

## Remarks

This library was created to be used in my projects.

If the API design has flaws, please let me know. I am open to suggestions.