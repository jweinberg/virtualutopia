//
//  vip.cpp
//  virtualutopia
//
//  Created by Joshua Weinberg on 9/28/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "vip.h"
#include "world.h"

namespace VIP
{
    VIP::VIP()
    {
        DPSTTS.DPBSY = 1;
        DPSTTS.SCANRDY = 1;
        DPSTTS.FCLK = 1;
    }
    
    char& VIP::operator[](const int offset)
    {
        switch (offset)
        {
            case 0x00000 ... 0x05FFF:
                return *((char*)&leftFrameBuffer_0 + offset);
            case 0x06000 ... 0x07FFF:
                return *(((char*)&chrRam[0]) + (offset - 0x06000));
            case 0x08000 ... 0x0DFFF:
                return *((char*)&leftFrameBuffer_1 + (offset - 0x08000));
            case 0x0E000 ... 0x0FFFF:
                return *(((char*)&chrRam[512]) + (offset - 0x0E000));
            case 0x10000 ... 0x15FFF:
                return *((char*)&rightFrameBuffer_0 + (offset - 0x10000));
            case 0x16000 ... 0x17FFF:
                return *(((char*)&chrRam[1024]) + (offset - 0x16000));
            case 0x18000 ... 0x1DFFF:
                return *((char*)&rightFrameBuffer_1 + (offset - 0x18000));
            case 0x1E000 ... 0x1FFFF:
                return *(((char*)&chrRam[1536]) + (offset - 0x1E000));
            case 0x3D800 ... 0x3DC00:
                return *(((char*)&worlds[0]) + (offset - 0x3D800));
            case 0x3E000 ... 0x3FFFF:
                return *(((char*)&oam[0]) + (offset - 0x3E000));
            case 0x5F800:
                return *(char*)&INTPND;
            case 0x5F802:
                return *(char*)&INTENB;
            case 0x5F804:
                return *(char*)&INTCLR;
            case 0x5F820:
                return *(char*)&DPSTTS;
            case 0x5F822:
                return *(char*)&DPCTRL;
            case 0x5F824:
                return *(char*)&BRTA;
            case 0x5F826:
                return *(char*)&BRTB;
            case 0x5F828:
                return *(char*)&BRTC;
            case 0x5F82A:
                return *(char*)&REST;
            case 0x5F830:
                return *(char*)&CTA;
            case 0x5F840:
                return *(char*)&XPSTTS;
            case 0x5F842:
                return *(char*)&XPCTRL;
            case 0x5F844:
                return *(char*)&VER;
            case 0x5F848:
                return *(char*)&SPT0;
            case 0x5F85A:
                return *(char*)&SPT1;
            case 0x5F84C:
                return *(char*)&SPT2;
            case 0x5F84E:
                return *(char*)&SPT3;
            case 0x78000 ... 0x7FFFF:
                return *(((char*)&chrRam[0]) + (offset - 0x78000));
            default:
                return videoRam[offset];
        }
    }
    
    void VIP::DrawChr(const Chr &chr)
    {
        
    }
    
    void VIP::SetPixel(int x, int y, int val)
    {
        printf("Setting (%d, %d) to %d\n", x, y, val);
        
    }
    
    void VIP::DrawObj(const Obj &obj)
    {
        printf("Drawing char %d\n", obj.JCA);
        Chr chr = chrRam[obj.JCA];
        
        for (int x = 0; x < 8; ++x)
        {
            for (int y = 0; y < 8; ++y)
            {
                SetPixel(x, y, chr.data[y] & 0x3);
                chr.data[y] >>= 2;
            }
        }
    }
    
    void VIP::Draw()
    {
        objSearchIndex = 3;
        //Draw the worlds, starting with the lowest priority
        for (int n = 31; n >= 0; --n)
        {
            World::WorldType type = worlds[n].Type();
            
            //Stop drawing now
            if (type == World::kEndType)
                break;            
            else if (type == World::kDummyType)
                continue;
            else if (type == World::kObjType)
            {
                int stopIndex = (objSearchIndex > 0) ? objControl[objSearchIndex - 1] : 0;
                for (int objIdx = objControl[objSearchIndex]; objIdx >= stopIndex; --objIdx)
                {
                    printf("Drawing obj %d\n", objIdx);
                    DrawObj(oam[objIdx]);
                }
                if (objSearchIndex)
                    objSearchIndex--;
            }
        }
    }
    
