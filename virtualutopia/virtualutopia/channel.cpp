//
//  channel.cpp
//  virtualutopia
//
//  Created by Joshua Weinberg on 2/11/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "channel.h"

static uint32_t envelopeStepTimes[8] = {154, 307, 461, 614, 768, 922, 1075, 1229};

namespace VSU
{    
    Channel::Channel() : currentTimestep(0),
    envelopeTimer(0),
    envelopeValue(0)
    {
    }
    
    double Channel::frequency()
    {
        float hz = 5000000.0 / ((2048 - ((SxFRQH.upper3Bits << 8) | SxFRQL)) * 32);
        return hz;
    }
        
    void Channel::update(long nanoseconds)
    {
        //Update envelope
        if (SxEV1.enabled)
        {
            envelopeTimer += nanoseconds;
            while (envelopeTimer >= envelopeStepTimes[SxEV0.step])
            {
                envelopeTimer -= nanoseconds;
                envelopeValue += (SxEV0.u_d ? 1 : -1);
                envelopeValue = clamp<int8_t>(0, 0xF, envelopeValue);
            }
        }
        
        

    }

};