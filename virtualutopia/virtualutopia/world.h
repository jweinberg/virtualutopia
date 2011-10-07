//
//  World.h
//  virtualutopia
//
//  Created by Joshua Weinberg on 10/7/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef virtualutopia_World_h
#define virtualutopia_World_h

namespace VIP
{
    class World
    {
        uint16_t BGMAP_BASE:4;
        uint16_t reserved_0:2;
        uint16_t END:1;
        uint16_t OVER:1;
        uint16_t SCY:2;
        uint16_t SCX:2;
        uint16_t BGM:2;
        uint16_t RON:1;
        uint16_t LON:1;
        
        int16_t GX;
        int16_t GP;
        int16_t GY;
        int16_t MX;
        int16_t MP;
        int16_t MY;
        int16_t W;
        int16_t H;
        int16_t reserved_1:4;
        int16_t PARAM_BASE:12;
        int16_t OVERPLANE_CHARACTER;
        
        int16_t reserved_2[5];
    };
}


#endif
