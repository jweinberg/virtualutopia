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
#import <QuartzCore/QuartzCore.h>

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
            case 0x20000 ... 0x3BFFF:
                return *(((char*)&bgMaps[0]) + (offset - 0x20000));
            case 0x3D800 ... 0x3DBFF:
                return *(((char*)&worlds[0]) + (offset - 0x3D800));
            case 0x3DC00 ... 0x3DFFF:
                return *(((char*)&columnTable[0]) + (offset - 0x3DC00));
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
            case 0x5F860 ... 0x5F867:
                return *(((char*)&GPLT[0]) + (offset - 0x5F860));
            case 0x5F868 ... 0x5F86F:
                return *(((char*)&JPLT[0]) + (offset - 0x5F868));
            case 0x78000 ... 0x7FFFF:
                return *(((char*)&chrRam[0]) + (offset - 0x78000));
            default:
                return videoRam[offset];
        }
    }
    
    void VIP::DrawChr(const Chr &chr, int xoff, int yoff, int sourceXOffset, int sourceYOffset, int w, int h, const Palette &palette)
    {
        Chr chrCpy = chr;
        for (int x = sourceXOffset; x < w; ++x)
        {
            for (int y = sourceYOffset; y < h; ++y)
            {
                SetPixel(x + xoff, y + yoff, palette[chrCpy.data[y] & 0x3]);
                chrCpy.data[y] >>= 2;
            }
        }   
    }
    
    void VIP::SetPixel(int x, int y, int val)
    {
        //screen width = 384
        //screen height = 224
        
        //Data is stored by column, not by row
        
        //We need to figure out what byte of data to look at, each 'pixel'
        // is 2 bits, each byte store 4 pixels
        
        //One column is 64 bytes of data
        char * column = leftFrameBuffer_0 + (x * 64);
        column += (y / 4);
        
        int shift = (y % 4) * 2;
        *column |= (val << shift);
    }
    
    void VIP::WriteFrame()
    {
        
        CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
        
        uint32_t * bmpdata = (uint32_t*)calloc(384 * 256 * 4, sizeof(char));
        memset(bmpdata, 0xFF, 384 * 256 * 4);
        
        char * internalData = leftFrameBuffer_0;
        
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
        DrawChr(chrRam[obj.JCA], obj.JX, obj.JY, 0, 0, 8, 8, JPLT[obj.JPLTS]);
    }
    
    void VIP::Draw()
    {
//       
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
                const BGMap &map = bgMaps[world.BGMAP_BASE];
                
                int x = 0;
                
                do
                {
                    int y = 0;
                    do
                    {
                        const BGMapData &data = map.chars[(y/8) * 64 + (x/8)];
                        const Chr& chr = chrRam[data.charNum];
                        DrawChr(chr, x + world.GX, y + world.GY, 0, 0, MIN(8, world.W - x), MIN(8, world.H - y), GPLT[data.GPLTS]);
                        y += 8;
                    } while(y < world.H);
                    x += 8;
                } while(x<world.W);
                
            }
        }
       WriteFrame();
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
                memset(leftFrameBuffer_0, 0, 0x6000);
            }
        }
        return 0;
    }
}