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
    vsu = new VSU::VSU();
    mmu = new MMU(*rom, *vip, *nvc, *vsu);
    cpu = new CPU::v810(*mmu, *vip, *nvc, *vsu);

    vip->cpu = cpu;
    nvc->cpu = cpu;
    
    cpu->reset();
}

VB::~VB()
{
    delete rom;
    delete vip;
    delete nvc;
    delete mmu;
    delete cpu;
}