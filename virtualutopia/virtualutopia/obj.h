//
//  obj.h
//  virtualutopia
//
//  Created by Joshua Weinberg on 10/7/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef virtualutopia_obj_h
#define virtualutopia_obj_h

namespace VIP
{
    struct Obj
    {
        int16_t JX;
        int16_t JP:14;
        bool JRON:1;
        bool JLON:1;
        int16_t JY;
        uint16_t JCA:11;
        int16_t reserved:1;
        bool JVFLP:1;
        bool JHFLP:1;
        uint16_t JPLTS:2;
    };
}


#endif
