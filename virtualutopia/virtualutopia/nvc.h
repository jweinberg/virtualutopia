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
#include <assert.h>

namespace CPU
{
    class v810;
}

namespace NVC 
{
    class NVC
    {
    public:
        NVC()
        {
        }
        void Reset();
        CPU::v810 *cpu;
        uint32_t lastTimer;
        void Step(uint32_t cycles);
        void ApplyReadWait(uint32_t address);
        void ApplyWriteWait(uint32_t address);
        template <typename T>
        inline const T& read(uint32_t address) const
        {
            switch (address)
            {
                case 0x2000000:
                    return *((T*)&CCR);
                case 0x2000004:
                    return *((T*)&CCSR);
                case 0x2000008:
                    return *((T*)&CDTR);
                case 0x200000C:
                    return *((T*)&CDRR);
                case 0x2000010:
                    return *((T*)&SDLR);
                case 0x2000014:
                    return *((T*)&SDHR);
                case 0x2000018:
                    return *((T*)&TLR);
                case 0x200001C:
                    return *((T*)&THR);
                case 0x2000020:
                    return *((T*)&TCR);
                case 0x2000024:
                    return *((T*)&WCR);
                case 0x2000028:
                    return *((T*)&SCR);
            }
            assert(false);
        }

        template <typename T>
        inline void store(T& val, uint32_t address)
        {
            //printf("Setting %X to %X\n", address, val);
            switch (address)
            {
                case 0x2000000:
                    *((T*)&CCR) = val;
                    break;
                case 0x2000004:
                    *((T*)&CCSR) = val;
                    break;
                case 0x2000008:
                    *((T*)&CDTR) = val;
                    break;
                case 0x200000C:
                    *((T*)&CDRR) = val;
                    break;
                case 0x2000010:
                    *((T*)&SDLR) = val;
                    break;
                case 0x2000014:
                    *((T*)&SDHR) = val;
                    break;
                case 0x2000018:
                    *((T*)&internalTLR) = val;
                    break;
                case 0x200001C:
                    *((T*)&internalTHR) = val;
                    break;
                case 0x2000020:
                {
                    _TCR *incomingTCR = (_TCR*)&val;
                    if (incomingTCR->T_ENB)
                        timerCount = internalTimerCount;
                    
                    TCR.T_ENB = incomingTCR->T_ENB;
                    TCR.TIM_Z_INT = incomingTCR->TIM_Z_INT;
                    TCR.T_CLK_SEL = incomingTCR->T_CLK_SEL;
                    if (!(TCR.T_ENB && timerCount > 0) && incomingTCR->Z_STAT_CLR)
                        TCR.Z_STAT = 0;
                    break;
                }
                case 0x2000024:
                    *((T*)&WCR) = val;
                case 0x2000028:
                    *((T*)&SCR) = val;
                    break;
                //default:
                 //   assert(false);
            }
        }
        
        inline char &operator[](uint32_t address) const
        {
            switch (address)
            {
                case 0x2000000:
                    return *((char*)&CCR);
                case 0x2000004:
                    return *((char*)&CCSR);
                case 0x2000008:
                    return *((char*)&CDTR);
                case 0x200000C:
                    return *((char*)&CDRR);
                case 0x2000010:
                    return *((char*)&SDLR);
                case 0x2000014:
                    return *((char*)&SDHR);
                case 0x2000018:
                    return *((char*)&TLR);
                case 0x200001C:
                    return *((char*)&THR);
                case 0x2000020:
                    return *((char*)&TCR);
                case 0x2000028:
                default:
                    return *((char*)&SCR);
            }
        }
        
        REGISTER_BITFIELD(SCR,
                          uint8_t DIS:1;
                          uint8_t STAT:1;
                          uint8_t HWSI:1;
                          uint8_t reserved_0:1;
                          uint8_t SOFTCK:1;
                          uint8_t PARA:1;
                          uint8_t reserved_1:1;
                          uint8_t INT:1;
                          );
                          
        REGISTER_BITFIELD(WCR,
                          uint8_t ROM1W:1;
                          uint8_t EXP1W:1;
                          uint8_t padding:6;
                          );
        
        REGISTER_BITFIELD(TCR, 
                          uint8_t T_ENB:1;
                          uint8_t Z_STAT:1;
                          uint8_t Z_STAT_CLR:1;
                          uint8_t TIM_Z_INT:1;
                          uint8_t T_CLK_SEL:1;
                          uint8_t padding:3;
                        );
        union
        {
            struct
            {
                uint8_t TLR;
                uint8_t THR;
            };
            uint16_t timerCount;
        };

        union
        {
            struct
            {
                uint8_t internalTHR;
                uint8_t internalTLR;
            };
            uint16_t internalTimerCount;
        };
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
        uint8_t CDRR;
        uint8_t CDTR;
        uint8_t CCSR;
        uint8_t CCR;
        
    };
    
    
}

#endif
