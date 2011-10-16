//
//  nvc.cpp
//  virtualutopia
//
//  Created by Joshua Weinberg on 9/28/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "nvc.h"
#include "v810.h"

void NVC::NVC::Reset()
{
    SDHR = 0;
    SDLR = 0;
    SCR = 0;
    TCR = 0;
    THR = 0xFF;
    TLR = 0xFF;
}

void NVC::NVC::Step(uint32_t cycles)
{
    if (TCR.T_ENB)
    {
        if ((cycles-lastTimer) > (TCR.T_CLK_SEL ? 400 : 2000))
        {
            if (timerCount)
                timerCount--;
            lastTimer=cycles;
            if (timerCount == 0) 
            {
                timerCount = internalTimerCount; //reset counter
                
                TCR.Z_STAT = 1;
                if (TCR.TIM_Z_INT)
                    cpu->processInterrupt((CPU::InterruptCode)1);
            }
        }
    }
}