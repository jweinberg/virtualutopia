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
    class Bitstring {
    public:
        Bitstring(MMU& _mmu, uint32_t _address, uint8_t _offset, uint32_t _length) : mmu(_mmu), currentLocation(_address), offset(_offset), stringLength(_length)
        {
        }
        
        template <typename Functype>
        inline void SetNext(const Functype& op, uint32_t bits, uint8_t length)
        {
            assert(length > 0 && length <= 32);
            assert(stringLength >= length);
            
            uint32_t currentWord = mmu.read<uint32_t>(currentLocation);
            currentWord = op(currentWord, bits);
            
            
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
            mmu.store(currentWord, currentLocation);
            
            if (leftover)
            {
                currentLocation += 4;
                currentWord = mmu.read<uint32_t>(currentLocation);
                currentWord = op(currentWord, bits);
                bits = (bits >> bitsLeft);
                mask = 0xFFFFFFFF << leftover;
                currentWord &= mask;
                currentWord |= bits;
                offset = leftover;
                
                mmu.store(currentWord, currentLocation);
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
            uint32_t readWord = mmu.read<uint32_t>(currentLocation);
            currentLocation += 4;
            
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
                uint32_t nextWord = mmu.read<uint32_t>(currentLocation);
                currentLocation += 4;
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
    public:
        uint32_t stringLength;
        uint8_t offset;
        uint32_t currentLocation;
    };
}

#endif
