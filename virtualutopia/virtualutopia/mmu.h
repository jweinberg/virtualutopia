#ifndef virtualutopia_mmu_h
#define virtualutopia_mmu_h
#include "rom.h"

class MMU 
{
public:
    template <typename T>
    class Stream;
    
    MMU(const ROM &rom);

    template<typename T>
    struct GetDataProxy
    {
        const MMU & mmu;
        GetDataProxy<T>(MMU& _mmu) : mmu(_mmu) {}
        
        T& operator()(uint32_t address) const
        {
            T* tp = (T*)(mmu.rawData(address));
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
            T* tp = (T*)(mmu.rawData(address));
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
    const char * rawData(uint32_t virtualAddress) const;
    const ROM &rom;
    char programRam[0xFFFF];
    char registers[0xFF];
    char videoRam[0x7FFFF];
};

#endif
