#ifndef virtualutopia_v810_h
#define virtualutopia_v810_h

#include <iostream>
#include <stdint.h>
#include "registers.h"
#include "instruction.h"
#include "cpu_utils.h"
#include "mmu.h"
#include <cmath>
#include "bitstring.h"
#include <vector>

class MMU;
namespace VIP
{
    class VIP;
};


namespace CPU
{
    class v810;
    class Instruction;
 
    
    enum ExceptionCode
    {
        DIV0 = 0xFF80,
    };
    
    enum InterruptCode
    {
        Dunno,
    };
    class v810 
    {
    public:
        v810(MMU& mmu, VIP::VIP& vip);
        void reset();
        void step();
        const std::string registerDescription() const;
        uint32_t cycles;

        bool debugOutput;
        void decode(const Instruction &anInstruction);
        void bitstringDecode(const Instruction &instruction);
        void floatingPointDecode(const Instruction &instruction);
        void throwException(ExceptionCode exceptionCode);
        void processInterrupt(InterruptCode interruptCode);
    private:        
        char *programCounter;
        union
        {
            int32_t generalRegisters[32];
            float generalRegistersFloat[32];
        };
        
        struct
        {
            int32_t EIPC;
            int32_t EIPSW;
            int32_t FEPC;
            int32_t FEPSW;
            REGISTER_BITFIELD (ECR,
                uint16_t EICC;
                uint16_t FECC;
            );
            REGISTER_BITFIELD (PSW,
                uint32_t Z:1; //Zero, result is zero
                uint32_t S:1; //Sign, result is negative
                uint32_t OV:1; //Overflow
                uint32_t CY:1; //Carry
                uint32_t FPR:1; //Floating Precision
                uint32_t FUD:1; //Floating Underflow
                uint32_t FOV:1; //Floating Overflow
                uint32_t FZD:1; //Floating Zero Divide
                uint32_t FIV:1; //Floating Invalid
                uint32_t FRO:1; //Floating Reserved Operand
                uint32_t RESERVED_0:1; //Unused, always zero
                uint32_t RESERVED_10:1; //Unused, always zero
                uint32_t ID:1; //Interrupt disabled, 1-disable, 0-enable
                uint32_t AE:1; //Address Trap Enable
                uint32_t EP:1; //Expection pending, exception, trap or interrupt is being handled
                uint32_t NP:1; //NMI pending, non maskable interrupt is being handled
                uint32_t IntLevel:4; //Maskable interrupt level (0-15)
                uint32_t RESERVED_LAST:12; //Unused, always zero
            );
            REGISTER_BITFIELD(PIR,
                uint32_t NECRV:4;
                uint32_t PT:12;
                uint32_t RESERVED:16;
            );
            int32_t TKCW;
            int32_t RESERVED_SR0[16];
            int32_t CHCW;
            int32_t ADTRE;
            int32_t RESERVED_SR1[6];
            
            int32_t& operator [] (const int index) { return ((int32_t*)this)[index]; }
        }systemRegisters;
        
        MMU &memoryManagmentUnit;
        VIP::VIP &vip;
#pragma mark - Instructions
        #include "v810_instructions.h"
    };
    
}

#endif
