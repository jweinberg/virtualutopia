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
    SDR = 0;
    SCR = 0;
    TCR = 0;
    THR = 0xFF;
    TLR = 0xFF;
    internalTimerCount = 0;
    lastTimer = 0;
    readCounter = 0;
    lastInputUpdate = 0;
    SDR_HW = 0;
    SDR_LATCHED = 0;
    currentReadButton = (Button)0;
    setKey = false;
}

void NVC::NVC::ApplyReadWait(uint32_t address)
{
    int region = (address >> 24) & 0x3;
    switch (region)
    {
        case 0: //VIP
            cpu->cycles += 4;
            break;
        case 1: //Sound
            cpu->cycles += 2;
            break;
        case 2: //Registers
            cpu->cycles += 2;
            break;
        case 3: //Unused
            cpu->cycles += 2;
            break;
        case 4: //GP Expansion
            cpu->cycles += (WCR.EXP1W ? 2 : 4);
            break;
        case 5: //Work ram
            cpu->cycles += 2;
            break;
        case 6: //GP Ram
            cpu->cycles += 2;
            break;
        case 7: //GP Rom
            cpu->cycles += (WCR.ROM1W ? 2 : 4);
            break;
    }
}

void NVC::NVC::ApplyWriteWait(uint32_t address)
{
    int region = (address >> 24) & 0x3;
    switch (region)
    {
        case 0: //VIP
            cpu->cycles += 4;
            break;
        case 1: //Sound
            cpu->cycles += 2;
            break;
        case 2: //Registers
            cpu->cycles += 2;
            break;
        case 3: //Unused
            cpu->cycles += 2;
            break;
        case 4: //GP Expansion
            cpu->cycles += (WCR.EXP1W ? 2 : 4);
            break;
        case 5: //Work ram
            cpu->cycles += 2;
            break;
        case 6: //GP Ram
            cpu->cycles += 2;
            break;
        case 7: //GP Rom
            cpu->cycles += (WCR.ROM1W ? 2 : 4);
            break;
    }
}

void NVC::NVC::SetButton(Button button, bool val)
{
    if (val)
        SDR_HW |= (1 << button);
    else
        SDR_HW &= ~(1 << button);   
}

void NVC::NVC::Step(uint32_t cycles)
{
//    //Update input
    if (readCounter > 0)
    {
        readCounter -= (cycles - lastInputUpdate);
        
        while (readCounter <= 0)
        {
            uint16_t oldVal = SDR & (1 << currentReadButton);
            
            SDR &= ~(1 << currentReadButton);
            uint16_t val = SDR_LATCHED & (1 << currentReadButton);
            if (val && !oldVal && currentReadButton > B)
                setKey = true;
            SDR |= val;
            
            currentReadButton++;
            if (currentReadButton != INVALID)
                readCounter += 640;
            else
            {
                SCR.STAT = 0;
                if (SCR.INT && setKey)
                {
                    cpu->processInterrupt(CPU::INTKEY);
                }
                setKey = false;
                break;
            }
        }
        
    }
    lastInputUpdate = cycles;
    
    //Update timer
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
                    cpu->processInterrupt(CPU::INTTIM);
                //TCR.T_ENB = 0;
            }
        }
    }
}