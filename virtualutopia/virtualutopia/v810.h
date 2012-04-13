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
#include <fstream>

extern std::ofstream outFile;
extern volatile bool debugOutput;


#define VIRTUAL_PC 0

#define d_printf(A, ...)
//#if VIRTUAL_PC
//#define d_printf(A, ...) do{ if(debugOutput) {\
//char buffer[255];\
//sprintf(buffer, "%X (%u): " A, programCounter, cycles , ##__VA_ARGS__);\
//outFile << buffer;\
//outFile.flush();\
//} }while(0)
//#else
//#define d_printf(A, ...) do{ if(debugOutput) {\
//char buffer[255];\
//sprintf(buffer, "%X (%u): " A, (0x07000000 + (uint32_t)((char*)programCounter - ((char*)memoryManagmentUnit.rom.data))),cycles , ##__VA_ARGS__);\
//outFile << buffer;\
//outFile.flush();\
//} }while(0)
//#endif


class MMU;
namespace VIP
{
    class VIP;
}

namespace VSU
{
    class VSU;
}


namespace CPU
{
    class v810;
    class Instruction;
    
    
    enum ExceptionCode
    {
        DIV0 = 0xFF80
    };
    
    enum InterruptCode
    {
        INTKEY,
        INTTIM,
        INTCRO,
        INTCOM,
        INTVPU
    };
    
    class v810 
    {
    public:
        v810(MMU& mmu, VIP::VIP& vip, NVC::NVC& nvc, VSU::VSU& vsu);
        void reset();
        void step();
        const std::string registerDescription() const;

