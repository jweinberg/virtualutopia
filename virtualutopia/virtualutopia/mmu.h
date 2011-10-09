#ifndef virtualutopia_mmu_h
#define virtualutopia_mmu_h
#include "rom.h"

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
        const MMU & mmu;
        GetDataProxy<T>(MMU& _mmu) : mmu(_mmu) {}
        
        T& operator()(uint32_t address) const
        {
            T* tp = (T*)(&mmu[address]);
            if (tp == NULL)
                return *tp;
            return *tp;   
        }        
    };    
    
    template<typename T>
    struct GetDataProxy<T*>
    {
        const MMU& mmu;
        GetDataProxy<T>(MMU& _mmu) : mmu(_mmu) {}
        
        T* operator()(uint32_t address)
        {
            T* tp = (T*)(&mmu[address]);
            return tp;   
        }
    };
    
    template <typename T>
    T& GetData(uint32_t address)
    {
        GetDataProxy<T> p(*this);
        return p(address);
    }
    
    void Store(uint32_t address, uint8_t byte);
    void StoreHWord(uint32_t address, uint16_t hword);
    void StoreWord(uint32_t address, uint32_t word);
    
private:
    const char &operator[](const uint32_t virtualAddress) const;
    const ROM &rom;
    NVC::NVC &nvc;
    VIP::VIP &vip;
    char soundRegisters[0x600];
    char programRam[0xFFFF];
    char *gamepackRam;
};

#endif
