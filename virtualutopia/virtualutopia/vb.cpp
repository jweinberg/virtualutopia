//
//  vb.cpp
//  virtualutopia
//

#include <iostream>
#include "vb.h"

VB::VB(const std::string &fileName)
{
    rom = new ROM(fileName);
    vip = new VIP::VIP();
    nvc = new NVC::NVC();
    mmu = new MMU(*rom, *vip, *nvc);
    cpu = new CPU::v810(*mmu, *vip);
    vip->cpu = cpu;
}

VB::~VB()
{
    delete rom;
    delete vip;
    delete nvc;
    delete mmu;
    delete cpu;
}