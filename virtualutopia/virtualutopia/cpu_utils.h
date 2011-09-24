//
//  cpu_utils.h
//  virtualutopia
//
//  Created by Joshua Weinberg on 9/7/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef virtualutopia_cpu_utils_h
#define virtualutopia_cpu_utils_h

    inline int sign(int32_t v)
    {
        return (v >> 31);
    }
    
    inline int32_t sign_extend(int bits, uint32_t rawValue)
    {
        bits -= 1;
        uint32_t signBitMask = 1 << bits;
        char signValue = (rawValue & signBitMask) >> bits;
        return rawValue | (signValue ? (0xFFFFFFFF << bits) : 0);
    }
    
    inline bool calculate_overflow_subtract(int32_t a, int32_t b)
    {
        int32_t result = b - a;
        if (sign(b) == 1 && sign(a) == 0 && sign(result) == 0)
            return true;
        if (sign(b) == 0 && sign(a) == 1 && sign(result) == 1)
            return true;
        return false;
    }


#endif
