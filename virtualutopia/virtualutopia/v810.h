#ifndef virtualutopia_v810_h
#define virtualutopia_v810_h

#include <iostream>
#include <stdint.h>
#include "registers.h"
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
    private:        
        bool debugOutput;
        void decode(const Instruction &anInstruction);
        void bitstringDecode(const Instruction &instruction);
        void floatingPointDecode(const Instruction &instruction);
        void throwException(ExceptionCode exceptionCode);
        void processInterrupt(InterruptCode interruptCode);
        uint32_t programCounter;
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
        void move(const Instruction& anInstruction);
        void add(const Instruction& anInstruction);
        void addImmediate5(const Instruction& anInstruction);
        void subtract(const Instruction& anInstruction);
        void compare(const Instruction& anInstruction);
        void divide(const Instruction& anInstruction);
        void shiftLeft(const Instruction& anInstruction);
        void shiftRight(const Instruction& anInstruction);
        void jump(const Instruction& anInstruction);
        void shiftArithmeticRight(const Instruction& anInstruction);
        void shiftArithmeticRightImmediate(const Instruction& anInstruction);
        void multiply(const Instruction& anInstruction);
        void multiplyUnsigned(const Instruction& anInstruction);
        void divideUnsigned(const Instruction& anInstruction);
        void logicalOr(const Instruction& anInstruction);
        void logicalAnd(const Instruction& anInstruction);
        void logicalXor(const Instruction& anInstruction);
        void logicalNot(const Instruction& anInstruction);
        
        void andImmediate(const Instruction& anInstruction);
        void orImmediate(const Instruction &anInstruction);
        void xorImmediate(const Instruction &anInstruction);
        void moveImmediate(const Instruction& anInstruction);
        void setFlag(const Instruction& anInstruction);
        void compareImmediate(const Instruction& anInstruction);
        void shiftLeftImmediate(const Instruction& anInstruction);
        void shiftRightImmediate(const Instruction& anInstruction);
        
        void moveHigh(const Instruction& anInstruction);
        void moveAddImmediate(const Instruction& anInstruction);
        void addImmediate(const Instruction& anInstruction);
        void storeByte(const Instruction& anInstruction);
        void storeHWord(const Instruction& anInstruction);
        void storeWord(const Instruction& anInstruction);
        void loadWord(const Instruction& anInstruction);
        void loadHWord(const Instruction& anInstruction);
        void loadByte(const Instruction& anInstruction);
        
        void loadSystemRegister(const Instruction& anInstruction);
        void storeSystemRegister(const Instruction& anInstruction);
        
        void branchIfOverflow(const Instruction& anInstruction);
        void branchIfNoOverflow(const Instruction& anInstruction);
        void branchIfCarry(const Instruction& anInstruction);
        void branchIfNoCarry(const Instruction& anInstruction);
        void branchIfZero(const Instruction& anInstruction);
        void branchIfNotZero(const Instruction& instruction);
        void branchIfNotHigher(const Instruction& anInstruction);
        void branchIfNegative(const Instruction& anInstruction);
        void branchIfPositive(const Instruction& anInstruction);
        void branchIfHigher(const Instruction& anInstruction);
        void branchIfLessThan(const Instruction& anInstruction);
        void branchIfGreaterThan(const Instruction& anInstruction);
        void branchIfGreaterOrEqual(const Instruction& instruction);
        void branchIfLessOrEqual(const Instruction& instruction);
        void branch(const Instruction& anInstruction);
        
        void returnFromTrap(const Instruction& anInstruction);
        void jumpRelative(const Instruction& anInstruction);
        void jumpAndLink(const Instruction& anInstruction);
        void nop(const Instruction& anInstruction);
        void outWrite(const Instruction& anInstruction);
        
        void setInterruptDisable(const Instruction &anInstruction);
        void clearInterruptDisable(const Instruction &anInstruction);
        
        void compareFloat(const Instruction& instruction);
        void convertWordToFloat(const Instruction& instruction);
        void convertFloatToWord(const Instruction& instruction);
        void addFloat(const Instruction& instruction);
        void subtractFloat(const Instruction& instruction);
        void multiplyFloat(const Instruction& instruction);
        void divideFloat(const Instruction& instruction);
        void truncateFloat(const Instruction& instruction);
        void reverseWord(const Instruction& instruction);
        void multiplyHalfWord(const Instruction& instruction);
        void exchangeByte(const Instruction& instruction);
        void exchangeHalfWord(const Instruction& instruction);
        void moveBitString(const Instruction& instruction);
        void andBitString(const Instruction& instruction);
        void orBitString(const Instruction& instruction);
        void xorBitString(const Instruction& instruction);
        
        void orNotBitString(const Instruction& instruction);
        void andNotBitString(const Instruction& instruction);
        void xorNotBitString(const Instruction& instruction);
        void notBitString(const Instruction& instruction);
    };
    
}

#endif
