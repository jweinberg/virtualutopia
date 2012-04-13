//
//  vsu.cpp
//  virtualutopia
//
//  Created by Joshua Weinberg on 10/23/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "vsu.h"

namespace VSU
{

    VSU::VSU() : sampleTimer(41700), lastUpdate(0)
    {
    }
    
    //1 cycle == 50ns
    void VSU::Step(uint32_t cycles)
    {
        for (int i = 0; i < 6; ++i)
        {
            channels[i].update(cycles * 50);
        }
    }
    
} 