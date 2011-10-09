//
//  nvc.h
//  virtualutopia
//
//  Created by Joshua Weinberg on 9/28/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef virtualutopia_nvc_h
#define virtualutopia_nvc_h

#include "registers.h"

namespace NVC 
{
    class NVC
    {
    public:
        NVC()
        {
        }
        
        char &operator[](uint32_t address) const;
        
        uint8_t SCR;
        uint8_t WCR;
        REGISTER_BITFIELD(TCR, 
                          uint8_t T_ENB:1;
                          uint8_t Z_STAT:1;
                          uint8_t Z_STAT_CLR:1;
                          uint8_t TIM_Z_INT:1;
                          uint8_t T_CLK_SEL:1;
                          uint8_t padding:3;
                        );
        uint8_t THR;
        uint8_t TLR;
        REGISTER_BITFIELD(SDLR,
                          uint8_t PWR:1;
                          uint8_t SGN:1;
                          uint8_t A:1;
                          uint8_t B:1;
                          uint8_t RT:1;
                          uint8_t LT:1;
                          uint8_t RU:1;
                          uint8_t RR:1;
                          );
        
        REGISTER_BITFIELD(SDHR,
                          uint8_t LR:1;
                          uint8_t LL:1;
                          uint8_t LD:1;
                          uint8_t LU:1;
                          uint8_t STA:1;
                          uint8_t SEL:1;
                          uint8_t RL:1;
                          uint8_t RD:1;
                          );
        uint8_t CDRR;
        uint8_t CDTR;
        uint8_t CCSR;
        uint8_t CCR;
    };
}

#endif
