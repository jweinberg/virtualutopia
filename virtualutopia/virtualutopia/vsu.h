//
//  vsu.h
//  virtualutopia
//
//  Created by Joshua Weinberg on 10/23/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef virtualutopia_vsu_h
#define virtualutopia_vsu_h
#include <stdint.h>

namespace VSU
{
    class VSU
    {
    public:
        template <typename T>
        inline const T& read(uint32_t address) const
        {
            return *(T*)&soundRegisters[address];
        }

        template <typename T>
        inline void store(T& val, const int offset)
        {
            T* t = (T*)&soundRegisters[offset];
            *t = val;
        }
        
    private:
        char soundRegisters[0x600];
    };
}

#endif
