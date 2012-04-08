//
//  memory.h
//  virtualutopia
//
//  Created by Joshua Weinberg on 9/17/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef virtualutopia_memory_h
#define virtualutopia_memory_h

template <typename X>
class Memory
{
public:
    template <typename T>
    class Reader
    {
        const X &memory;
    public:
        Reader(const X &_memory) : memory(_memory)
        {}
        
        inline T operator()(const int offset)
        {
            const T *ptr = memory.template memoryLookup<T>(offset);
            if (ptr)
                return *ptr;
            T val = 0;
            return val;
        }
    };
    
    template <typename T>
    class Reader<T*>
    {
        const X &memory;
    public:
        Reader(const X &_memory) : memory(_memory)
        {}
        
        inline const T* operator()(const int offset)
        {
            return memory.template memoryLookup<T>(offset);
        }
    };
};


#endif
