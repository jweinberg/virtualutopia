//
//  registers.h
//  virtualutopia
//
//  Created by Joshua Weinberg on 9/28/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef virtualutopia_registers_h
#define virtualutopia_registers_h

#define REGISTER_BITFIELD(T, NAME, ...) \
struct _##NAME \
{\
    __VA_ARGS__\
    operator T&() { return *(T*)this; }\
    T &operator =(const T& val) { *(T*)this = val; return *(T*)this; }\
} NAME


#endif
