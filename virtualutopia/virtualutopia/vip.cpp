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

namespace VIP
{
    VIP::VIP()
    {
        FRMCYC = 0;
        INTPND = 0;
        INTENB = 0;
        DPSTTS = 0;
        XPSTTS = 0;
        XPCTRL = 0;
        BRT[0] = 0;
        BRT[1] = 0;
        BRT[2] = 0;
        lastFrameBuffer = 0;
        drawingCounter = 0;
        displayRegion = 0;
        sbOutResetTime = -1;
        column = 0;
        columnCounter = 259;
        displayFB = 0;
        drawingFB = 1;
        frame = 0;
        
        memset((char*)&rightFrameBuffer[0], 0, 0x6000);
        memset((char*)&leftFrameBuffer[0], 0, 0x6000);
        memset((char*)&rightFrameBuffer[1], 0, 0x6000);
        memset((char*)&leftFrameBuffer[1], 0, 0x6000);
        
        memset((char*)&worlds, 0, sizeof(worlds));
    }
    
//    void VIP::DumpCHR()
//    {
//        //Draw as 64x32
//        
//        CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
//        
//        uint32_t * bmpdata = (uint32_t*)calloc(64 * 8 * 32 * 8 * 4, sizeof(char));
//        memset(bmpdata, 0xFF, 64 * 8 * 32 * 8 * 4);
//        
//        uint16_t * internalData = (uint16_t*)chrRam[0].data;
//        
//        
//        
//        for (int y = 0; y < 16384; ++y)
//        {
//            for (int x = 0; x < 8; ++x)
//            {
//                uint32_t pixel = 0;
//                char px = (*internalData >> (x * 2)) & 0x3;
//                if (px == 1)
//                    pixel = 0xFF520052;
//                else if (px == 2)
//                    pixel = 0xFFAD00AD;
//                else if (px == 3)
//                    pixel = 0xFFFF00FF;
//                else if (px == 0)
//                    pixel = 0xFF000000;
//                
//                *(bmpdata + (8 * y + x)) = pixel;
//                
//            }
//            internalData++;
//        }
//        
//        CGContextRef context = CGBitmapContextCreate(bmpdata,
//                                                     8, 
//                                                     16384, 8, 32, colorSpace, kCGImageAlphaPremultipliedLast);
//        
//        
//        CGImageRef image = CGBitmapContextCreateImage(context);
//        
//        static int f = 0;
//        CFStringRef str = CFStringCreateWithFormat(NULL, NULL, CFSTR("/Users/jweinberg/chr.png"), f++); 
//        CFURLRef url = CFURLCreateWithFileSystemPath(NULL, str, kCFURLPOSIXPathStyle, false);
//        
//        CGImageDestinationRef destination = CGImageDestinationCreateWithURL(url, kUTTypePNG, 1, NULL);
//        CGImageDestinationAddImage(destination, image, NULL);
//        
//        CGImageDestinationFinalize(destination);
//        
//        
//        CFRelease(destination);
//        CFRelease(url);
//        CFRelease(str);
//        
//        CGColorSpaceRelease(colorSpace);
//        CGContextRelease(context);
//        free(bmpdata);
//    }
    
    void VIP::DrawObj(const Obj &obj, int row)
    {
        int h = 8;
        //    int w = 8;
        
        int topInset = (row * 8) - (obj.JY + h);
        int bottomInset = ((row * 8) + 8) - obj.JY;
        
        if (topInset >= 0)
            return;
        
        if (bottomInset < 0)
            return;
        
        if (obj.JLON)
        {
            int x = obj.JX - sign_extend<8>(obj.JP);
            if (x + 8 < 0 || x >= 384)
                return;
            
            if (!chrCache[obj.JCA].valid)
                chrCache[obj.JCA].LoadData(chrRam[obj.JCA].data);
            leftFrameBuffer[drawingFB].DrawCachedChr(chrCache[obj.JCA], row, x, obj.JY, 0, 0, 8, 8, obj.JHFLP, obj.JVFLP, JPLT[obj.JPLTS]);
        }
        if (obj.JRON)
        {
            int x = obj.JX + sign_extend<8>(obj.JP);
            if (x + 8 < 0 || x >= 384)
                return;
            
            if (!chrCache[obj.JCA].valid)
                chrCache[obj.JCA].LoadData(chrRam[obj.JCA].data);
            rightFrameBuffer[drawingFB].DrawCachedChr(chrCache[obj.JCA], row, x, obj.JY, 0, 0, 8, 8, obj.JHFLP, obj.JVFLP, JPLT[obj.JPLTS]);
        }
        
    }
    
