//
//  palette.h
//  virtualutopia
//
//  Created by Joshua Weinberg on 10/8/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef virtualutopia_palette_h
#define virtualutopia_palette_h

namespace VIP
{
    class Palette
    {
    public:
        uint16_t transparent:2;
        uint16_t color_1:2;
        uint16_t color_2:2;
        uint16_t color_3:2;
        uint16_t reserved:8;
        
        Palette()
        {
            transparent = 0;
        }
        
        uint8_t operator[](uint8_t index) const
        {
            switch (index) 
            {
                case 1:
                    return color_1;
                case 2:
                    return color_2;
                case 3:
                    return color_3;
                default:
                    return 0;
            }
        }
    };
}

#endif
