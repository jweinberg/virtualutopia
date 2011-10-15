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
        
        void DrawChr(const Chr &chr, uint8_t row, int xoff, int yoff, int sourceXOffset, int sourceYOffset, int w, int h, bool flipHor, bool flipVert,const Palette &palette)
        {
            const uint16_t * const data = chr.data;
            const uint8_t expandedRow = row * 8;
            
            //See what is offscreen or not
            if (xoff + w < 0 || yoff + h < expandedRow || xoff >= 384 || yoff >= (expandedRow + 8))
                return;
            
            int yOver = (expandedRow + 8) - (yoff + h);
            uint8_t y = 0;
            if (yOver > -h && yOver < 0)
                h += yOver;
            
            int xOver = 384 - (xoff + w);
            uint8_t xStart = 0;
            if (xOver > -w && xOver < 0)
                w += xOver;
            
            if (xoff < 0)
                xStart = -xoff;
            
            if (yoff < expandedRow) 
                y = -(yoff - expandedRow);
            
            for (; y < h; ++y)
            {                   
                uint16_t yPxl = y + yoff;    
                uint8_t yIdx = y + sourceYOffset;
                if (flipVert) yIdx = 7 - yIdx;
                uint16_t row = data[yIdx];
                for (uint8_t x = xStart; x < w; ++x)
                {
                    uint16_t xPxl = x + xoff;
                    uint8_t xIdx = x + sourceXOffset;
                    if (flipHor) xIdx = 7 - xIdx;
                    uint8_t colorIdx = (row >> (xIdx * 2)) & 0x3;
                    if (colorIdx)
                    {
                        char * column = this->data + (xPxl * 64);
                        column += (yPxl / 4);
                        
                        uint8_t shift = (yPxl & 3) * 2;
                        *column &= ~(0x3 << shift);
                        *column |= (palette[colorIdx] << shift);
                    }
                }
            }   
        }
    };
}

#endif
