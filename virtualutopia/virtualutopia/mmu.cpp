#include <iostream>
#include "mmu.h"
#include "rom.h"
#include "vip.h"

MMU::MMU(const ROM &_rom, VIP::VIP &_vip) : rom(_rom), vip(_vip)
{
}

const char * MMU::rawData(uint32_t virtualAddress) const
{
//    std::cout << "Accessing " << std::hex << virtualAddress << " in ";
    virtualAddress = virtualAddress & 0x07FFFFFF;
    switch (virtualAddress)
    {
        case 0x0 ... 0x7FFFF: //Display RAM, VIP
//            std::cout << "display region" << std::endl;
            return vip[virtualAddress];
        case 0x01000000 ... 0x010005FF: //Sound Memory
//            std::cout << "sound region" << std::endl;
            break;
        case 0x02000000 ... 0x0200002C: //Hardware registers
            return &registers[virtualAddress & 0xFF];
//            std::cout << "hardware region" << std::endl;
            break;
        case 0x04000000 ... 0x04FFFFFF: //Expansion Area
//            std::cout << "expansion region" << std::endl;
            break;
        case 0x05000000 ... 0x0500FFFF: //Program RAM (mask with 0xFFFF)
//            std::cout << "program ram region" << std::endl;
            return &programRam[virtualAddress & 0xFFFF];
        case 0x06000000 ... 0x06001FFF: //Cartridge RAM
//            std::cout << "cart ram region" << std::endl;
            break;
        case 0x07000000 ... 0x07FFFFFF: //Cartridge ROM
//            std::cout << "cart rom region" << std::endl;
            return rom[virtualAddress];
    }
    return 0;
}

void MMU::Store(uint32_t address, uint8_t byte)
{
    uint8_t * rawData = (uint8_t*)this->rawData(address);
    *rawData = byte;
}

void MMU::StoreHWord(uint32_t address, uint16_t hword)
{
    uint16_t * rawData = (uint16_t*)this->rawData(address);
    if (rawData)
        *rawData = hword;    
    else
        printf("Accessing incorrect memory at 0x%X\n", address);
}

void MMU::StoreWord(uint32_t address, uint32_t word)
{
    uint32_t * rawData = (uint32_t*)this->rawData(address);
    *rawData = word;    
}