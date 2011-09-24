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
private:
    char *data;
    long fileLength;
public:
    ROM(const std::string &fileName);
    ~ROM();
    
    const std::string romName() const;
    const char* operator[](uint32_t address) const;
};


#endif
