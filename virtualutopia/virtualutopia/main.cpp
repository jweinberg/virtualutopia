//
//  main.cpp
//  virtualutopia
//
//  Created by Joshua Weinberg on 9/4/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include <sgtty.h>
#include <iostream>
#include <fstream>
#include "v810.h"
#include "rom.h"
#include "mmu.h"
#include "vip.h"

int main (int argc, const char * argv[])
{
    ROM rom("test1.vb");
    VIP::VIP vip;
    MMU mmu(rom, vip);
    
    CPU::v810 cpu(mmu);

    //Lets break the term and have some fun!
	struct sgttyb modmodes;
	ioctl(fileno(stdin), TIOCGETP, &modmodes);
	modmodes.sg_flags |= CBREAK;
	ioctl(fileno(stdin), TIOCSETN, &modmodes);
    
    
    bool wait = true;
    while (1)
    {
        if (wait)
        {
            switch (getchar())
            {
                case 'r':
                case 'R':
                    std::cout << cpu.registerDescription() << std::endl;
                    break;
                case 'c':
                case 'C':
                    wait = false;
                    break;
                default:
                    cpu.step();
                    break;
            }
        }
        else
            cpu.step();
        
    }
}

