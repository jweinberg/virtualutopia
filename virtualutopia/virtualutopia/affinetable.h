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
};

struct Fixed7x9
{
    int16_t integer;
};

struct Fixed16x16
{
    int32_t integer;
    
    const int16_t operator+(const Fixed16x16& other)
    {
        return ((int64_t)integer + (int64_t)other.integer) >> 16;
    }
 
    const Fixed16x16 operator*(const Fixed16x16& other)
    {
        int64_t combined = ((int64_t)integer * (int64_t)other.integer);
        return combined >> 16;
    }
    
    Fixed16x16(int64_t val) : integer((int32_t)val) {}
    Fixed16x16(const Fixed7x9& other) :integer(other.integer << 7) {}
    Fixed16x16(const Fixed13x3& other) :integer(other.integer << 13) {}
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
