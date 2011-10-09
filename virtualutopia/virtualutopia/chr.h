//
//  chr.h
//  virtualutopia
//
//  Created by Joshua Weinberg on 10/7/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef virtualutopia_chr_h
#define virtualutopia_chr_h

namespace VIP
{
    //2 bits for each pixel
    //Data is 8x8
    struct Chr
    {
        uint16_t data[8];
        
        void FlipVertical()
        {
            uint16_t oldData[8];
            memcpy(oldData, data, sizeof(uint16_t) * 8);
            
            for (int i = 7; i >= 0; --i)
            {
                data[7 - i] = oldData[i];
            }
        }
        
        void FlipHorizontal()
        {
            for (int i = 0; i < 8; ++i)
            {
                uint16_t row = data[i];
                data[i] = (((row >> 0) & 0x3) << 14) | 
                          (((row >> 2) & 0x3) << 12) |
                          (((row >> 4) & 0x3) << 10) |
                          (((row >> 6) & 0x3) << 8) |
                          (((row >> 8) & 0x3) << 6) |
                          (((row >> 10) & 0x3) << 4) |
                          (((row >> 12) & 0x3) << 2) |
                            (((row >> 14) & 0x3) << 0);
            }
        }
    };
    
   }

#endif