    void VIP::DrawNormalWorld(int row, const World &world)
    {
        int xWorlds = 1 << world.SCX;
        int yWorlds = 1 << world.SCY;;
        
        BGMapLookup mapLookup(*this, &bgMaps[world.BGMAP_BASE], xWorlds, yWorlds, world.OVER, world.OVERPLANE_CHARACTER);
        
        int y = max<int>((row * 8) - world.GY, 0);
        do
        {   
            int srcY = (y + world.MY);
            int yOff = srcY & 7;
            
            mapLookup.SetY(srcY);
            
            int h = 8 - yOff;
            h = min<int>(h, world.H + 1 - y);
            
            if (world.LON)
            {
                int x = 0;
                do
                {
                    int srcX = (x + world.MX - world.MP);
                    int xOff = srcX & 7;
                    
                    mapLookup.SetX(srcX);
                    
                    int w = 8 - xOff;
                    w = min<int>(w, world.W + 1 - x);
                    
                    int xPos = x + world.GX - sign_extend<9>(world.GP);
                    if (!(xPos + w < 0 || xPos >= 384))
                    {
                        const BGMapData& data = mapLookup.GetMapData();
                        
                        if (!chrCache[data.charNum].valid)
                            chrCache[data.charNum].LoadData(chrRam[data.charNum].data);
                        leftFrameBuffer[drawingFB].DrawCachedChr(chrCache[data.charNum], row, xPos, y + world.GY, xOff, yOff, w, h, data.BHFLP, data.BVFLP, GPLT[data.GPLTS]);
                    }
                    x += (8 - xOff);
                } while(x <= world.W);
            }
            
            if (world.RON)
            {
                int x = 0;
                do
                {
                    int srcX = (x + world.MX + world.MP);
                    int xOff = srcX & 7;
                    
                    mapLookup.SetX(srcX);
                    
                    int w = 8 - xOff;
                    w = min<int>(w, world.W + 1 - x);
                    
                    int xPos = x + world.GX + sign_extend<9>(world.GP);
                    if (!(xPos + w < 0 || xPos >= 384))
                    {
                        const BGMapData& data = mapLookup.GetMapData();
                        if (!chrCache[data.charNum].valid)
                            chrCache[data.charNum].LoadData(chrRam[data.charNum].data);
                        
                        rightFrameBuffer[drawingFB].DrawCachedChr(chrCache[data.charNum], row, xPos, y + world.GY, xOff, yOff, w, h, data.BHFLP, data.BVFLP, GPLT[data.GPLTS]);
                    }
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
        
        BGMapLookup mapLookup(*this, &bgMaps[world.BGMAP_BASE], xWorlds, yWorlds, world.OVER, world.OVERPLANE_CHARACTER);
        
        int y = max<int>((row * 8) - world.GY, 0);
        
        for (; y < (max<int>((row * 8) - world.GY, 0) + 8) && y <= world.H; ++y)
        {
            const AffineTable &affineTable = *memoryLookup<AffineTable>(((world.PARAM_BASE & 0xFFF0) * 2 + 0x00020000) + y * sizeof(AffineTable));
            
            int rightParalax = (affineTable.MP >= 0 ? affineTable.MP : 0);
            int leftParalax = (affineTable.MP < 0 ? affineTable.MP : 0);
            
            for (int x = 0; x < world.W; ++x)
            {
                int srcY = Fixed16x16(affineTable.MY) + Fixed16x16(affineTable.DY) * (((int64_t)x) << 16);
                mapLookup.SetY(srcY);
                
                uint8_t yOff = srcY & 7;
                
                if (world.LON)
                {
                    int xPos = x + world.GX - sign_extend<9>(world.GP);
                    if (!(xPos < 0 || xPos >= 384))
                    {
                        int srcX = (Fixed16x16(affineTable.MX) + (Fixed16x16(affineTable.DX) * (((int64_t)(x + leftParalax)) << 16)));
                        
                        mapLookup.SetX(srcX);
                        
                        uint8_t xOff = srcX & 7;
                        
                        const BGMapData &data = mapLookup.GetMapData();
                        const Chr& chr = chrRam[data.charNum];  
                        
                        //We know we're rather safe here, don't need all the checks DrawChr does
                        uint16_t row = chr.data[data.BVFLP ? (7 - yOff) : yOff];
                        uint8_t shift = ((data.BHFLP ? 7 - xOff : xOff) * 2);
                        uint8_t idx = (row >> shift) & 0x3;
                        if (idx)
                        {
                            leftFrameBuffer[drawingFB].SetPixel(xPos, y + world.GY, GPLT[data.GPLTS][idx]);
                        }
                    }
                }
                if (world.RON)
                {
                    int xPos = x + world.GX + sign_extend<9>(world.GP);
                    if (!(xPos < 0 || xPos >= 384))
                    {                                
                        int srcX = (Fixed16x16(affineTable.MX) + (Fixed16x16(affineTable.DX) * (((int64_t)((x + rightParalax))) << 16)));
                        mapLookup.SetX(srcX);
                        
                        uint8_t xOff = srcX & 7;
                        
                        const BGMapData &data = mapLookup.GetMapData();
                        const Chr& chr = chrRam[data.charNum];  
                        
                        //We know we're rather safe here, don't need all the checks DrawChr does
                        uint16_t row = chr.data[data.BVFLP ? (7 - yOff) : yOff];
                        uint8_t shift = ((data.BHFLP ? 7 - xOff : xOff) * 2);
                        uint8_t idx = (row >> shift) & 0x3;
                        if (idx)
                        {
                            rightFrameBuffer[drawingFB].SetPixel(xPos, y + world.GY, GPLT[data.GPLTS][idx]);
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
        
        BGMapLookup mapLookup(*this, &bgMaps[world.BGMAP_BASE], xWorlds, yWorlds, world.OVER, world.OVERPLANE_CHARACTER);
        
        int y = max<int>((row * 8) - world.GY, 0);
        
        for (; y < (max<int>((row * 8) - world.GY, 0) + 8) && y <= world.H; ++y)
        {
            const HBiasTable &hBiasTable = *memoryLookup<HBiasTable>((world.PARAM_BASE & 0xFFF0) * 2 + 0x00020000 + y * sizeof(HBiasTable));
            
            int srcY = (y + world.MY);
            mapLookup.SetY(srcY);
            uint8_t yOff = srcY & 7;
            
            if (world.LON)
            {
                int x = 0;
                do
                {
                    int srcX = (x + world.MX - world.MP + hBiasTable.HOFSTL);
                    
                    mapLookup.SetX(srcX);
                    int xOff = srcX & 7;
                    
                    int w = 8 - xOff;
                    w = min<int>(w, world.W + 1 - x);
                    
                    int xPos = x + world.GX - sign_extend<9>(world.GP);
                    if (!(xPos + w < 0 || xPos >= 384))
                    {
                        const BGMapData &data = mapLookup.GetMapData();
                        if (!chrCache[data.charNum].valid)
                            chrCache[data.charNum].LoadData(chrRam[data.charNum].data);
                        
                        leftFrameBuffer[drawingFB].DrawCachedChr(chrCache[data.charNum], row, xPos, y + world.GY, xOff, yOff, w, 1, data.BHFLP, data.BVFLP, GPLT[data.GPLTS]);
                    }
                    x += (8 - xOff);
                } while(x <= world.W);
            }
            
            if (world.RON)
            {
                int x = 0;
                do
                {
                    int srcX = (x + world.MX + world.MP + hBiasTable.HOFSTR);
                    
                    mapLookup.SetX(srcX);
                    int xOff = srcX & 7;
                    
                    int w = 8 - xOff;
                    w = min<int>(w, world.W + 1 - x);
                    
                    int xPos = x + world.GX + sign_extend<9>(world.GP);
                    if (!(xPos + w < 0 || xPos >= 384))
                    {
                        
                        const BGMapData &data = mapLookup.GetMapData();
                        if (!chrCache[data.charNum].valid)
                            chrCache[data.charNum].LoadData(chrRam[data.charNum].data);
                        
                        rightFrameBuffer[drawingFB].DrawCachedChr(chrCache[data.charNum], row, xPos, y + world.GY, xOff, yOff, w, 1, data.BHFLP, data.BVFLP, GPLT[data.GPLTS]);
                    }
                    x += (8 - xOff);
                } while(x <= world.W);
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
        int32_t clocks = cycles - lastFrameBuffer;
        int32_t running_timestamp = cycles;
        
        if (cycles < sbOutResetTime)
            XPSTTS.SBOUT = 0;
        DPSTTS.SCANRDY = 1;
        while (clocks > 0)
        {
            int32_t chunk_clocks = clocks;
            
            if (drawingCounter > 0 && chunk_clocks > drawingCounter)
                chunk_clocks = drawingCounter;
            if (chunk_clocks > columnCounter)
                chunk_clocks = columnCounter;
            
            running_timestamp += chunk_clocks;
            
            if (drawingCounter > 0)
            {
                drawingCounter -= chunk_clocks;
                if (drawingCounter <= 0)
                {
                    Draw(rowCount);
                    
                    if (DPSTTS.DISP)
                    {
                        char * internalDataLeft = (char*)&leftFrameBuffer[displayFB];
                        char * internalDataRight = (char*)&rightFrameBuffer[displayFB];    
                        
                        for (int x = 0; x < 384; ++x)
                        {
                            for (int y = rowCount * 2; y < rowCount * 2 + 2; ++y)
                            {
                                for (int bt = 0; bt < 4; ++bt)
                                {
                                    uint8_t color = 0;
                                    
                                    char leftPx = (*(internalDataLeft + (x * 64) + y) >> (bt * 2)) & 0x3;
                                    if (leftPx == 1)
                                        color = BRT[0].val;
                                    else if (leftPx == 2)
                                        color = BRT[1].val;
                                    else if (leftPx == 3)
                                        color = (BRT[2].val + BRT[1].val + BRT[0].val);
                                    
                                    leftBmpData[(384 * (y * 4 + bt)) + x] = color;
                                    
                                    color = 0;
                                    char rightPx = (*(internalDataRight + (x * 64) + y) >> (bt * 2)) & 0x3;
                                    if (rightPx == 1)
                                        color = BRT[0].val;// << 16;
                                    else if (rightPx == 2)
                                        color = BRT[1].val;// << 16;
                                    else if (rightPx == 3)
                                        color = (BRT[2].val + BRT[1].val + BRT[0].val);// << 16;
                                    
                                    
                                    rightBmpData[(384 * (y * 4 + bt)) + x] = color;
                                    
                                    

                                }
                            }
                        }
                    }
                    
                    sbOutResetTime = running_timestamp + 1120;
                    XPSTTS.SBCOUNT = rowCount;
                    
                    rowCount++;
                    if(rowCount == 28)
                    {
                        drawingActive = false;
                        XPSTTS.XPBSY = 0;
                        INTPND.XPEND |= 1;
                        DPSTTS.FCLK = 0;
                        if (INTENB.XPEND)
                            cpu->processInterrupt(CPU::INTVPU);
                    }
                    else
                        drawingCounter += 1120 * 4;
                }
            }
            
            columnCounter -= chunk_clocks;
            if (columnCounter == 0)
            {
                if (displayRegion & 1)
                {
                    if (!(column & 3))
                    {
                        //                        const int lr = (displayRegion & 2) >> 1;
                        //                        uint16 ctdata = VIP_MA16R16(DRAM, 0x1DFFE - ((Column >> 2) * 2) - (lr ? 0 : 0x200));
                        //                        
                        //                        if((ctdata >> 8) != Repeat)
                        //                        {
                        //                            Repeat = ctdata >> 8;
                        //                            RecalcBrightnessCache();
                        //                        }
                    }
                    //                    CopyFBColumnToTarget();
                }
                
                columnCounter = 259;
                column++;
                if (column == 384)
                {
                    column = 0;
                    
                    if (DPSTTS.DISP)
                    {
                        if (displayRegion & 1)	// Did we just finish displaying an active region?
                        {
                            if (displayRegion & 2)	// finished displaying right eye
                            {
                                INTPND.RFBEND |= 1;
                                if (INTENB.RFBEND)
                                    cpu->processInterrupt(CPU::INTVPU);
                            }
                            else		// Otherwise, left eye
                            {
                                INTPND.LFBEND |= 1;
                                if (INTENB.LFBEND)
                                    cpu->processInterrupt(CPU::INTVPU);
                                
                            }
                        }
                    }
                    
                    displayRegion = (displayRegion + 1) & 3;
                    DPSTTS.DPBSY = displayRegion;
                    if (displayRegion == 0)	// New frame start
                    {
                        DPSTTS.FCLK = 1;
                    
                        INTPND.FRAMESTART |= 1;
                        if (INTENB.FRAMESTART)
                            cpu->processInterrupt(CPU::INTVPU);

                        frame++;
                        if(frame > FRMCYC) // New game frame start?
                        {
                            
                            INTPND.GAMESTART |= 1;
                            if (INTENB.GAMESTART)
                                cpu->processInterrupt(CPU::INTVPU);

                            rowCount = 0;
                            drawingCounter = 1120 * 4;
                            
                            if (XPCTRL.XPEN)
                            {
                                displayFB ^= 1;

                                drawingActive = true;
                                drawingFB = displayFB ^ 1;
                                char col = (BKCOL & 3) | (BKCOL & 3) << 2 |  (BKCOL & 3) << 4 |  (BKCOL & 3) << 6 ;
                                
                                memset((char*)&rightFrameBuffer[drawingFB], col, 0x6000);
                                memset((char*)&leftFrameBuffer[drawingFB], col, 0x6000);
                                XPSTTS.XPBSY = 1 + drawingFB;
                            }
                            
                            frame = 0;
                        }
                        
                        break;
                        //                        VB_ExitLoop();
                    }
                }
            }
            
            clocks -= chunk_clocks;
        }
        
        lastFrameBuffer = cycles;
        
        return 0;
    }
}