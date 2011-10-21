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
#include "affinetable.h"
#include "hbiastable.h"
#import <QuartzCore/QuartzCore.h>

namespace VIP
{
    VIP::VIP()
    {
        DPSTTS.DPBSY = 1;
        DPSTTS.SCANRDY = 1;
        DPSTTS.FCLK = 1;
    }
    
    void VIP::DumpCHR()
    {
        //Draw as 64x32
        
        CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
        
        uint32_t * bmpdata = (uint32_t*)calloc(64 * 8 * 32 * 8 * 4, sizeof(char));
        memset(bmpdata, 0xFF, 64 * 8 * 32 * 8 * 4);
        
        uint16_t * internalData = (uint16_t*)chrRam[0].data;
        
        
        
        for (int y = 0; y < 16384; ++y)
        {
            for (int x = 0; x < 8; ++x)
            {
                uint32_t pixel;
                char px = (*internalData >> (x * 2)) & 0x3;
                if (px == 1)
                    pixel = 0xFF520052;
                else if (px == 2)
                    pixel = 0xFFAD00AD;
                else if (px == 3)
                    pixel = 0xFFFF00FF;
                else if (px == 0)
                    pixel = 0xFF000000;
                
                *(bmpdata + (8 * y + x)) = pixel;
                
          }
          internalData++;
        }
        
        CGContextRef context = CGBitmapContextCreate(bmpdata,
                                                     8, 
                                                     16384, 8, 32, colorSpace, kCGImageAlphaPremultipliedLast);
        
        
        CGImageRef image = CGBitmapContextCreateImage(context);
        
        static int f = 0;
        CFStringRef str = CFStringCreateWithFormat(NULL, NULL, CFSTR("/Users/jweinberg/chr.png"), f++); 
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
    
    void VIP::DrawObj(const Obj &obj, int row)
    {
        int h = 8;
        int w = 8;
        
        int topInset = (row * 8) - (obj.JY + h);
        int bottomInset = ((row * 8) + 8) - obj.JY;
        
        if (topInset >= 0)
            return;
            
        if (bottomInset < 0)
            return;

        if (obj.JLON)
        {
            int x = obj.JX - obj.JP;
            if (x + 8 < 0 || x >= 384)
                return;

            leftFrameBuffer[0].DrawChr(chrRam[obj.JCA], row, obj.JX - obj.JP, obj.JY, 0, 0, 8, 8, obj.JHFLP, obj.JVFLP, JPLT[obj.JPLTS]);
        }
        if (obj.JRON)
        {
            int x = obj.JX + obj.JP;
            if (x + 8 < 0 || x >= 384)
                return;
            rightFrameBuffer[0].DrawChr(chrRam[obj.JCA], row, obj.JX + obj.JP, obj.JY, 0, 0, 8, 8, obj.JHFLP, obj.JVFLP, JPLT[obj.JPLTS]);
        }
            
    }
    
    void VIP::DrawNormalWorld(int row, const World &world)
    {
        int xWorlds = 1 << world.SCX;
        int yWorlds = 1 << world.SCY;;
        
        int y = max<int>((row * 8) - world.GY, 0);
        do
        {   
            int srcY = (y + world.MY) % (yWorlds * 512);
            int yWorld = srcY / 512;
            int yChar = (srcY & 0x1FF) / 8;                            
            int yOff = srcY & 7;
            
            int h = 8 - yOff;
            h = MIN(h, world.H + 1 - y);
            
            if (world.LON)
            {
                int x = 0;
                do
                {
                    int srcX = (x + world.MX - world.MP) % (xWorlds * 512);
                    int xWorld = srcX / 512;
                    int xChar = (srcX & 0x1FF) / 8;
                    int xOff = srcX & 7;
                    
                    int w = 8 - xOff;
                    w = MIN(w, world.W + 1 - x);
                    
                    const BGMap &map = bgMaps[world.BGMAP_BASE + (yWorld * xWorlds) + xWorld];
                    const BGMapData &data = map.chars[yChar * 64 + xChar];
                    const Chr& chr = chrRam[data.charNum];  
                    
                    int xPos = x + world.GX - world.GP;
                    if (!(xPos + w < 0 || xPos >= 384))
                        leftFrameBuffer[0].DrawChr(chr, row, xPos, y + world.GY, xOff, yOff, w, h, data.BHFLP, data.BVFLP, GPLT[data.GPLTS]);
                    x += (8 - xOff);
                } while(x <= world.W);
            }
            
            if (world.RON)
            {
                int x = 0;
                do
                {
                    int srcX = (x + world.MX + world.MP) % (xWorlds * 512);
                    int xWorld = srcX / 512;
                    int xChar = (srcX & 0x1FF) / 8;
                    int xOff = srcX & 7;
                    
                    int w = 8 - xOff;
                    w = MIN(w, world.W + 1 - x);
                    
                    const BGMap &map = bgMaps[world.BGMAP_BASE + (yWorld * xWorlds) + xWorld];
                    const BGMapData &data = map.chars[yChar * 64 + xChar];
                    const Chr& chr = chrRam[data.charNum];  
                    
                    int xPos = x + world.GX + world.GP;
                    if (!(xPos + w < 0 || xPos >= 384))
                        rightFrameBuffer[0].DrawChr(chr, row, xPos, y + world.GY, xOff, yOff, w, h, data.BHFLP, data.BVFLP, GPLT[data.GPLTS]);
                    x += (8 - xOff);
                } while(x <= world.W);
            }
            y += (8 - yOff);
        } while(y <= (max<int>((row * 8) - world.GY, 0) + 8) && y <= world.H);    
    }
    
    void VIP::DrawAffineWorld(int row, const World& world)
    {
        uint8_t xWorlds = 1 << world.SCX;
        uint8_t yWorlds = 1 << world.SCY;;
        
        
        int y = max<int>((row * 8) - world.GY, 0);
        
        for (; y < (max<int>((row * 8) - world.GY, 0) + 8) && y <= world.H; ++y)
        {
            const AffineTable affineTable = read<AffineTable>((world.PARAM_BASE & 0xFFF0) * 2 + 0x00020000 + y * sizeof(AffineTable));
            
            int rightParalax = (affineTable.MP < 0 ? affineTable.MP : 0);
            int leftParalax = (affineTable.MP >= 0 ? affineTable.MP : 0);
            
            int srcY = Fixed16x16(affineTable.MY) + Fixed16x16(affineTable.DY) * (((int64_t)y) << 16);
            if (srcY < 0 || srcY > yWorlds * 512)
                srcY %= (yWorlds * 512);
            
            uint8_t yWorld = srcY / 512;
            uint16_t yChar = (srcY & 0x1FF) / 8;                            
            uint8_t yOff = srcY & 7;
            
            for (int x = 0; x < world.W; ++x)
            {
                if (world.LON)
                {
                    int xPos = x + world.GX + world.GP;
                    if (!(xPos < 0 || xPos >= 384))
                    {
                        int srcX = (Fixed16x16(affineTable.MX) + Fixed16x16(affineTable.DX) * (((int64_t)(x + leftParalax)) << 16));
                        
                        //if (!world.OVER)
                        if (srcX < 0 || srcX > xWorlds * 512)
                        {
                            srcX %= (xWorlds * 512);
                        }
                        uint8_t xWorld = srcX / 512;
                        uint16_t xChar = (srcX & 0x1FF) / 8;
                        uint8_t xOff = srcX & 7;
                        
                        const BGMap &map = bgMaps[world.BGMAP_BASE + (yWorld * xWorlds) + xWorld];
                        const BGMapData &data = map.chars[yChar * 64 + xChar];
                        const Chr& chr = chrRam[data.charNum];  
                        
                        //We know we're rather safe here, don't need all the checks DrawChr does
                        uint16_t row = chr.data[data.BVFLP ? (7 - yOff) : yOff];
                        uint8_t shift = ((data.BHFLP ? 7 - xOff : xOff) * 2);
                        uint8_t idx = (row >> shift) & 0x3;
                        if (idx)
                        {
                            leftFrameBuffer[0].SetPixel(xPos, y + world.GY, GPLT[data.GPLTS][idx]);
                        }
                    }
                }
                if (world.RON)
                {
                    int xPos = x + world.GX - world.GP;
                    if (!(xPos < 0 || xPos >= 384))
                    {                                
                        int srcX = (Fixed16x16(affineTable.MX) + Fixed16x16(affineTable.DX) * (((int64_t)((x + rightParalax))) << 16));
                        
                        //if (!world.OVER)
                        if (srcX < 0 || srcX > xWorlds * 512)
                        {
                            srcX %= (xWorlds * 512);
                        }
                        uint8_t xWorld = srcX / 512;
                        uint16_t xChar = (srcX & 0x1FF) / 8;
                        uint8_t xOff = srcX & 7;
                        
                        const BGMap &map = bgMaps[world.BGMAP_BASE + (yWorld * xWorlds) + xWorld];
                        const BGMapData &data = map.chars[yChar * 64 + xChar];
                        const Chr& chr = chrRam[data.charNum];  
                        
                        //We know we're rather safe here, don't need all the checks DrawChr does
                        uint16_t row = chr.data[data.BVFLP ? (7 - yOff) : yOff];
                        uint8_t shift = ((data.BHFLP ? 7 - xOff : xOff) * 2);
                        uint8_t idx = (row >> shift) & 0x3;
                        if (idx)
                        {
                            rightFrameBuffer[0].SetPixel(xPos, y + world.GY, GPLT[data.GPLTS][idx]);
                        }
                    }
                }   
            }
        }
    }
    
    void VIP::DrawHBiasWorld(int row, const World &world)
    {
        uint8_t xWorlds = 1 << world.SCX;
        uint8_t yWorlds = 1 << world.SCY;;
        
        int y = max<int>((row * 8) - world.GY, 0);
        
        for (; y < (max<int>((row * 8) - world.GY, 0) + 8) && y <= world.H; ++y)
        {
            const HBiasTable hBiasTable = read<HBiasTable>((world.PARAM_BASE & 0xFFF0) * 2 + 0x00020000 + y * sizeof(HBiasTable));
            
            int srcY = (y + world.MY);
            if (srcY < 0 || srcY > yWorlds * 512)
                srcY %= (yWorlds * 512);
            
            uint8_t yWorld = srcY / 512;
            uint16_t yChar = (srcY & 0x1FF) / 8;                            
            uint8_t yOff = srcY & 7;
            
            for (int x = 0; x < world.W; ++x)
            {
                if (world.LON)
                {
                    int xPos = x + world.GX + world.GP;
                    if (!(xPos < 0 || xPos >= 384))
                    {
                        int srcX = x + world.MX - world.MP + hBiasTable.HOFSTL;
                        
                        //if (!world.OVER)
                        if (srcX < 0 || srcX > xWorlds * 512)
                        {
                            srcX %= (xWorlds * 512);
                        }
                        uint8_t xWorld = srcX / 512;
                        uint16_t xChar = (srcX & 0x1FF) / 8;
                        uint8_t xOff = srcX & 7;
                        
                        const BGMap &map = bgMaps[world.BGMAP_BASE + (yWorld * xWorlds) + xWorld];
                        const BGMapData &data = map.chars[yChar * 64 + xChar];
                        const Chr& chr = chrRam[data.charNum];  
                        
                        //We know we're rather safe here, don't need all the checks DrawChr does
                        uint16_t row = chr.data[data.BVFLP ? (7 - yOff) : yOff];
                        uint8_t shift = ((data.BHFLP ? 7 - xOff : xOff) * 2);
                        uint8_t idx = (row >> shift) & 0x3;
                        if (idx)
                        {
                            leftFrameBuffer[0].SetPixel(xPos, y + world.GY, GPLT[data.GPLTS][idx]);
                        }
                    }
                }
                if (world.RON)
                {
                    int xPos = x + world.GX - world.GP;
                    if (!(xPos < 0 || xPos >= 384))
                    {                                
                        int srcX = x + world.MX + world.MP + hBiasTable.HOFSTR;
                        
                        //if (!world.OVER)
                        if (srcX < 0 || srcX > xWorlds * 512)
                        {
                            srcX %= (xWorlds * 512);
                        }
                        uint8_t xWorld = srcX / 512;
                        uint16_t xChar = (srcX & 0x1FF) / 8;
                        uint8_t xOff = srcX & 7;
                        
                        const BGMap &map = bgMaps[world.BGMAP_BASE + (yWorld * xWorlds) + xWorld];
                        const BGMapData &data = map.chars[yChar * 64 + xChar];
                        const Chr& chr = chrRam[data.charNum];  
                        
                        //We know we're rather safe here, don't need all the checks DrawChr does
                        uint16_t row = chr.data[data.BVFLP ? (7 - yOff) : yOff];
                        uint8_t shift = ((data.BHFLP ? 7 - xOff : xOff) * 2);
                        uint8_t idx = (row >> shift) & 0x3;
                        if (idx)
                        {
                            rightFrameBuffer[0].SetPixel(xPos, y + world.GY, GPLT[data.GPLTS][idx]);
                        }
                    }
                }   
            }
        }   
    }
    
    void VIP::Draw(int row)
    {
        volatile static bool test = false;
        volatile static int testVal = 31;
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
                int stopIndex = (objSearchIndex > 0) ? objControl[objSearchIndex - 1].SPT : 0;
                if (stopIndex >= objControl[objSearchIndex].SPT)
                    stopIndex = 0;
                for (int objIdx = objControl[objSearchIndex].SPT; objIdx >= stopIndex; --objIdx)
                {
                     if (test ? testVal == n : true)
                         DrawObj(oam[objIdx], row);
                }
                if (objSearchIndex)
                    objSearchIndex--;   
            }
            else if (test ? testVal == n : true)
            {
                const World &world = worlds[n];
                if (world.GY + world.H <= row * 8)
                    continue;
                if (world.GY > (row * 8 + 8))
                    continue;
                
                if (type == World::kNormalType)
                    DrawNormalWorld(row, world);
                else if (type == World::kAffineType)
                    DrawAffineWorld(row, world);                                            
                else if (type == World::kHBiasType)
                    DrawHBiasWorld(row, world);
            }
        }
    }
    
    uint16_t VIP::Step(int32_t cycles)
    {
        //There are 400,000 cycles in one frame (1 cycle = 50ns, 1 frame = 20ms)
        uint32_t timeSinceBuffer = cycles - lastFrameBuffer;
        
        
        if (rowCount < 28)
        {
            if (rowCount == -1 && timeSinceBuffer > 528)
            {
                
                memset((char*)&rightFrameBuffer[0], 0, 0x6000);
                memset((char*)&leftFrameBuffer[0], 0, 0x6000);
                
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
                    framesWaited = 0;

                INTPND.FRAMESTART |= true; //Trigger an FCLK
                INTPND.GAMESTART |= (framesWaited == 0);

                if (INTENB.FRAMESTART || INTENB.GAMESTART)
                {
                    cpu->processInterrupt((CPU::InterruptCode)4);
                }

                return 1;
            }
            
            if (timeSinceBuffer > sbOutResetTime)
                XPSTTS.SBOUT = 0;
            
            if (timeSinceBuffer > 2560)
            {
                XPSTTS = 0;
                XPSTTS.SBCOUNT = rowCount;
                XPSTTS.XPEN = XPCTRL.XPEN;
                XPSTTS.SBOUT = 1;
                
                sbOutResetTime = timeSinceBuffer + 1120; //56 microseconds
                
                if (XPSTTS.SBCOUNT == XPCTRL.SBCMP)
                {
                    INTPND.SBHIT |= true;
                    if (INTENB.SBHIT)
                        cpu->processInterrupt((CPU::InterruptCode)4);
                }
                
                if (DPSTTS.DISP)
                {
                    Draw(rowCount);
                }
                
                char * internalDataLeft = (char*)&leftFrameBuffer[0];
                char * internalDataRight = (char*)&rightFrameBuffer[0];    
                
                for (int x = 0; x < 384; ++x)
                {
                    for (int y = rowCount * 2; y < rowCount * 2 + 2; ++y)
                    {
                        for (int bt = 0; bt < 4; ++bt)
                        {
                            uint32_t pixel = 0xFF000000;
                            
                            char leftPx = (*(internalDataLeft + (x * 64) + y) >> (bt * 2)) & 0x3;
                            char rightPx = (*(internalDataRight + (x * 64) + y) >> (bt * 2)) & 0x3;
                            
                            if (leftPx == 1)
                                pixel |= BRT[0].val;
                            else if (leftPx == 2)
                                pixel |= BRT[1].val;
                            else if (leftPx == 3)
                                pixel |= (BRT[2].val + BRT[1].val + BRT[0].val);
                            
                            if (rightPx == 1)
                                pixel |= BRT[0].val << 16;
                            else if (rightPx == 2)
                                pixel |= BRT[1].val << 16;
                            else if (rightPx == 3)
                                pixel |= (BRT[2].val + BRT[1].val + BRT[0].val) << 16;
                            
                            *(bmpData + (384 * (y * 4 + bt)) + x) = pixel;
                        }
                    }
                }
                
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
             
                INTPND.XPEND |= true;
                if (INTENB.XPEND) cpu->processInterrupt((CPU::InterruptCode)4);
                
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
                
                INTPND.LFBEND |= true;                
                if (INTENB.LFBEND) cpu->processInterrupt((CPU::InterruptCode)4);

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
                
                INTPND.RFBEND |= true;
                if (INTENB.RFBEND) cpu->processInterrupt((CPU::InterruptCode)4);

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