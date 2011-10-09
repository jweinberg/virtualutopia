//
//  vb.h
//  virtualutopia
//
//  Created by Joshua Weinberg on 10/9/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef virtualutopia_vb_h
#define virtualutopia_vb_h

#include "v810.h"
#include "rom.h"
#include "mmu.h"
#include "vip.h"
#include "nvc.h"

class VB
{
public:
    VB(const std::string &fileName);
    ~VB();
    ROM *rom;
    CPU::v810 *cpu;
    VIP::VIP *vip;
    NVC::NVC *nvc;
    MMU *mmu;
};

#endif
