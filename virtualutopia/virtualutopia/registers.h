//
//  registers.h
//  virtualutopia
//
//  Created by Joshua Weinberg on 9/28/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef virtualutopia_registers_h
#define virtualutopia_registers_h

#ifndef REGISTER_BITFIELD

#define REGISTER_BITFIELD(NAME, ...) \
struct \
{\
    __VA_ARGS__\
    operator uint32_t&() { return *(uint32_t*)this; }\
    uint32_t &operator =(const uint32_t& val) { *(uint32_t*)this = val; return *(uint32_t*)this; }\
} NAME

#endif

#endif
