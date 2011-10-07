#include <iostream>
#include <assert.h>
#include "mmu.h"
#include "rom.h"
#include "vip.h"

MMU::MMU(const ROM &_rom, VIP::VIP &_vip) : rom(_rom), vip(_vip)
{
    gamepackRam = (char*)calloc(0xFFFFFF, sizeof(char));
}

const char &MMU::operator[](uint32_t virtualAddress) const
{
    virtualAddress = virtualAddress & 0x07FFFFFF;
    switch (virtualAddress)
    {
        case 0x0 ... 0xFFFFFF: //Display RAM, VIP
            return vip[virtualAddress % 0x7FFFF];
        case 0x01000000 ... 0x010005FF: //Sound Memory
            return soundRegisters[virtualAddress & 0x5FF];
        case 0x02000000 ... 0x0200002C: //Hardware registers
            return registers[virtualAddress & 0xFF];
        case 0x04000000 ... 0x04FFFFFF: //Expansion Area
            break;
        case 0x05000000 ... 0x0500FFFF: //Program RAM (mask with 0xFFFF)
            return programRam[virtualAddress & 0xFFFF];
        case 0x06000000 ... 0x06FFFFFF: //Cartridge RAM
            return gamepackRam[virtualAddress & 0xFFFFFF];
        case 0x07000000 ... 0x07FFFFFF: //Cartridge ROM
            return rom[virtualAddress];
    }
    assert(false);
    return rom[0];
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