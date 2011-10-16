#ifndef virtualutopia_mmu_h
#define virtualutopia_mmu_h
#include "rom.h"
#include "vip.h"
#include "nvc.h"

namespace VIP
{
    class VIP;
};

namespace NVC {
    class NVC;
};

class MMU 
{
public:
    template <typename T>
    class Stream;
    
    MMU(const ROM &rom, VIP::VIP &vip, NVC::NVC &nvc);

    template <typename T>
    inline const T& read(uint32_t virtualAddress)
    {
        virtualAddress = virtualAddress & 0x07FFFFFF;
        switch (virtualAddress)
        {
            case 0x0 ... 0xFFFFFF: //Display RAM, VIP
                return vip.read<T>(virtualAddress & 0x7FFFF);
            case 0x01000000 ... 0x010005FF: //Sound Memory
                return *((T*)&soundRegisters[virtualAddress & 0x5FF]);
            case 0x02000000 ... 0x0200002C: //Hardware registers
                return nvc.read<T>(virtualAddress);
            case 0x05000000 ... 0x05FFFFFF: //Program RAM (mask with 0xFFFF)
                return *((T*)&programRam[virtualAddress & 0xFFFF]);
            case 0x06000000 ... 0x06FFFFFF: //Cartridge RAM
                return *((T*)&gamepackRam[(virtualAddress & 0xFFFFFF) & 0x1FFFF]);
            case 0x07000000 ... 0x07FFFFFF: //Cartridge ROM
                return rom.read<T>(virtualAddress);
        }
        assert(false);
    }
    
    template <typename T>
    inline void store(T val, uint32_t virtualAddress)
    {
        virtualAddress = virtualAddress & 0x07FFFFFF;
        switch (virtualAddress)
        {
            case 0x0 ... 0xFFFFFF: //Display RAM, VIP
                vip.store(val, virtualAddress & 0x7FFFF);
                break;
            case 0x01000000 ... 0x010005FF: //Sound Memory
                *(T*)&soundRegisters[virtualAddress & 0x5FF] = val;
                break;
            case 0x02000000 ... 0x0200002C: //Hardware registers
                nvc.store(val, virtualAddress);
                break;
            case 0x05000000 ... 0x05FFFFFF: //Program RAM (mask with 0xFFFF)
                *(T*)&programRam[virtualAddress & 0xFFFF] = val;
                break;
            case 0x06000000 ... 0x06FFFFFF: //Cartridge RAM
                *(T*)&gamepackRam[(virtualAddress & 0xFFFFFF) & 0x1FFFF] = val;
                break;
        }
    }
public:
    const ROM &rom;
    NVC::NVC &nvc;
    VIP::VIP &vip;
    char soundRegisters[0x600];
    char *programRam;
    char *gamepackRam;
};

#endif
