//
//  rom.cpp
//  virtualutopia
//
//  Created by Joshua Weinberg on 9/5/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "rom.h"
#include <assert.h>
#include <stdio.h>

ROM::ROM(const std::string &filePath)
{
    FILE *f = fopen(filePath.c_str(), "rb");
    fseek(f, 0, SEEK_END);
    fileLength = ftell(f);
    fseek(f, 0, SEEK_SET);
    data = new char[fileLength];
    fread(data, sizeof(char), fileLength, f);
    fclose(f);
}

ROM::~ROM()
{
    delete[] data;
}

const std::string ROM::romName() const
{
    const int nameMaxLength = 0x07FFFDF3 - 0x07FFFDE0;
    char nameBuffer[nameMaxLength+1] = {0};
    memcpy(nameBuffer, (*this)[0x07FFFDE0], nameMaxLength);
    return std::string(nameBuffer);
}

const char* ROM::operator[](uint32_t address) const
{
    //Address needs to be in range, should be taken care of by the MMU
   // assert(address > 0x07000000 && address < 0x07FFFFFF);
    
    //The data is mirrored from 0x07000000 to 0x07FFFFFF
    address = ((address & 0x07FFFFFF) - 0x07000000);
    return &data[address % fileLength];
}