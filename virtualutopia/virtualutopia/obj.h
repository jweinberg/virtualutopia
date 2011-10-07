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
    class Obj
    {
        int16_t JX;
        int16_t JP:14;
        int16_t JRON:1;
        int16_t JLON:1;
        int16_t JY;
        int16_t JCA:11;
        int16_t reserved:1;
        int16_t JVFLP:1;
        int16_t JHFLP:1;
        int16_t JPLTS:2;
    };
}


#endif
