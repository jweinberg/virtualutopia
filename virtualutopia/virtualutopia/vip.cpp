//
//  vip.cpp
//  virtualutopia
//
//  Created by Joshua Weinberg on 9/28/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "vip.h"
namespace VIP
{
    VIP::VIP()
    {
        DPSTTS.DPBSY = 1;
        DPSTTS.SCANRDY = 1;
        DPSTTS.FCLK = 1;
    }
    
    
    void v810_int(int a)
    {
        
    }
    
    char * VIP::operator[](int offset)
    {
        switch (offset)
        {
            case 0x5F800:
                return (char*)&INTPND;
            case 0x5F802:
                return (char*)&INTENB;
            case 0x5F804:
                return (char*)&INTCLR;
            case 0x5F820:
                return (char*)&DPSTTS;
            case 0x5F822:
                return (char*)&DPCTRL;
            case 0x5F824:
                return (char*)&BRTA;
            case 0x5F826:
                return (char*)&BRTB;
            case 0x5F828:
                return (char*)&BRTC;
            case 0x5F82A:
                return (char*)&REST;
            case 0x5F830:
                return (char*)&CTA;
            case 0x5F840:
                return (char*)&XPSTTS;
            case 0x5F842:
                return (char*)&XPCTRL;
            case 0x5F844:
                return (char*)&VER;
            case 0x5F848:
                return (char*)&SPT0;
            case 0x5F85A:
                return (char*)&SPT1;
            case 0x5F84C:
                return (char*)&SPT2;
            case 0x5F84E:
                return (char*)&SPT3;
            default:
                return &videoRam[offset];
        }
    }
    
    void VIP::Step(int cycles)
    {
        //There are 400,000 cycles in one frame (1 cycle = 50ns, 1 frame = 20ms)
        
        if (rowCount == 0) //Starting a new cycle
        {
            XPSTTS.SBCOUNT = 0;
            XPSTTS.SBOUT = 0;
            
            framesWaited++;
            if (framesWaited > FRMCYC) //We've triggered a GCLK
            {
                framesWaited = 0;
                INTPND.GAMESTART = 1;
            }
            INTPND.FRAMESTART = 1; //Trigger an FCLK
        }
        
        if (!XPSTTS.SBOUT) XPSTTS.SBOUT = 1;
        XPSTTS.SBCOUNT = rowCount;
     
        
        INTPND &= INTENB;
        if (INTPND)
        {
            INTPND = 0;
            v810_int(4);
        }
    }
}