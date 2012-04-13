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
        //        void DumpCHR();
        void DrawObj(const Obj &obj, int row);
        void DrawNormalWorld(int row, const World &world);
        void DrawAffineWorld(int row, const World& world);
        void DrawHBiasWorld(int row, const World& world);
        VIP();

        template <typename T>
        inline void store(T& val, const int offset)
        {
            //Invalidate the CHR cache
            switch (offset)
            {
                case 0x06000 ... 0x07FFF:
                    chrCache[(offset - 0x06000) / sizeof(Chr)].valid = false;
                    break;
                case 0x0E000 ... 0x0FFFF:
                    chrCache[(offset - 0x0E000) / sizeof(Chr) + 512].valid = false;
                    break;
                case 0x16000 ... 0x17FFF:
                    chrCache[(offset - 0x16000) / sizeof(Chr) + 1024].valid = false;
                    break;
                case 0x1E000 ... 0x1FFFF:
                    chrCache[(offset - 0x1E000) / sizeof(Chr) + 1536].valid = false;
                    break;
                case 0x78000 ... 0x7FFFF:
                    chrCache[(offset - 0x78000) / sizeof(Chr)].valid = false;
                    break;
            }
            
            if (offset == 0x5F804) //INTCLR
                INTPND &= ~val;
            else if (offset == 0x5F822) //DPCTRL
            {
                _DPCTRL *dp = (_DPCTRL*)&val;
                DPSTTS.LOCK = dp->LOCK;
                DPSTTS.SYNCE = dp->SYNCE;
                DPSTTS.RE = dp->RE;
                DPSTTS.DISP = dp->DISP;
                
                if (dp->DPRST)
                {
                    INTENB.SCANERR = 0;
                    INTENB.LFBEND = 0;
                    INTENB.RFBEND = 0;
                    INTENB.GAMESTART = 0;
                    INTENB.FRAMESTART = 0;
                    INTENB.TIMERR = 0;   
                }
            }
            else if (offset == 0x5F842) //XPCTRL
            {
                _XPCTRL *xp = (_XPCTRL*)&val;
                if (xp->XPRST)
                {
                    INTENB.TIMERR = 0;
                    INTENB.XPEND = 0;
                    INTENB.SBHIT = 0;
                    XPSTTS.XPEN = 0;
                }
                else
                    XPSTTS.XPEN = xp->XPEN;
            }
           
            T *ptr = memoryLookup<T>(offset);
            if (ptr)
                *ptr = val;
        }
        
        template <typename T>
        T* memoryLookup(const int offset) const
        {
            switch (offset)
            {
                case 0x00000 ... 0x05FFF:
                    return (T*)((char*)&leftFrameBuffer[0] + offset);
                case 0x06000 ... 0x07FFF:
                    return (T*)(((char*)&chrRam[0]) + (offset - 0x06000));
                case 0x08000 ... 0x0DFFF:
                    return (T*)((char*)&leftFrameBuffer[1] + (offset - 0x08000));
                case 0x0E000 ... 0x0FFFF:
                    return (T*)(((char*)&chrRam[512]) + (offset - 0x0E000));
                case 0x10000 ... 0x15FFF:
                    return (T*)((char*)&rightFrameBuffer[0] + (offset - 0x10000));
                case 0x16000 ... 0x17FFF:
                    return (T*)(((char*)&chrRam[1024]) + (offset - 0x16000));
                case 0x18000 ... 0x1DFFF:
                    return (T*)((char*)&rightFrameBuffer[1] + (offset - 0x18000));
                case 0x1E000 ... 0x1FFFF:
                    return (T*)(((char*)&chrRam[1536]) + (offset - 0x1E000));
                case 0x20000 ... 0x3BFFF:
                    return (T*)(((char*)&bgMaps[0]) + (offset - 0x20000));
                case 0x3C000 ... 0x3D7FF:
                    return (T*)((char*)&paramTables[0] + (offset - 0x3C000));
                case 0x3D800 ... 0x3DBFF:
                    return (T*)(((char*)&worlds[0]) + (offset - 0x3D800));
                case 0x3DC00 ... 0x3DFFF:
                    return (T*)(((char*)&columnTable[0]) + (offset - 0x3DC00));
                case 0x3E000 ... 0x3FFFF:
                    return (T*)(((char*)&oam[0]) + (offset - 0x3E000));
                case 0x5F800:
                    return (T*)&INTPND;
                case 0x5F802:
                    return (T*)&INTENB;
                case 0x5F804:
                    return (T*)&INTCLR;
                case 0x5F820:
                    return (T*)&DPSTTS;
                case 0x5F822:
                    return (T*)&DPCTRL;
                case 0x5F824:
                    return (T*)&BRT[0];
                case 0x5F826:
                    return (T*)&BRT[1];
                case 0x5F828:
                    return (T*)&BRT[2];
                case 0x5F82A:
                    return (T*)&REST;
                case 0x5F82E:
                    return (T*)&FRMCYC;
                case 0x5F830:
                    return (T*)&CTA;
                case 0x5F840:
                    return (T*)&XPSTTS;
                case 0x5F842:
                    return (T*)&XPCTRL;
                case 0x5F844:
                    return (T*)&VER;
                case 0x5F848:
                    return (T*)&objControl[0];
                case 0x5F84A:
                    return (T*)&objControl[1];
                case 0x5F84C:
                    return (T*)&objControl[2];
                case 0x5F84E:
                    return (T*)&objControl[3];
                case 0x5F860 ... 0x5F867:
                    return (T*)(((char*)&GPLT[0]) + (offset - 0x5F860));
                case 0x5F868 ... 0x5F86E:
                    return (T*)(((char*)&JPLT[0]) + (offset - 0x5F868));
                case 0x5F870:
                    return (T*)(char*)&BKCOL;
                case 0x78000 ... 0x7FFFF:
                    return (T*)(((char*)&chrRam[0]) + (offset - 0x78000));
                default:
                    return NULL;
            }
        }
        

        uint16_t Step(int32_t cycles);
        void Draw(int row);
        
        CPU::v810 *cpu;
        World worlds[32];
        
        Chr chrRam[2048];
        ChrCacheEntry chrCache[2048];
        Obj oam[1024];
        
        uint8_t leftBmpData[384 * 256];
        uint8_t rightBmpData[384 * 256];
        BGMap bgMaps[14];
        char paramTables[0x1800];
        Framebuffer leftFrameBuffer[2];
        Framebuffer rightFrameBuffer[2];

        int32_t drawingCounter;
        int32_t columnCounter;
        int32_t sbOutResetTime;
        
        
        int displayFB;
        int drawingFB;
        
        int displayRegion;
        int drawingRegion;
        int column;
        
        uint32_t lastFrameBuffer;
        bool drawingActive;
        char frame;
        int8_t rowCount;
        int8_t objSearchIndex;
        
        char columnTable[0x400];
        
        bool gameStartTriggered;
        uint8_t framesWaited;