    uint16_t VIP::Step(uint32_t cycles)
    {
        //There are 400,000 cycles in one frame (1 cycle = 50ns, 1 frame = 20ms)
        uint32_t timeSinceBuffer = cycles - lastFrameBuffer;
        
        if (rowCount < 28)
        {
            if (rowCount == -1 && timeSinceBuffer > 528)
            {
                rowCount = 0;
                XPSTTS.OVERTIME = 0;
                XPSTTS.SBCOUNT = 0;
                XPSTTS.SBOUT = 0;
                
                DPSTTS.SCANRDY = 1;
                DPSTTS.FCLK = 1;
                DPSTTS.DPBSY = 0;
                DPSTTS.DISP = DPCTRL.DISP;
                DPSTTS.RE = DPCTRL.RE;
                DPSTTS.SYNCE = DPCTRL.SYNCE;
                DPSTTS.LOCK = 0;
                
                framesWaited++;
                if (framesWaited > FRMCYC) //We've triggered a GCLK
                {
                    framesWaited = 0;
                    INTPND.GAMESTART = 1;
                }
                INTPND.FRAMESTART = 1; //Trigger an FCLK
                return 1;
            }
            
            if (!XPSTTS.SBOUT) XPSTTS.SBOUT = 1;
            
            if (timeSinceBuffer > 2560)
            {
                XPSTTS = 0;
                XPSTTS.SBCOUNT = rowCount;
                XPSTTS.XPEN = XPCTRL.XPEN;
                rowCount++;
                lastFrameBuffer = cycles;
            }
            
            if (rowCount == 18 && timeSinceBuffer > 1648)
            {
                DPSTTS.SCANRDY = 0;
                DPSTTS.DISP = DPCTRL.DISP;
                DPSTTS.RE = DPCTRL.RE;
                DPSTTS.SYNCE = DPCTRL.SYNCE;
                DPSTTS.FCLK = 1;
                DPSTTS.DPBSY = (frame & 1) ? 0x1 : 0x4;
            }
        }
        else
        {
            if (rowCount == 28 && timeSinceBuffer > 65536)
            {
                XPSTTS = 0;
                XPSTTS.SBCOUNT = 27;
                XPSTTS.XPEN = XPCTRL.XPEN;
                
                rowCount++;
            }
            else if (rowCount == 29 && timeSinceBuffer > 98304)
            {
                DPSTTS.SCANRDY = 1;
                DPSTTS.FCLK = 1;
                DPSTTS.DPBSY = 0;
                DPSTTS.DISP = DPCTRL.DISP;
                DPSTTS.RE = DPCTRL.RE;
                DPSTTS.SYNCE = DPCTRL.SYNCE;
                DPSTTS.LOCK = 0;
                rowCount++;
            }
            else if (rowCount == 30 && timeSinceBuffer > 131072)
            {
                DPSTTS.SCANRDY = 1;
                DPSTTS.FCLK = 0;
                DPSTTS.DPBSY = 0;
                DPSTTS.DISP = DPCTRL.DISP;
                DPSTTS.RE = DPCTRL.RE;
                DPSTTS.SYNCE = DPCTRL.SYNCE;
                DPSTTS.LOCK = 0;
                rowCount++;
            }
            else if (rowCount == 31 && timeSinceBuffer > 163840)
            {
                DPSTTS.SCANRDY = 0;
                DPSTTS.DISP = DPCTRL.DISP;
                DPSTTS.RE = DPCTRL.RE;
                DPSTTS.SYNCE = DPCTRL.SYNCE;
                DPSTTS.FCLK = 1;
                DPSTTS.DPBSY = (frame & 1) ? 0x8 : 0x2;
                rowCount++;
            }
            else if (rowCount == 32 && timeSinceBuffer > 229376)
            {
                DPSTTS.SCANRDY = 1;
                DPSTTS.FCLK = 1;
                DPSTTS.DPBSY = 0;
                DPSTTS.DISP = DPCTRL.DISP;
                DPSTTS.RE = DPCTRL.RE;
                DPSTTS.SYNCE = DPCTRL.SYNCE;
                DPSTTS.LOCK = 0;
                rowCount++;
            }
            else if (rowCount == 33 && timeSinceBuffer > 270336)
            {
                Draw();
                rowCount = -1;
                frame++;
                if (frame < 1 || frame > 2) frame = 1;
                XPSTTS.XPBSY = frame;
                XPSTTS.SBCOUNT = 27;
                XPSTTS.OVERTIME = 0;
                XPSTTS.XPEN = XPCTRL.XPEN;
                lastFrameBuffer = cycles;
            }
        }
        return 0;
    }
}