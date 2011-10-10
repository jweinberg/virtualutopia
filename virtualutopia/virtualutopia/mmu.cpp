#include <iostream>
#include <assert.h>
#include "mmu.h"

MMU::MMU(const ROM &_rom, VIP::VIP &_vip, NVC::NVC &_nvc) : rom(_rom), vip(_vip), nvc(_nvc)
{
    gamepackRam = (char*)calloc(0x20000, sizeof(char));
    programRam = (char*)calloc(0x10000, sizeof(char));
}

void MMU::Store(uint32_t address, uint8_t byte)
{
    uint8_t * rawData = (uint8_t*)&(*this)[address];
    *rawData = byte;
}

void MMU::StoreHWord(uint32_t address, uint16_t hword)
{
    uint16_t * rawData = (uint16_t*)&(*this)[address];
    assert((void*)rawData != (const void*)&rom);
        
    if (rawData)
        *rawData = hword;    
    else
        printf("Accessing incorrect memory at 0x%X trying to store 0x%X\n", address, hword);
}

void MMU::StoreWord(uint32_t address, uint32_t word)
{
    uint32_t * rawData = (uint32_t*)&(*this)[address];
    *rawData = word;    
}