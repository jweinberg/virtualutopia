//
//  affinetable.h
//  virtualutopia
//
//  Created by Joshua Weinberg on 10/16/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef virtualutopia_affinetable_h
#define virtualutopia_affinetable_h

struct Fixed13x3
{
    int16_t integer;
    
    operator float() const 
    {
        return (float)integer / 8.0f;
    }
};

struct Fixed7x9
{
    int16_t integer;
    
    operator float() const
    {
        return (float)integer / 512.0f;
    }
};

struct AffineTable
{
    Fixed13x3 MX;
    int16_t MP;
    Fixed13x3 MY;
    Fixed7x9 DX;
    Fixed7x9 DY;
    int16_t reserved[3];
};

#endif
