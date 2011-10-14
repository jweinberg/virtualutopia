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

namespace VIP
{
    class Framebuffer
    {
        public:
        char data[0x6000];
        
        void SetPixel(int x, int y, int val)
        {
            //screen width = 384
            //screen height = 224
            
            if (x < 0 || y < 0 || x >= 384 || y >= 256)
                return;
            
            //Data is stored by column, not by row
            
            //We need to figure out what byte of data to look at, each 'pixel'
            // is 2 bits, each byte store 4 pixels
            
            //One column is 64 bytes of data
            char * column = data + (x * 64);
            column += (y / 4);
            if (column > data + 0x6000)
                return;
            
            int shift = (y % 4) * 2;
            *column &= ~(0x3 << shift);
            *column |= (val << shift);
        }
        
        inline void DrawChr(const Chr &chr, int xoff, int yoff, int sourceXOffset, int sourceYOffset, int w, int h, bool flipHor, bool flipVert,const Palette &palette)
        {
            const uint16_t * const data = chr.data;

            for (int y = 0; y < h; ++y)
            {                   
                int yIdx = y + sourceYOffset;
                if (flipVert) yIdx = 7 - yIdx;
                uint16_t row = data[yIdx];
                for (int x = 0; x < w; ++x)
                {
                    int xIdx = x + sourceXOffset;
                    if (flipHor) xIdx = 7 - xIdx;
                    uint8_t colorIdx = (row >> (xIdx * 2)) & 0x3;
                    if (colorIdx)
                        SetPixel(x + xoff, y + yoff, palette[colorIdx]);
                }
            }   
        }
    };
}

#endif
