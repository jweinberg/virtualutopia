#include <iostream>
#include <assert.h>
#include "mmu.h"

MMU::MMU(const ROM &_rom, VIP::VIP &_vip, NVC::NVC &_nvc, VSU::VSU &_vsu) : rom(_rom), nvc(_nvc), vip(_vip), vsu(_vsu)
{
    gamepackRam = (char*)calloc(0x20000, sizeof(char));
    programRam = (char*)calloc(0x10000, sizeof(char));
}