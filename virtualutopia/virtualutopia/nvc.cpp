//
//  nvc.cpp
//  virtualutopia
//
//  Created by Joshua Weinberg on 9/28/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "nvc.h"
 
char &NVC::NVC::operator[](uint32_t address) const
{
    switch (address)
    {
        case 0x2000000:
            return *((char*)&CCR);
        case 0x2000004:
            return *((char*)&CCSR);
        case 0x2000008:
            return *((char*)&CDTR);
        case 0x200000C:
            return *((char*)&CDRR);
        case 0x2000010:
            return *((char*)&SDLR);
        case 0x2000014:
            return *((char*)&SDHR);
        case 0x2000018:
            return *((char*)&TLR);
        case 0x200001C:
            return *((char*)&THR);
        case 0x2000020:
            return *((char*)&TCR);
        case 0x2000028:
        default:
            return *((char*)&SCR);
    }
}