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
    ROM rom("mariofly.vb");
    VIP::VIP vip;
    MMU mmu(rom, vip);
    
    CPU::v810 cpu(mmu, vip);

    //Lets break the term and have some fun!
	struct sgttyb modmodes;
	ioctl(fileno(stdin), TIOCGETP, &modmodes);
	modmodes.sg_flags |= CBREAK;
	ioctl(fileno(stdin), TIOCSETN, &modmodes);
    
    
    bool wait = true;
    int steps = 0;
    while (1)
    {
        if (wait && steps == 0)
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
                case '0':
                    steps = 1000;
                    break;
                case '9':
                    steps = 500;
                    break;
                case '8':
                    steps = 100;
                    break;
                default:
                    cpu.step();
                    break;
            }
        }
        else
        {
            if (steps)
                steps--;
            cpu.step();
        }
        
    }
}