        void fetchAndDecode();
        void bitstringDecode(const Instruction &instruction);
        void floatingPointDecode(const Instruction &instruction);
        void throwException(ExceptionCode exceptionCode);
        void processInterrupt(InterruptCode interruptCode);
    private: 
        MMU &memoryManagmentUnit;
        VIP::VIP &vip;
        NVC::NVC &nvc;
        VSU::VSU &vsu;
        
#if VIRTUAL_PC
        uint32_t programCounter;
#else
        const char *programCounter;
#endif
    public:
        uint32_t cycles;
    private:
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
            REGISTER_BITFIELD (uint32_t, ECR,
                uint16_t EICC;
                uint16_t FECC;
            );
            REGISTER_BITFIELD (uint32_t, PSW,
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
            REGISTER_BITFIELD(uint32_t, PIR,
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
        
#pragma mark - Instructions
        void move(uint8_t reg1, uint8_t reg2);
        void moveImmediate(uint8_t imm5, uint8_t reg2);
        void add(uint8_t reg1, uint8_t reg2);
        void addImmediate5(uint8_t imm5, uint8_t reg2);
        void addImmediate(uint8_t reg1, uint8_t reg2, int16_t imm16);
        void subtract(uint8_t reg1, uint8_t reg2);
        void divide(uint8_t reg1, uint8_t reg2);
        void divideUnsigned(uint8_t reg1, uint8_t reg2);
        void multiply(uint8_t reg1, uint8_t reg2);
        void multiplyUnsigned(uint8_t reg1, uint8_t reg2);
        void compare(uint8_t reg1, uint8_t reg2);
        void compareImmediate(uint8_t imm5, uint8_t reg2);
        void setFlag(uint8_t imm5, uint8_t reg2);
        void shiftLeft(uint8_t reg1, uint8_t reg2);
        void shiftLeftImmediate(uint8_t imm5, uint8_t reg2);
        void shiftRight(uint8_t reg1, uint8_t reg2);
        void shiftRightImmediate(uint8_t imm5, uint8_t reg2);
        void shiftArithmeticRight(uint8_t reg1, uint8_t reg2);
        void shiftArithmeticRightImmediate(uint8_t imm5, uint8_t reg2);
        void logicalOr(uint8_t reg1, uint8_t reg2);
        void logicalXor(uint8_t reg1, uint8_t reg2);
        void logicalAnd(uint8_t reg1, uint8_t reg2);
        void logicalNot(uint8_t reg1, uint8_t reg2);
        void andImmediate(uint8_t reg1, uint8_t reg2, uint16_t imm16);
        void orImmediate(uint8_t reg1, uint8_t reg2, uint16_t imm16);
        void xorImmediate(uint8_t reg1, uint8_t reg2, uint16_t imm16);
        void jump(uint8_t reg1, uint8_t unused);
        void jumpRelative(uint32_t disp26);
        void branchIfNoOverflow(uint16_t disp9);
        void branchIfOverflow(uint16_t disp9);
        void branchIfZero(uint16_t disp9);
        void branchIfNotZero(uint16_t disp9);
        void branchIfLessThan(uint16_t disp9);
        void branchIfNotHigher(uint16_t disp9);
        void branchIfNegative(uint16_t disp9);
        void branchIfPositive(uint16_t disp9);
        void branchIfHigher(uint16_t disp9);
        void branchIfGreaterOrEqual(uint16_t disp9);
        void branchIfGreaterThan(uint16_t disp9);
        void branchIfLessOrEqual(uint16_t disp9);
        void nop(uint16_t disp9);
        void branchIfCarry(uint16_t disp9);
        void branchIfNoCarry(uint16_t disp9);
        void branch(uint16_t disp9);
        void moveHigh(uint8_t reg1, uint8_t reg2, uint16_t imm16);
        void moveAddImmediate(uint8_t reg1, uint8_t reg2, uint16_t imm16);
        void storeWord(uint8_t reg1, uint8_t reg2, uint16_t disp16);
        void storeHWord(uint8_t reg1, uint8_t reg2, uint16_t disp16);
        void storeByte(uint8_t reg1, uint8_t reg2, uint16_t disp16);
        void outWord(uint8_t reg1, uint8_t reg2, uint16_t disp16);
        void outHWord(uint8_t reg1, uint8_t reg2, uint16_t disp16);
        void outByte(uint8_t reg1, uint8_t reg2, uint16_t disp16);
        void loadByte(uint8_t reg1, uint8_t reg2, uint16_t disp16);
        void loadHWord(uint8_t reg1, uint8_t reg2, uint16_t disp16);
        void loadWord(uint8_t reg1, uint8_t reg2, uint16_t disp16);
        void inByte(uint8_t reg1, uint8_t reg2, uint16_t disp16);
        void inHWord(uint8_t reg1, uint8_t reg2, uint16_t disp16);
        void inWord(uint8_t reg1, uint8_t reg2, uint16_t disp16);
        void loadSystemRegister(uint8_t reg1, uint8_t reg2);
        void storeSystemRegister(uint8_t reg1, uint8_t reg2);
        void jumpAndLink(uint32_t disp26);
        void outWrite(uint8_t reg1, uint8_t reg2, uint16_t disp16);
        void setInterruptDisable(uint8_t reg1, uint8_t reg2);
        void clearInterruptDisable(uint8_t reg1, uint8_t reg2);
        void returnFromTrap(uint8_t imm5, uint8_t reg2);
        void compareFloat(uint8_t reg1, uint8_t reg2);
        void convertWordToFloat(uint8_t reg1, uint8_t reg2);
        void convertFloatToWord(uint8_t reg1, uint8_t reg2);
        void addFloat(uint8_t reg1, uint8_t reg2);
        void subtractFloat(uint8_t reg1, uint8_t reg2);
        void multiplyFloat(uint8_t reg1, uint8_t reg2);
        void divideFloat(uint8_t reg1, uint8_t reg2);
        void truncateFloat(uint8_t reg1, uint8_t reg2);
        void exchangeByte(uint8_t reg1, uint8_t reg2);
        void exchangeHalfWord(uint8_t reg1, uint8_t reg2);
        void reverseWord(uint8_t reg1, uint8_t reg2);
        void multiplyHalfWord(uint8_t reg1, uint8_t reg2);
        void moveBitString(uint8_t reg1, uint8_t reg2);
        void andBitString(uint8_t reg1, uint8_t reg2);
        void orBitString(uint8_t reg1, uint8_t reg2);
        void xorBitString(uint8_t reg1, uint8_t reg2);
        void andNotBitString(uint8_t reg1, uint8_t reg2);
        void orNotBitString(uint8_t reg1, uint8_t reg2);
        void xorNotBitString(uint8_t reg1, uint8_t reg2);
        void notBitString(uint8_t reg1, uint8_t reg2);

//        #include "v810_instructions.h"
    };
    
}

#endif
