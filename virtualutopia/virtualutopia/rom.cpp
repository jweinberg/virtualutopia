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
    memcpy(nameBuffer, &(*this)[0x07FFFDE0], nameMaxLength);
    return std::string(nameBuffer);
}
