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

    template<typename T>
    struct GetDataProxy
    {
        MMU& mmu;
        GetDataProxy<T>(MMU& _mmu) : mmu(_mmu) {}
        
        const T& operator()(uint32_t address) const
        {
            return *(T*)(&mmu[address]);
        }        
    };    
    
    template<typename T>
    struct GetDataProxy<T*>
    {
        MMU& mmu;
        GetDataProxy<T*>(MMU& _mmu) : mmu(_mmu) {}
        
        const T* operator()(uint32_t address) const
        {
            return (T*)(&mmu[address]);
        }
    };
    
    template <typename T>
    inline T& GetData(uint32_t address)
    {
        return *(T*)(&(*this)[address]);
//        const GetDataProxy<T> p(*this);
//        return p(address);
    }
    
    void Store(uint32_t address, uint8_t byte);
    void StoreHWord(uint32_t address, uint16_t hword);
    void StoreWord(uint32_t address, uint32_t word);
    
private:
    inline char &operator[](uint32_t virtualAddress)
    {
        virtualAddress = virtualAddress & 0x07FFFFFF;
        switch (virtualAddress)
        {
            case 0x0 ... 0xFFFFFF: //Display RAM, VIP
                return vip[virtualAddress & 0x7FFFF];
            case 0x01000000 ... 0x010005FF: //Sound Memory
                return soundRegisters[virtualAddress & 0x5FF];
            case 0x02000000 ... 0x0200002C: //Hardware registers
                return nvc[virtualAddress];
            case 0x05000000 ... 0x0500FFFF: //Program RAM (mask with 0xFFFF)
                return programRam[virtualAddress & 0xFFFF];
            case 0x06000000 ... 0x06FFFFFF: //Cartridge RAM
                return gamepackRam[(virtualAddress & 0xFFFFFF) & 0x1FFFF];
            case 0x07000000 ... 0x07FFFFFF: //Cartridge ROM
                return rom[virtualAddress];
        }
        //assert(false);
        return vip[0];
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
