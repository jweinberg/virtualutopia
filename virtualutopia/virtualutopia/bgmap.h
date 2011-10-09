//
//  bgmap.h
//  virtualutopia
//
//  Created by Joshua Weinberg on 10/8/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef virtualutopia_bgmap_h
#define virtualutopia_bgmap_h

struct BGMapData
{
    uint16_t charNum:11;
    uint16_t reserved:1;
    uint16_t BVFLP:1;
    uint16_t BHFLP:1;
    uint16_t GPLTS:2;
};


struct BGMap
{
    BGMapData chars[4096];
};

#endif
