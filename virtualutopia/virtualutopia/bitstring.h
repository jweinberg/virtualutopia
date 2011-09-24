//
//  bitstring.h
//  virtualutopia
//

#ifndef virtualutopia_bitstring_h
#define virtualutopia_bitstring_h

#include <assert.h>
#include <cmath>
#include "mmu.h"
namespace CPU
{
    template <typename T>
    inline T min(T a, T b)
    {
        return (a > b) ? b : a;
    }
    
    class Bitstring {
    public:
        Bitstring(MMU& _mmu, uint32_t _address, uint8_t _offset, uint32_t _length) : mmu(_mmu), currentLocation(_address), offset(_offset), stringLength(_length)
        {
        }
        
        template <typename Functype>
        inline void SetNext(uint32_t bits, uint8_t length)
        {
            const Functype operation;
            
            assert(length > 0 && length <= 32);
            assert(stringLength >= length);
            
            uint32_t currentWord = mmu.GetData<uint32_t>(currentLocation);
            currentWord = operation(currentWord, bits);
            
            
            //How much of THIS word is left to work with after the offsetting
            uint8_t bitsLeft = 32-offset;
            uint32_t mask = 0xFFFFFFFF;
            uint8_t leftover = 0;
            
            if (length < bitsLeft)
                mask = (mask >> (32 - length));
            else
                leftover = length - bitsLeft;
            
            mask = (mask << offset);
            
            currentWord &= ~mask;
            currentWord |= (bits << offset); 
            mmu.StoreWord(currentLocation, currentWord);
            
            if (leftover)
            {
                currentLocation += 4;
                currentWord = mmu.GetData<uint32_t>(currentLocation);
                currentWord = operation(currentWord, bits);
                bits = (bits >> bitsLeft);
                mask = 0xFFFFFFFF << leftover;
                currentWord &= mask;
                currentWord |= bits;
                offset = leftover;
                
                mmu.StoreWord(currentLocation, currentWord);
            }
            else
            {
                offset = (offset + length) % 32;
                if (offset == 0)
                    currentLocation += 4;
            }
            
            stringLength -= length;
        }
        
        inline void Read(uint32_t &data, uint8_t &readLength)
        {
            uint32_t readWord = mmu.GetData<uint32_t>(currentLocation);
            readLength = min<uint32_t>(stringLength, 32);
            uint8_t bitsLeft = 32 - offset;
            uint32_t mask = 0xFFFFFFFF;
            uint32_t leftover = 0;
            
            if (readLength > bitsLeft)
            {
                leftover = readLength - bitsLeft;
                mask = (mask >> offset);
            }
            else
            {
                mask = mask >> (32 - readLength);
            }
            
            readWord = (readWord >> offset);
            
            
            readWord &= mask;
            
            if (leftover)
            {
                currentLocation += 4;
                uint32_t nextWord = mmu.GetData<uint32_t>(currentLocation);
                mask = 0xFFFFFFFF << leftover;
                nextWord &= ~mask;
                nextWord = nextWord << (32 - leftover);
                readWord |= nextWord;
            }
            stringLength -= readLength;
            
            data = readWord;
        }
        
        inline bool HasData()
        {
            return stringLength > 0;
        }
        
    private:
        MMU &mmu;
        uint32_t stringLength;
        uint8_t offset;
        uint32_t currentLocation;
    };
}

#endif
