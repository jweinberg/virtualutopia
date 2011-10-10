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
            
            //Data is stored by column, not by row
            
            //We need to figure out what byte of data to look at, each 'pixel'
            // is 2 bits, each byte store 4 pixels
            
            //One column is 64 bytes of data
            char * column = data + (x * 64);
            column += (y / 4);
            
            int shift = (y % 4) * 2;
            *column &= ~(0x3 << shift);
            *column |= (val << shift);
        }
        
        void DrawChr(Chr chr, int xoff, int yoff, int sourceXOffset, int sourceYOffset, int w, int h, bool flipHor, bool flipVert,const Palette &palette)
        {
            if (flipHor)
                chr.FlipHorizontal();
            if (flipVert)
                chr.FlipVertical();
            
            for (int x = sourceXOffset; x < w; ++x)
            {
                for (int y = sourceYOffset; y < h; ++y)
                {
                    uint8_t colorIdx = chr.data[y] & 0x3;
                    if (colorIdx)
                        SetPixel(x + xoff, y + yoff, palette[colorIdx]);
                    chr.data[y] >>= 2;
                }
            }   
        }
    };
}

#endif
