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

void NVC::NVC::ApplyReadWait(uint32_t address)
{
    int region = (address >> 24) & 0x3;
    switch (region)
    {
        case 0: //VIP
            cpu->cycles += 2;
            break;
        case 1: //Sound
            cpu->cycles += 1;
            break;
        case 2: //Registers
            cpu->cycles += 1;
            break;
        case 3: //Unused
            cpu->cycles += 1;
            break;
        case 4: //GP Expansion
            cpu->cycles += (WCR.EXP1W ? 1 : 2);
            break;
        case 5: //Work ram
            cpu->cycles += 1;
            break;
        case 6: //GP Ram
            cpu->cycles += 1;
            break;
        case 7: //GP Rom
            cpu->cycles += (WCR.ROM1W ? 1 : 2);
            break;
    }
}

void NVC::NVC::ApplyWriteWait(uint32_t address)
{
    int region = (address >> 24) & 0x3;
    switch (region)
    {
        case 0: //VIP
            cpu->cycles += 2;
            break;
        case 1: //Sound
            cpu->cycles += 1;
            break;
        case 2: //Registers
            cpu->cycles += 1;
            break;
        case 3: //Unused
            cpu->cycles += 1;
            break;
        case 4: //GP Expansion
            cpu->cycles += (WCR.EXP1W ? 1 : 2);
            break;
        case 5: //Work ram
            cpu->cycles += 1;
            break;
        case 6: //GP Ram
            cpu->cycles += 1;
            break;
        case 7: //GP Rom
            cpu->cycles += (WCR.ROM1W ? 1 : 2);
            break;
    }
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