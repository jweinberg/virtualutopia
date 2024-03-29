//
//  channel.h
//  virtualutopia
//
//  Created by Joshua Weinberg on 2/11/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef virtualutopia_channel_h
#define virtualutopia_channel_h
#include "registers.h"

template <typename T>
T clamp(T min, T max, T val)
{
    return (((val <= min) ? min : val) >= max) ? max : val;
}

namespace VSU
{
    
class Channel
{
public:
    Channel();
    double frequency();
    void update(long nanoseconds);
    
    uint64_t currentTimestep;
    uint64_t envelopeTimer;
    REGISTER_BITFIELD(uint8_t, SxINT, 
                      uint8_t counter : 5;
                      uint8_t data : 1;
                      uint8_t reserved : 1;
                      uint8_t enabled : 1;
                      );
    REGISTER_BITFIELD(uint8_t, SxLRV, 
                      uint8_t right : 4;
                      uint8_t left : 4;
                      );
    uint8_t SxFRQL;
    REGISTER_BITFIELD(uint8_t, SxFRQH,
                      uint8_t upper3Bits : 3;
                      uint8_t reserved : 5;
                      );
    REGISTER_BITFIELD(uint8_t, SxEV0,
                      uint8_t step : 3;
                      uint8_t u_d : 1;
                      uint8_t initialValue : 4;
                      );
    REGISTER_BITFIELD(uint8_t, SxEV1,
                      uint8_t enabled : 1;
                      uint8_t r_s : 1;
                      uint8_t reserved_0 : 2;
                      uint8_t sweep : 1;
                      uint8_t modulation : 1;
                      uint8_t enable_sweep : 1;
                      uint8_t reserved : 1;
                      );
    REGISTER_BITFIELD(uint8_t, SxRAM,
                      uint8_t waveformAddress : 4;
                      uint8_t reserved : 4;
                      );
    int8_t envelopeValue;
};
}

#endif
