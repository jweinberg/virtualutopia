//
//  vip.h
//  virtualutopia
//
//  Created by Joshua Weinberg on 9/28/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef virtualutopia_vip_h
#define virtualutopia_vip_h

#include "registers.h"

namespace VIP
{
    class VIP
    {
    public:
        VIP();
        char& operator[](const int offset);
        uint16_t Step(uint32_t cycles);
        
        private:
        char videoRam[0x7FFFF];
        bool gameStartTriggered;
        uint8_t framesWaited;
        int8_t rowCount;
        uint32_t lastFrameBuffer;
        char frame;
#pragma mark - 4.1 Interrupt Registers
        REGISTER_BITFIELD(INTPND,
            uint16_t SCANERR:1;
            uint16_t LFBEND:1;
            uint16_t RFBEND:1;
            uint16_t GAMESTART:1;
            uint16_t FRAMESTART:1;
            uint16_t padding:8;
            uint16_t SBHIT:1;
            uint16_t XPEND:1;
            uint16_t TIMERR:1;
        );

        REGISTER_BITFIELD(INTENB,
            uint16_t SCANERR:1;
            uint16_t LFBEND:1;
            uint16_t RFBEND:1;
            uint16_t GAMESTART:1;
            uint16_t FRAMESTART:1;
            uint16_t padding:8;
            uint16_t SBHIT:1;
            uint16_t XPEND:1;
            uint16_t TIMERR:1;
        );
        
        REGISTER_BITFIELD(INTCLR,
            uint16_t SCANERR:1;
            uint16_t LFBEND:1;
            uint16_t RFBEND:1;
            uint16_t GAMESTART:1;
            uint16_t FRAMESTART:1;
            uint16_t padding:8;
            uint16_t SBHIT:1;
            uint16_t XPEND:1;
            uint16_t TIMERR:1;
        );
        
#pragma mark - 4.2 Image Display Registers
        REGISTER_BITFIELD(DPSTTS,
            uint16_t padding_0:1;
            uint16_t DISP:1;
            uint16_t DPBSY:4;
            uint16_t SCANRDY:1;
            uint16_t FCLK:1;
            uint16_t RE:1;
            uint16_t SYNCE:1;
            uint16_t LOCK:1;
            uint16_t padding_1:5;
        );
        REGISTER_BITFIELD(DPCTRL,
            uint16_t DPRST:1;
            uint16_t DISP:1;
            uint16_t padding_0:6;
            uint16_t RE:1;
            uint16_t SYNCE:1;
            uint16_t LOCK:1;
            uint16_t padding_1:5;
        );
        uint16_t BRTA;
        uint16_t BRTB;
        uint16_t BRTC;
        uint16_t REST;
        uint16_t FRMCYC;
        uint16_t CTA;
        
#pragma mark - 4.3 Image Drawing Registers
        REGISTER_BITFIELD(XPSTTS,
            uint16_t padding_0:1;
            uint16_t XPEN:1;
            uint16_t XPBSY:2;
            uint16_t OVERTIME:1;
            uint16_t padding_1:3;
            uint16_t SBCOUNT:5;
            uint16_t padding_2:2;
            uint16_t SBOUT:1;
        );
        REGISTER_BITFIELD(XPCTRL, 
            uint16_t XPRST:1;
            uint16_t XPEN:1;
            uint16_t padding_0:6;
            uint16_t SBCMP:5;
            uint16_t padding_1:3;
        );
        uint16_t VER;
        uint16_t SPT0;
        uint16_t SPT1;
        uint16_t SPT2;
        uint16_t SPT3;
        uint16_t GPLT0;
        uint16_t GPLT1;
        uint16_t GPLT2;
        uint16_t GPLT3;
        uint16_t JPLT0;
        uint16_t JPLT1;
        uint16_t JPLT2;
        uint16_t JPLT3;
        uint16_t BKCOL;
    };
    
}

#endif
