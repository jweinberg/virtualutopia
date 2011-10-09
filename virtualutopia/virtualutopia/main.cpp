//
//  main.cpp
//  virtualutopia
//
//  Created by Joshua Weinberg on 9/4/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <sgtty.h>
#include <iostream>
#include <fstream>
#include "v810.h"
#include "rom.h"
#include "mmu.h"
#include "vip.h"
#include "nvc.h"

int kbhit();

int kbhit()
{
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
    select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &fds);
}

int main (int argc, const char * argv[])
{
    ROM rom("mariofly.vb");
    VIP::VIP vip;
    NVC::NVC nvc;
    MMU mmu(rom, vip, nvc);
    
    CPU::v810 cpu(mmu, vip);

    struct termios ttystate;
    
    //get the terminal state
    tcgetattr(STDIN_FILENO, &ttystate);
    
        //turn off canonical mode
        ttystate.c_lflag &= ~ICANON;
        //minimum of number input read.
        ttystate.c_cc[VMIN] = 1;

    //set the terminal attributes.
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
    
    
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
            //int key = kbhit();
            
          //  if (key)
            {
               // fgetc(stdin);
//                printf("%d\n", );
                nvc.SDHR.LR = 1;
                nvc.SDHR.LD = 1;
             
            }
//            else
//            {
//                nvc.SDLR.LR = 0;
//                nvc.SDHR.RR = 0;                
//            }
            if (steps)
                steps--;
            cpu.step();
        }
        
    }
}

