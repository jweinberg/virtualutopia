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

    template<typename T, int min, int max>
    class ClampedInt
    {
        T s;
    public:
        void operator =(T& _s) const
        { 
            s = _s;
        }
      
        operator T() const 
        { 
            if (s < min)
                return min;
            else if (s > max)
                return max;
            return s;
        }
    };
    
    
    struct World
    {
        enum WorldType
        {
            kEndType,
            kDummyType,
            kObjType,
            kNormalType,
            kHBiasType,
            kAffineType,
        };
        
        union
        {
            struct
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
                
                ClampedInt<int16_t, -512, 511> GX;
                ClampedInt<int16_t, -512, 511> GP;
                ClampedInt<int16_t, 0, 223> GY;
                int16_t MX;
                int16_t MP;
                int16_t MY;
                ClampedInt<int16_t, 0, 383>  W;
                ClampedInt<int16_t, 0, 223>  H;
                int16_t reserved_1:4;
                int16_t PARAM_BASE:12;
                int16_t OVERPLANE_CHARACTER;
                
                int16_t reserved_2[5];
            };
            int16_t data[4];
        };
        
        WorldType Type()
        {
            if (END)
                return kEndType;
            if (data[0] == 0)
                return kDummyType;
            
            switch (BGM) 
            {
                case 0:
                    return kNormalType;
                case 1:
                    return kHBiasType;
                case 2:
                    return kAffineType;
                case 3:
                    return kObjType;
            }
            return kDummyType;
        }
    };
}


#endif
