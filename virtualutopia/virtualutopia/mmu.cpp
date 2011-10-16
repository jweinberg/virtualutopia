#include <iostream>
#include <assert.h>
#include "mmu.h"

MMU::MMU(const ROM &_rom, VIP::VIP &_vip, NVC::NVC &_nvc) : rom(_rom), vip(_vip), nvc(_nvc)
{
    gamepackRam = (char*)calloc(0x20000, sizeof(char));
    programRam = (char*)calloc(0x10000, sizeof(char));
}