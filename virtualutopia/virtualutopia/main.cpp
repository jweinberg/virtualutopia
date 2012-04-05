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
#include "vb.h"

int main (int argc, const char * argv[])
{

    struct termios ttystate;
    
    //get the terminal state
    tcgetattr(STDIN_FILENO, &ttystate);
    
        //turn off canonical mode
        ttystate.c_lflag &= ~ICANON;
        //minimum of number input read.
        ttystate.c_cc[VMIN] = 1;

    //set the terminal attributes.
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
    
    
    VB vb("/Users/jweinberg/base.vb");
    
    bool wait = true;
    int steps = 0;
    while (1)
    {
        vb.cpu->step();
    }
    {
        if (wait && steps == 0)
        {
            switch (getchar())
            {
                case 'r':
                case 'R':
                    std::cout << vb.cpu->registerDescription() << std::endl;
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
                    vb.cpu->step();
                    break;
            }
        }
        else
        {
            //int key = kbhit();
            
          //  if (key)
            {
//                nvc.SDHR = ~nvc.SDHR;
//                nvc.SDLR = ~nvc.SDLR;
                
                // fgetc(stdin);
//                printf("%d\n", );
                vb.nvc->SDR.SDHR.LR = 1;
//                nvc.SDHR.LD = 1;
             
            }
//            else
//            {
//                nvc.SDLR.LR = 0;
//                nvc.SDHR.RR = 0;                
//            }
            if (steps)
                steps--;
            vb.cpu->step();
        }
        
    }
}

