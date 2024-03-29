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
    enum Button
    {
        PWR,
        SGN,
        A,
        B,
        RT,
        LT,
        RU,
        RR,
        LR,
        LL,
        LD,
        LU,
        STA,
        SELECT,
        RL,
        RD,
        INVALID
    };
    
    class NVC
    {
    public:
        NVC()
        {
        }
        void Reset();
        void Step(uint32_t cycles);
        void SetButton(Button button, bool val);
        void ApplyReadWait(uint32_t address);
        void ApplyWriteWait(uint32_t address);
        
        template <typename T>
        inline T *memoryLookup(uint32_t address) const
        {
            switch (address)
            {
                case 0x2000000:
                    return ((T*)&CCR);
                case 0x2000004:
                    return ((T*)&CCSR);
                case 0x2000008:
                    return ((T*)&CDTR);
                case 0x200000C:
                    return ((T*)&CDRR);
                case 0x2000010:
                    return ((T*)(((uint8_t*)&SDR_HW)));
                case 0x2000014:
                    return ((T*)(((uint8_t*)&SDR_HW) + 1));
                case 0x2000018:
                    return ((T*)&TLR);
                case 0x200001C:
                    return ((T*)&THR);
                case 0x2000020:
                    return ((T*)&TCR);
                case 0x2000024:
                    return ((T*)&WCR);
                case 0x2000028:
                    return ((T*)&SCR);
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
                    *((T*)&SDR.SDLR) = val;
                    break;
                case 0x2000014:
                    *((T*)&SDR.SDHR) = val;
                    break;
                case 0x2000018:
                    *((T*)&internalTLR) = val;
                    awaitingReload = true;
                    break;
                case 0x200001C:
                    *((T*)&internalTHR) = val;
                    awaitingReload = true;
                    break;
                case 0x2000020:
                {
                    _TCR *incomingTCR = (_TCR*)&val;
                    if (!(TCR.T_ENB && timerCount > 0) && incomingTCR->Z_STAT_CLR)
                        TCR.Z_STAT = 0;
                    
                    TCR.T_ENB = incomingTCR->T_ENB;
                    TCR.TIM_Z_INT = incomingTCR->TIM_Z_INT;
                    TCR.T_CLK_SEL = incomingTCR->T_CLK_SEL;
                    cachedTimerStep = (TCR.T_CLK_SEL ? 500 : 2000);
                    break;
                }
                case 0x2000024:
                    *((T*)&WCR) = val;
                    break;
                case 0x2000028:
//                    *((T*)&SCR) = val;
                    _SCR *incomingSCR = (_SCR*)&val;

                    SCR.DIS = incomingSCR->DIS;
                    SCR.SOFTCK = incomingSCR->SOFTCK;
                    SCR.PARA = incomingSCR->PARA;
                    SCR.INT = incomingSCR->INT;
                    
                    if (incomingSCR->HWSI && !SCR.DIS && readCounter <= 0)
                    {
                        SCR.STAT = true;
                        currentReadButton = (Button)0;
                        SDR_LATCHED = SDR_HW;
                        readCounter = 640;
                    }
                    
                    if (incomingSCR->DIS)
                    {
                        readCounter = 0;
                        currentReadButton = (Button)0;
                    }
                    break;
            }
        }
           
        
        CPU::v810 *cpu; //8 bytes
        int32_t readCounter; //4
        int32_t lastInputUpdate; //4
        uint32_t lastTimer; //4
        uint16_t cachedTimerStep; //2 

        union //2
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
                uint8_t internalTLR;
                uint8_t internalTHR;
            };
            uint16_t internalTimerCount;
        };

        REGISTER_BITFIELD(uint16_t, SDR,
            REGISTER_BITFIELD(uint8_t, SDLR,
                              uint8_t PWR:1;
                              uint8_t SGN:1;
                              uint8_t A:1;
                              uint8_t B:1;
                              uint8_t RT:1;
                              uint8_t LT:1;
                              uint8_t RU:1;
                              uint8_t RR:1;
                              );
            REGISTER_BITFIELD(uint8_t, SDHR,
                              uint8_t LR:1;
                              uint8_t LL:1;
                              uint8_t LD:1;
                              uint8_t LU:1;
                              uint8_t STA:1;
                              uint8_t SEL:1;
                              uint8_t RL:1;
                              uint8_t RD:1;
                              );
                          
                          );

        uint16_t SDR_HW;
        uint16_t SDR_LATCHED;

        Button currentReadButton; //4
        
        bool setKey; //1
        bool awaitingReload; //1
        
        
        REGISTER_BITFIELD(uint8_t, SCR, // 1
                          uint8_t DIS:1;
                          uint8_t STAT:1;
                          uint8_t HWSI:1;
                          uint8_t reserved_0:1;
                          uint8_t SOFTCK:1;
                          uint8_t PARA:1;
                          uint8_t reserved_1:1;
                          uint8_t INT:1;
                          );
        
        REGISTER_BITFIELD(uint8_t, WCR, //1
                          uint8_t ROM1W:1;
                          uint8_t EXP1W:1;
                          uint8_t padding:6;
                          );
        
        REGISTER_BITFIELD(uint8_t, TCR, //1
                          uint8_t T_ENB:1;
                          uint8_t Z_STAT:1;
                          uint8_t Z_STAT_CLR:1;
                          uint8_t TIM_Z_INT:1;
                          uint8_t T_CLK_SEL:1;
                          uint8_t padding:3;
                          );
        
        uint8_t CDRR;
        uint8_t CDTR;
        uint8_t CCSR;
        uint8_t CCR;
        
    };
    
    
}

#endif
