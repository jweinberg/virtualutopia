//
//  cpu_utils.h
//  virtualutopia
//
//  Created by Joshua Weinberg on 9/7/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef virtualutopia_cpu_utils_h
#define virtualutopia_cpu_utils_h

    template <typename T>
    inline T min(T a, T b)
    {
        return (a > b) ? b : a;
    }

    template <typename T>
    inline T max(T a, T b)
    {
        return (a < b) ? b : a;
    }

    inline uint8_t sign(int32_t v)
    {
        return ((uint32_t)v >> 31);
    }
    

    template <int bits>
    inline int32_t sign_extend(uint32_t rawValue)
    {
        const uint32_t signBitValue = 1 << (bits - 1);
        rawValue = rawValue & (~(0xFFFFFFFF << bits));
        int32_t result = ((rawValue ^ signBitValue) - signBitValue);
        return result;
    }

    inline bool calculate_overflow_subtract(int32_t a, int32_t b)
    {
        uint32_t result = b - a;
        if (sign(b) == 1 && sign(a) == 0 && sign(result) == 0)
            return true;
        if (sign(b) == 0 && sign(a) == 1 && sign(result) == 1)
            return true;
        return false;
    }


#endif