#pragma mark - 4.1 Interrupt Registers
        REGISTER_BITFIELD(uint16_t, INTPND,
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

        REGISTER_BITFIELD(uint16_t, INTENB,
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
        
        REGISTER_BITFIELD(uint16_t, INTCLR,
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

        
        REGISTER_BITFIELD(uint16_t, DPSTTS,
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
        
        REGISTER_BITFIELD(uint16_t, DPCTRL,
            uint16_t DPRST:1;
            uint16_t DISP:1;
            uint16_t padding_0:6;
            uint16_t RE:1;
            uint16_t SYNCE:1;
            uint16_t LOCK:1;
            uint16_t padding_1:5;
        );
        REGISTER_BITFIELD(uint16_t, BRT,
            uint8_t val;
            uint8_t reserved;
        )[3];
        uint16_t REST;
        uint16_t FRMCYC;
        uint16_t CTA;
        
#pragma mark - 4.3 Image Drawing Registers
        REGISTER_BITFIELD(uint16_t, XPSTTS,
            uint16_t padding_0:1;
            uint16_t XPEN:1;
            uint16_t XPBSY:2;
            uint16_t OVERTIME:1;
            uint16_t padding_1:3;
            uint16_t SBCOUNT:5;
            uint16_t padding_2:2;
            uint16_t SBOUT:1;
        );
        REGISTER_BITFIELD(uint16_t, XPCTRL, 
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
    
    struct BGMapLookup
    {
    private:
        VIP& vip;
        const BGMapData *cachedData;
        const BGMapData *overPlaneChar;
        const BGMap * const baseMap;    
        const uint16_t overplaneChrAddress;
        uint16_t maxX;
        uint16_t maxY;
        int16_t x;
        int16_t y;
        uint16_t xChar;
        uint16_t yChar;
        const uint8_t xMaps;
        const uint8_t yMaps;
        const bool over;
        bool requiresReload;
        bool useOverplane;

        uint8_t xMap;
        uint8_t yMap;


    public: 
        BGMapLookup(VIP& _vip, const BGMap * const _baseMap, uint8_t _xMaps, uint8_t _yMaps, bool _over, uint16_t _overAddress) : 
        vip(_vip),
        baseMap(_baseMap), 
        overplaneChrAddress(_overAddress),
        maxX(_xMaps * 512),
        maxY(_yMaps * 512),
        xChar(0),
        yChar(0),
        xMaps(_xMaps), 
        yMaps(_yMaps),
        over(_over),
        xMap(INT_MAX),
        yMap(INT_MAX)
        {
            requiresReload = true;
            if (over)
            {
                overPlaneChar = vip.memoryLookup<BGMapData>(overplaneChrAddress * 2 + 0x20000);
                assert(overPlaneChar);
            }
        }
        
        void SetX(int16_t _x) 
        { 
            if (!over)
            {
                while (_x < 0)
                    _x += maxX;
                x = _x % maxX;
            }
            else 
                x = _x;
            
            uint8_t prevXMap = xMap;
            xMap = x / 512;
            uint16_t prevXChar = xChar;
            xChar = (x & 0x1FF) / 8;
            
            if (prevXMap != xMap || prevXChar != xChar)
                requiresReload = true;
        }
        
        void SetY(int16_t _y)
        {
            if (!over)
            {
                while (_y < 0)
                    _y += maxY;
                y = _y % maxY;
            }
            else
                y = _y;
            
            uint8_t prevYMap = yMap;
            yMap = y / 512;
            uint16_t prevYChar = yChar;
            yChar = (y & 0x1FF) / 8;
            
            if (prevYMap != yMap || prevYChar != yChar)
                requiresReload = true;
        }
        
        const BGMapData& GetMapData()
        {
            if (y < 0 || y >= maxY || x < 0 || x >= maxX)
            {
                return *overPlaneChar;   
            }
            else
            {
                if (requiresReload)
                {
                    const BGMap &map = *(baseMap + (yMap * xMaps) + xMap);
                    cachedData = &map.chars[yChar * 64 + xChar];
                    requiresReload = false;
                }
                return *cachedData;
            }
        }
    };
}

#endif
