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
#include "world.h"
#include "obj.h"
#include "chr.h"
#include "bgmap.h"
#include "palette.h"
#include "framebuffer.h"

namespace CPU
{
    class v810;
}

namespace VIP
{
    class VIP
    {
    public:
        void WriteFrame();
        void DumpCHR();
        void DrawObj(const Obj &obj, int row);
        VIP();
        CPU::v810 *cpu;
        inline char& operator[](const int offset)
        {
            switch (offset)
            {
                case 0x00000 ... 0x05FFF:
                    return *((char*)&leftFrameBuffer[0] + offset);
                case 0x06000 ... 0x07FFF:
                    return *(((char*)&chrRam[0]) + (offset - 0x06000));
                case 0x08000 ... 0x0DFFF:
                    return *((char*)&leftFrameBuffer[1] + (offset - 0x08000));
                case 0x0E000 ... 0x0FFFF:
                    return *(((char*)&chrRam[512]) + (offset - 0x0E000));
                case 0x10000 ... 0x15FFF:
                    return *((char*)&rightFrameBuffer[0] + (offset - 0x10000));
                case 0x16000 ... 0x17FFF:
                    return *(((char*)&chrRam[1024]) + (offset - 0x16000));
                case 0x18000 ... 0x1DFFF:
                    return *((char*)&rightFrameBuffer[1] + (offset - 0x18000));
                case 0x1E000 ... 0x1FFFF:
                    return *(((char*)&chrRam[1536]) + (offset - 0x1E000));
                case 0x20000 ... 0x3BFFF:
                    return *(((char*)&bgMaps[0]) + (offset - 0x20000));
                case 0x3D800 ... 0x3DBFF:
                    return *(((char*)&worlds[0]) + (offset - 0x3D800));
                case 0x3DC00 ... 0x3DFFF:
                    return *(((char*)&columnTable[0]) + (offset - 0x3DC00));
                case 0x3E000 ... 0x3FFFF:
                    return *(((char*)&oam[0]) + (offset - 0x3E000));
                case 0x5F800:
                    return *(char*)&INTPND;
                case 0x5F802:
                    return *(char*)&INTENB;
                case 0x5F804:
                    return *(char*)&INTCLR;
                case 0x5F820:
                    return *(char*)&DPSTTS;
                case 0x5F822:
                    return *(char*)&DPCTRL;
                case 0x5F824:
                    return *(char*)&BRT[0];
                case 0x5F826:
                    return *(char*)&BRT[1];
                case 0x5F828:
                    return *(char*)&BRT[2];
                case 0x5F82A:
                    return *(char*)&REST;
                case 0x5F82E:
                    return *(char*)&FRMCYC;
                case 0x5F830:
                    return *(char*)&CTA;
                case 0x5F840:
                    return *(char*)&XPSTTS;
                case 0x5F842:
                    return *(char*)&XPCTRL;
                case 0x5F844:
                    return *(char*)&VER;
                case 0x5F848:
                    return *(char*)&objControl[0];
                case 0x5F84A:
                    return *(char*)&objControl[1];
                case 0x5F84C:
                    return *(char*)&objControl[2];
                case 0x5F84E:
                    return *(char*)&objControl[3];
                case 0x5F860 ... 0x5F867:
                    return *(((char*)&GPLT[0]) + (offset - 0x5F860));
                case 0x5F868 ... 0x5F86E:
                    return *(((char*)&JPLT[0]) + (offset - 0x5F868));
                case 0x78000 ... 0x7FFFF:
                    return *(((char*)&chrRam[0]) + (offset - 0x78000));
                default:
                  //  printf("Accessing memory: %x\n", offset);
                    return videoRam[offset];
            }
        }
        

        uint16_t Step(uint32_t cycles);
        void Draw(int row);
        Chr chrRam[2048];
        Obj oam[1024];
        BGMap bgMaps[14];
        World worlds[32];
        
        uint32_t bmpData[384 * 256];
        
        Framebuffer leftFrameBuffer[2];
        Framebuffer rightFrameBuffer[2];
        char columnTable[0x400];
        char videoRam[0x80000];
        bool gameStartTriggered;
        uint8_t framesWaited;
        int8_t rowCount;
        int32_t drawingCounter;
        int32_t columnCounter;
        uint32_t lastFrameBuffer;
        char frame;
        int8_t objSearchIndex;

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
        struct
        {
            uint8_t val;
            uint8_t reserved;
        } BRT[3];
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
        
        struct ObjControlVal
        {
            uint16_t SPT : 10;
            uint16_t reserved : 6;
        } objControl[4];

        Palette GPLT[4];
        Palette JPLT[4];
        uint16_t BKCOL;
    };
    
}

#endif
