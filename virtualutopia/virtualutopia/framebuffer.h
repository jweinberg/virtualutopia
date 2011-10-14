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
        
        void DrawChr(const Chr &chr, int xoff, int yoff, int sourceXOffset, int sourceYOffset, int w, int h, bool flipHor, bool flipVert,const Palette &palette)
        {
            const uint16_t * const data = chr.data;
            
            //See what is offscreen or not
            if (xoff + w < 0 || yoff + h < 0 || xoff >= 384 || yoff >= 224)
                return;
            
            for (int y = 0; y < h; ++y)
            {                   
                int yPxl = y + yoff;
                if (yPxl < 0 || yPxl >= 224)
                    continue;
                
                int yIdx = y + sourceYOffset;
                if (flipVert) yIdx = 7 - yIdx;
                uint16_t row = data[yIdx];
                for (int x = 0; x < w; ++x)
                {
                    int xPxl = x + xoff;
                    if (xPxl < 0 || xPxl >= 384)
                        continue;

                    int xIdx = x + sourceXOffset;
                    if (flipHor) xIdx = 7 - xIdx;
                    uint8_t colorIdx = (row >> (xIdx * 2)) & 0x3;
                    if (colorIdx)
                    {
                        char * column = this->data + (xPxl * 64);
                        column += (yPxl / 4);
                        
                        int shift = (yPxl % 4) * 2;
                        *column &= ~(0x3 << shift);
                        *column |= (palette[colorIdx] << shift);
                    }
                }
            }   
        }
    };
}

#endif
