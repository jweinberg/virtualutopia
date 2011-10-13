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
#include "v810.h"
#import <QuartzCore/QuartzCore.h>

namespace VIP
{
    VIP::VIP()
    {
        DPSTTS.DPBSY = 1;
        DPSTTS.SCANRDY = 1;
        DPSTTS.FCLK = 1;
    }
    
    void VIP::WriteFrame()
    {
        
        CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
        
        uint32_t * bmpdata = (uint32_t*)calloc(384 * 256 * 4, sizeof(char));
        memset(bmpdata, 0xFF, 384 * 256 * 4);
        
        char * internalData = (char*)&leftFrameBuffer[0];
        
        for (int x = 0; x < 384; ++x)
        {
            for (int y = 0; y < 64; ++y)
            {
                for (int bt = 0; bt < 4; ++bt)
                {
                    uint32_t pixel;
                    char px = (*internalData >> (bt * 2)) & 0x3;
                    if (px == 1)
                        pixel = 0xFF520052;
                    else if (px == 2)
                        pixel = 0xFFAD00AD;
                    else if (px == 3)
                        pixel = 0xFFFF00FF;
                    else if (px == 0)
                        pixel = 0xFF000000;
                    
                    *(bmpdata + (384 * (y * 4 + bt)) + x) = pixel;
                }
                internalData++;
            }
        }
        
        CGContextRef context = CGBitmapContextCreate(bmpdata,
                                                     384, 
                                                     256, 8, 1536, colorSpace, kCGImageAlphaPremultipliedLast);
        
        
        CGImageRef image = CGBitmapContextCreateImage(context);

        static int f = 0;
        CFStringRef str = CFStringCreateWithFormat(NULL, NULL, CFSTR("/Users/jweinberg/frames/frame_%03d.png"), f++); 
        CFURLRef url = CFURLCreateWithFileSystemPath(NULL, str, kCFURLPOSIXPathStyle, false);
        
        CGImageDestinationRef destination = CGImageDestinationCreateWithURL(url, kUTTypePNG, 1, NULL);
        CGImageDestinationAddImage(destination, image, NULL);
        
        CGImageDestinationFinalize(destination);
        
        
        CFRelease(destination);
        CFRelease(url);
        CFRelease(str);
        
        CGColorSpaceRelease(colorSpace);
        CGContextRelease(context);
        free(bmpdata);
    }
    
    void VIP::DrawObj(const Obj &obj)
    {
        if (obj.JLON)
            leftFrameBuffer[0].DrawChr(chrRam[obj.JCA], obj.JX - obj.JP, obj.JY, 0, 0, 8, 8, obj.JHFLP, obj.JVFLP, JPLT[obj.JPLTS]);
        if (obj.JRON)
            rightFrameBuffer[0].DrawChr(chrRam[obj.JCA], obj.JX + obj.JP, obj.JY, 0, 0, 8, 8, obj.JHFLP, obj.JVFLP, JPLT[obj.JPLTS]);
            
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
                    DrawObj(oam[objIdx]);
                }
                if (objSearchIndex)
                    objSearchIndex--;   
            }
            else if (type == World::kNormalType)
            {
                const World &world = worlds[n];
             
                int xWorlds = (int)pow(2, world.SCX);
                //int yWorlds = pow(2, world.SCY);
                
                int x = 0;
                do
                {
                    int y = 0;
                    do
                    {
                        int yChar = ((y + world.MY)/8);
                        int xChar = ((x + world.MX)/8);
                        
                        
                        const BGMap &map = bgMaps[world.BGMAP_BASE + ((yChar / 64) * xWorlds) + (xChar / 64)];
                        
                        const BGMapData &data = map.chars[(yChar % 64) * 64 + (xChar % 64)];
                        const Chr& chr = chrRam[data.charNum];  
                        if (world.LON)
                            leftFrameBuffer[0].DrawChr(chr, x + world.GX - world.GP, y + world.GY, 0, 0, MIN(8, (world.W + 1) - x), MIN(8, (world.H + 1) - y), data.BHFLP, data.BVFLP, GPLT[data.GPLTS]);
                        if (world.RON)
                            rightFrameBuffer[0].DrawChr(chr, x + world.GX + world.GP, y + world.GY, 0, 0, MIN(8, (world.W + 1) - x), MIN(8, (world.H + 1) - y), data.BHFLP, data.BVFLP, GPLT[data.GPLTS]);
                        y += 8;
                    } while(y <= world.H);
                    x += 8;
                } while(x <= world.W);
                
            }
        }
       //WriteFrame();
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
                bool gameStart = false;
                if (framesWaited > FRMCYC) //We've triggered a GCLK
                {
                    framesWaited = 0;
                    gameStart = true;
                }
                if (INTENB.FRAMESTART)
                {
                    cpu->processInterrupt((CPU::InterruptCode)4);
                }
                INTPND.FRAMESTART = 1; //Trigger an FCLK
                INTPND.GAMESTART = gameStart;
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
             
                if (INTENB.XPEND)
                    cpu->processInterrupt((CPU::InterruptCode)4);
                INTPND.XPEND = true;
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
                
                if (INTENB.LFBEND)
                    cpu->processInterrupt((CPU::InterruptCode)4);
                INTPND.LFBEND = true;
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
                if (INTENB.RFBEND)
                    cpu->processInterrupt((CPU::InterruptCode)4);
                INTPND.RFBEND = true;
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
                
                if (INTENB.SBHIT)
                    cpu->processInterrupt((CPU::InterruptCode)4);
                INTPND.SBHIT = true;
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
                memset((char*)&rightFrameBuffer[0], 0, 0x6000);
                memset((char*)&leftFrameBuffer[0], 0, 0x6000);
                
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