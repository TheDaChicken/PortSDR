//
// Created by TheDaChicken on 12/19/2024.
//

#include "PortSDR.h"

PortSDR::IQConverterUINT8ToINT16::IQConverterUINT8ToINT16()
{
    for (unsigned int i = 0; i < 0x100; i++)
        m_lut.push_back(static_cast<int16_t>((static_cast<int16_t>(i) - 127) * 64));
}

void PortSDR::IQConverterUINT8ToINT16::Process(const uint8_t* data, int16_t* output,
                                               const std::size_t elementSize) const
{
    for (int i = 0; i < elementSize; i++)
    {
        output[i] = m_lut[data[i]];
    }
}

PortSDR::IQConverterINT16ToFLOAT::IQConverterINT16ToFLOAT()
{
    for (int16_t i = -32767; i < 32767; ++i)
    {
        m_lut.push_back(static_cast<float>(i) * (1.0f / 32768.0f));
    }
}

void PortSDR::IQConverterINT16ToFLOAT::Process(const uint8_t* data, float* output,
                                               const std::size_t elementSize) const
{
    for (int i = 0; i < elementSize; i++)
    {
        output[i] = m_lut[data[i]];
    }
}
