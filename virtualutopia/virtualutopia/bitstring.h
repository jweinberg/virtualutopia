//
//  bitstring.h
//  virtualutopia
//

#ifndef virtualutopia_bitstring_h
#define virtualutopia_bitstring_h

#include <assert.h>
#include <cmath>
#include "mmu.h"
#include <iostream>
#include <bitset>

namespace CPU
{
    class Bitstring {
    public:
        Bitstring(MMU& _mmu, uint32_t _address, uint8_t _offset, uint32_t _length) : mmu(_mmu), stringLength(_length), currentLocation(_address), offset(_offset)
        {
            int wordsShifted = offset / 32;
            currentLocation += 4 * wordsShifted;
            offset -= wordsShifted * 32;
        }
        
        void Dump()
        {
            for (int i = 3; i >= 0; --i)
                std::cout << std::bitset<32>(mmu.read<uint32_t>(currentLocation + i * 4)) << " ";
            
            std::cout << std::endl << "Offset: " << (int)offset << " Length: " << stringLength << std::endl;
        }
        
        template <typename Functype>
        inline void SetNext(const Functype& op, uint32_t bits, uint8_t length)
        {   
            uint8_t bitsFromWord = min<uint8_t>(length, 32-offset);
            uint32_t currentWord = mmu.read<uint32_t>(currentLocation);
            uint32_t modifiedWord = op(currentWord >> offset, bits);
            currentWord = currentWord & (uint32_t)(~(~(0xFFFFFFFFL << bitsFromWord) << offset));
            currentWord = currentWord | (modifiedWord << offset);
            
            mmu.store(currentWord, currentLocation);
            
            if (bitsFromWord < length)
            {
                uint32_t leftToWrite = length - bitsFromWord;
                currentLocation += 4;
                uint32_t nextWord = mmu.read<uint32_t>(currentLocation);
                uint32_t modifiedWord = op(nextWord & ~(0xFFFFFFFFL << leftToWrite), bits >> bitsFromWord);
                nextWord &= 0xFFFFFFFFL << leftToWrite;
                nextWord |= modifiedWord;
                mmu.store(nextWord, currentLocation);
                
                offset = leftToWrite;
            }
            else
            {
                offset += length;
                if (offset == 32)
                {
                    offset = 0;
                    currentLocation += 4;
                }
            }
            
            stringLength -= length;
        }
        
        
        inline void Read(uint32_t &data, uint8_t &readLength)
        {   
            readLength = min<uint32_t>(32, stringLength);
            
            //Read in the entire current word (cache this?) and remove the data that has already been processed
            uint32_t readWord = mmu.read<uint32_t>(currentLocation) >> offset;

            uint8_t bitsFromWord = 32-offset;
            
            //We need to read a bit from the next word
            if (bitsFromWord < readLength)
            {
                uint32_t leftToRead = readLength - bitsFromWord;
                currentLocation += 4;
                uint32_t nextWord = mmu.read<uint32_t>(currentLocation);
                offset = leftToRead;
                readWord |= nextWord << bitsFromWord;
            }
            else
            {
                offset += readLength;
                if (offset == 32)
                {
                    offset = 0;
                    currentLocation += 4;
                }
            }
            
            
            stringLength -= readLength;
            data = readWord & (0xFFFFFFFFL >> (32 - readLength));
        }
        
        inline bool HasData()
        {
            return stringLength > 0;
        }
        
    private:
        MMU &mmu;
    public:
        uint32_t stringLength;
        uint32_t currentLocation;
        uint8_t offset;
    };
}

#endif
