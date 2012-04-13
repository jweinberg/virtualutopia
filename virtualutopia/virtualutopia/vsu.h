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
#include <assert.h>
#include <stdio.h>
#include "registers.h"
#include "channel.h"

namespace VSU
{
    class VSU
    {
    public:
        VSU();
        template <typename T>
        inline T *memoryLookup(uint32_t offset) const
        {
            return ((T*)&(*this)[offset]);
        }
        
        inline char& operator[](uint32_t offset) const
        {
            return *((char*)&soundBuffer[offset]);
//            switch (offset)
//            {
//                case 0x0000 ... 0x007F:
//                    return *((char*)&soundData[0] + offset);
//                case 0x0080 ... 0x00FF:
//                    return *((char*)&soundData[1] + (offset - 0x0080));
//                case 0x0100 ... 0x017F:
//                    return *((char*)&soundData[2] + (offset - 0x0100));
//                case 0x0180 ... 0x01FF:
//                    return *((char*)&soundData[3] + (offset - 0x0180));
//                case 0x0200 ... 0x027F:
//                    return *((char*)&soundData[4] + (offset - 0x0200));
//                case 0x0280 ... 0x02FF:
//                    return *((char*)&soundData[5] + (offset - 0x0280));
//                case 0x0400 ... 0x041F:
//                    return *((char*)&channels[0] + offset - 0x0400);
//                case 0x0440 ... 0x045F:
//                    return *((char*)&channels[1] + (offset - 0x0440));
//                case 0x0480 ... 0x049F:
//                    return *((char*)&channels[2] + (offset - 0x0480));
//                case 0x04C0 ... 0x04DF:
//                    return *((char*)&channels[3] + (offset - 0x04C0));
//                case 0x0500 ... 0x051F:
//                    return *((char*)&channels[4] + (offset - 0x0500));
//                case 0x0540 ... 0x058F:
//                    return *((char*)&channels[5] + (offset - 0x0540));
//                default:
//                    break;
//            }
//            assert(false);
        }

        template <typename T>
        inline void store(T& val, const int offset)
        {
            *((T*)&(*this)[offset]) = val;
        }
        void Step(uint32_t cycles);
    private:
        Channel channels[6];
        int64_t sampleTimer;
        uint32_t lastUpdate;
        char soundBuffer[0x1000];
        char soundData[0x20][6];
    };
}

#endif
