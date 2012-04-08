//
//  rom.h
//  virtualutopia
//
//  Created by Joshua Weinberg on 9/5/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef virtualutopia_rom_h
#define virtualutopia_rom_h
#include <string>

class ROM {
public:
    char *data;
    long fileLength;
public:
    ROM(const std::string &fileName);
    ~ROM();
    
    const std::string romName() const;
    template <typename T>
    const T *memoryLookup(uint32_t address) const
    {
        return ((T*)&(*this)[address]);
    }
    
    inline char& operator[](uint32_t address) const
    {
        //The data is mirrored from 0x07000000 to 0x07FFFFFF
        address = ((address & 0x07FFFFFF) - 0x07000000);
        return data[address & (fileLength-1)];
    }
};


#endif
