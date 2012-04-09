//
//  framebuffer.h
//  virtualutopia
//
//  Created by Joshua Weinberg on 10/10/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef virtualutopia_framebuffer_h
#define virtualutopia_framebuffer_h

#include "chr.h"
#include <assert.h>
#include "cpu_utils.h"

namespace VIP
{
    class Framebuffer
    {
    
        public:
        char data[0x6000];
    
        inline void SetPixel(uint16_t x, uint16_t y, char color)
        {
            char * column = this->data + (x * 64);
            column += (y / 4);
            
            uint8_t shift = (y & 3) * 2;
            *column &= ~(0x3 << shift);
            *column |= (color << shift);
        }
        
        void DrawCachedChr(const ChrCacheEntry &chrCache, uint8_t row, int xoff, int yoff, int sourceXOffset, int sourceYOffset, int w, int h, bool flipHor, bool flipVert,const Palette &palette)
        {
            const uint8_t * const data = chrCache.expandedData;
            const uint8_t expandedRow = row * 8;
            
            int yOver = (expandedRow + 8) - (yoff + h);
            uint8_t yStart = 0;
            if (yOver > -h && yOver < 0)
                h += yOver;
            
            int xOver = 384 - (xoff + w);
            uint8_t xStart = 0;
            if (xOver > -w && xOver < 0)
                w += xOver;
            
            if (xoff < 0)
                xStart = -xoff;
            
            
            if (yoff < expandedRow) 
                yStart = -(yoff - expandedRow);
            
            for (uint8_t x = xStart; x < w; ++x)
            {
                uint16_t xPxl = x + xoff;
                uint8_t xIdx = x + sourceXOffset;
                if (flipHor) xIdx = 7 - xIdx;
                
                char * column = this->data + (xPxl * 64);
                
                for (uint8_t y = yStart; y < h; ++y)
                {                   
                    uint16_t yPxl = y + yoff;    
                    uint8_t yIdx = y + sourceYOffset;
                    if (flipVert) yIdx = 7 - yIdx;
                    
                    uint8_t colorIdx = data[xIdx + (yIdx * 8)];
                    if (colorIdx)
                    {
                        uint8_t shift = (yPxl & 3) * 2;
                        column[yPxl / 4] &= ~(0x3 << shift);
                        column[yPxl / 4] |= (palette[colorIdx] << shift);
                    }
                }
            }   
        }
    };
}

#endif
