//
//  v810_instructions.cpp
//  virtualutopia
//

#include "v810.h"
#include <deque>
#include <fstream>
//#include "instruction.h"
//#include "cpu_utils.h"
//#include "mmu.h"
//#include "bitstring.h"
//#include <cmath>

#ifndef v810_instructions_guard
#define v810_instructions_guard

//#undef d_printf
//#define d_printf(A, ...) 

struct OrNoter
{
    OrNoter() {}
    uint32_t operator()(uint32_t a, uint32_t b) const { return a | (~b); };
};

struct XorNoter
{
    XorNoter() {}
    uint32_t operator()(uint32_t a, uint32_t b) const { return a | (~b); };
};

struct Noter
{
    Noter() {}
    uint32_t operator()(uint32_t a, uint32_t b) const { return ~b; };
};

struct Mover
{
    Mover() {}
    uint32_t operator()(uint32_t a, uint32_t b) const { return b; };
};

struct Xorer
{
    Xorer() {}
    uint32_t operator()(uint32_t a, uint32_t b) const { return a ^ b; };
};

struct AndNoter
{
    AndNoter() {}
    uint32_t operator()(uint32_t a, uint32_t b) const { return a & (~b); };
};

struct Ander
{
    Ander() {}
    uint32_t operator()(uint32_t a, uint32_t b) const { return a & b; };
};

struct Orer
{
    Orer() {}
    uint32_t operator()(uint32_t a, uint32_t b) const { return a | b; };
};

enum FlagCondition
{
    kFlagOverflow = 0x0, //V
    kFlagNoOverflow = 0x8, //NV
    kFlagCarry = 0x1, //C
    kFlagNoCarry = 0x9, //NC
    kFlagZero = 0x2, //Z
    kFlagNotZero = 0xA, //NZ
    kFlagNotHigher = 0x3, //NH
    kFlagHigher = 0xB, //H
    kFlagPositive = 0x4, //S/P
    kFlagNegative = 0xC, //NS/N
    kFlagTrue = 0x5, //T
    kFlagFalse = 0xD, //F
    kFlagLessThan = 0x6, //LT
    kFlagGreaterOrEqual = 0xE, //GE
    kFlagLessOrEqual = 0x7, //LE
    kFlagGreaterThan = 0xF, //GT
}; 

//Move Form I
void CPU::v810::move(uint8_t reg1, uint8_t reg2)
{
    d_printf("MOV: GR[%d] <- GR[%d](0x%X)\n", reg2, reg1, generalRegisters[reg1]);
    generalRegisters[reg2] = generalRegisters[reg1];
    programCounter += 2;
    cycles += 1;
}

//Move Form II
void CPU::v810::moveImmediate(uint8_t imm5, uint8_t reg2)
{
    d_printf("MOV: GR[%d] <- (0x%X)\n", reg2, sign_extend<5>(imm5));
    generalRegisters[reg2] = sign_extend<5>(imm5);
    programCounter += 2;
    cycles += 1;
}

//Add Form I
void CPU::v810::add(uint8_t reg1, uint8_t reg2)
{
    d_printf("ADD: GR[%d] <- GR[%d](0x%X) + GR[%d](0x%X)\n", reg2, reg2, generalRegisters[reg2], reg1, generalRegisters[reg1]);
    int32_t a = generalRegisters[reg1];
    int32_t b = generalRegisters[reg2];
    int64_t r = a+b;
    int32_t truncatedResult = r & 0xFFFFFFFF;
    generalRegisters[reg2] = truncatedResult;           
    
    systemRegisters.PSW.OV = (sign(a) == sign(b)) ? (sign(truncatedResult) != sign(a)) : 0;
    systemRegisters.PSW.CY = r < b;
    systemRegisters.PSW.S = truncatedResult < 0;
    systemRegisters.PSW.Z = truncatedResult == 0;

    programCounter += 2;
    cycles += 1;
}

//Add Form II
void CPU::v810::addImmediate5(uint8_t imm5, uint8_t reg2)
{
    d_printf("ADD: GR[%d] <- GR[%d](0x%X) + 0x%X\n", reg2, reg2, generalRegisters[reg2], sign_extend<5>(imm5));
    int32_t a = sign_extend<5>(imm5);
    int32_t b = generalRegisters[reg2];
    int64_t r = a+b;
    int32_t truncatedResult = r & 0xFFFFFFFF;
    generalRegisters[reg2] = truncatedResult;           
    
    systemRegisters.PSW.OV = (sign(a) == sign(b)) ? (sign(truncatedResult) != sign(a)) : 0;
    systemRegisters.PSW.CY = r < b;
    systemRegisters.PSW.S = truncatedResult < 0;
    systemRegisters.PSW.Z = truncatedResult == 0;
    
    programCounter += 2;
    cycles += 1;
}


void CPU::v810::addImmediate(uint8_t reg1, uint8_t reg2, int16_t imm16)
{
    d_printf("ADDI: GR[%d] <- GR[%d](0x%X) + 0x%X\n", reg2, reg1, generalRegisters[reg1], sign_extend<16>(imm16));
    
    int32_t a = sign_extend<16>(imm16);
    int32_t b = generalRegisters[reg1];
    int64_t r = a+b;
    int32_t truncatedResult = r & 0xFFFFFFFF;
    generalRegisters[reg2] = truncatedResult;           
    
    systemRegisters.PSW.OV = (sign(a) == sign(b)) ? (sign(truncatedResult) != sign(a)) : 0;
    systemRegisters.PSW.CY = r < a;
    systemRegisters.PSW.S = truncatedResult < 0;
    systemRegisters.PSW.Z = truncatedResult == 0;
    programCounter += 4;
    cycles += 1;
}


//Subtract
void CPU::v810::subtract(uint8_t reg1, uint8_t reg2)
{
    d_printf("SUB: GR[%d] <- GR[%d](0x%X) - GR[%d](0x%X)\n", reg2, reg2, generalRegisters[reg2], reg1, generalRegisters[reg1]);
    int32_t a = generalRegisters[reg1];
    int32_t b = generalRegisters[reg2];
    uint32_t r = b-a;
    generalRegisters[reg2] = (int32_t)r;           
    
    systemRegisters.PSW.OV = calculate_overflow_subtract(a, b);
    systemRegisters.PSW.CY = r > (uint32_t)b;
    systemRegisters.PSW.S = (int32_t)r < 0;
    systemRegisters.PSW.Z = (int32_t)r == 0;
    
    programCounter += 2;
    cycles += 1;
}

void CPU::v810::divide(uint8_t reg1, uint8_t reg2)
{
    d_printf("DIV: GR[%d] <- GR[%d](0x%X) / GR[%d](0x%X)\n", reg2, reg2, generalRegisters[reg2], reg1, generalRegisters[reg1]);
    int32_t a = generalRegisters[reg1];
    int32_t b = generalRegisters[reg2];
    
    if (a == 0)
    {
        throwException(DIV0);
        return;
    }
    
    int32_t result = b / a;
    
    generalRegisters[30] = b % a;
    generalRegisters[reg2] = result;
    
    systemRegisters.PSW.OV = ((uint32_t)b == 0x80000000 && (uint32_t)a == 0xFFFFFFFF) ? 1 : 0;
    systemRegisters.PSW.S = (result < 0) ? 1 : 0;
    systemRegisters.PSW.Z = (result == 0) ? 1 : 0;
    
    programCounter += 2;
    cycles += 38;
}

void CPU::v810::divideUnsigned(uint8_t reg1, uint8_t reg2)
{
    d_printf("DIVU: GR[%d] <- GR[%d](0x%X) / GR[%d](0x%X)\n", reg2, reg2, generalRegisters[reg2], reg1, generalRegisters[reg1]);
    uint32_t a = generalRegisters[reg1];
    int32_t b = generalRegisters[reg2];
    
    if (a == 0)
    {
        throwException(DIV0);
        return;
    }
    
    int32_t result = b / a;
    
    generalRegisters[30] = b % a;
    generalRegisters[reg2] = result;
    
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.S = (result < 0) ? 1 : 0;
    systemRegisters.PSW.Z = (result == 0) ? 1 : 0;
    
    programCounter += 2;
    cycles += 36;
}

void CPU::v810::multiply(uint8_t reg1, uint8_t reg2)
{
    d_printf("MUL: GR[%d] <- GR[%d](0x%X) * GR[%d](0x%X)\n", reg2, reg2, generalRegisters[reg2], reg1, generalRegisters[reg1]);
    int32_t a = generalRegisters[reg1];
    int32_t b = generalRegisters[reg2];
    int64_t result = (int64_t)a * b;
    
    generalRegisters[30] = (int32_t)(result >> 32);
    generalRegisters[reg2] = (int32_t)result;
    
    systemRegisters.PSW.OV = (generalRegisters[30] == 0) ? 0 : 1;
    systemRegisters.PSW.S = (generalRegisters[reg2] < 0) ? 1 : 0;
    systemRegisters.PSW.Z = (generalRegisters[reg2] == 0) ? 1 : 0;
    
    programCounter += 2;
    cycles += 13;
}

void CPU::v810::multiplyUnsigned(uint8_t reg1, uint8_t reg2)
{
    d_printf("MULU: GR[%d] <- GR[%d](0x%X) * GR[%d](0x%X)\n", reg2, reg2, generalRegisters[reg2], reg1, generalRegisters[reg1]);
    uint32_t a = generalRegisters[reg1];
    uint32_t b = generalRegisters[reg2];
    uint64_t result = (uint64_t)a * b;
    
    generalRegisters[30] = (int32_t)(result >> 32);
    generalRegisters[reg2] = (int32_t)(result & 0xFFFFFFFF);
    
    systemRegisters.PSW.OV = (generalRegisters[30] == 0) ? 0 : 1;
    systemRegisters.PSW.S = (generalRegisters[reg2] < 0) ? 1 : 0;
    systemRegisters.PSW.Z = (generalRegisters[reg2] == 0) ? 1 : 0;
    
    programCounter += 2;
    cycles += 13;
}

//Compare Form I
void CPU::v810::compare(uint8_t reg1, uint8_t reg2)
{
    uint32_t a = generalRegisters[reg1];
    uint32_t b = generalRegisters[reg2];
    
    d_printf("CMP: GR[%d](0x%X) and GR[%d](0x%X)\n", reg1, a, reg2, b);
    uint32_t result = b-a;

    
    systemRegisters.PSW.Z = (result == 0) ? 1 : 0;
    systemRegisters.PSW.S = ((int32_t)result < 0) ? 1 : 0;
    systemRegisters.PSW.OV = calculate_overflow_subtract(a, b);
    systemRegisters.PSW.CY = (result > b);
    programCounter += 2;
    cycles += 1;
}

//Compare Form II
void CPU::v810::compareImmediate(uint8_t imm5, uint8_t reg2)
{
    int32_t a = sign_extend<5>(imm5);
    int32_t b = generalRegisters[reg2];
    
    d_printf("CMP: 0x%X and GR[%d](0x%X)\n", a, reg2, b);
    uint32_t result = b-a;
    
    
    systemRegisters.PSW.Z = (result == 0) ? 1 : 0;
    systemRegisters.PSW.S = ((int32_t)result < 0) ? 1 : 0;
    systemRegisters.PSW.OV = calculate_overflow_subtract((int32_t)a, (int32_t)b);
    systemRegisters.PSW.CY = (result > (uint32_t)b);
    programCounter += 2;

    cycles += 1;
}

//Set Flag Condition
void CPU::v810::setFlag(uint8_t imm5, uint8_t reg2)
{
    d_printf("SETF: GR[%d] <- Condition (0x%X)\n", reg2, imm5);
    bool conditionMet = false;
    switch ((FlagCondition)imm5) {
        case kFlagOverflow:
            conditionMet = (systemRegisters.PSW.OV == 1);
            break;
        case kFlagNoOverflow:
            conditionMet = (systemRegisters.PSW.OV == 0);
            break;
        case kFlagCarry:
            conditionMet = (systemRegisters.PSW.CY == 1);
            break;
        case kFlagNoCarry:
            conditionMet = (systemRegisters.PSW.CY == 0);
            break;
        case kFlagZero:
            conditionMet = (systemRegisters.PSW.Z == 1);
            break;
        case kFlagNotZero:
            conditionMet = (systemRegisters.PSW.Z == 0);
            break;
        case kFlagNotHigher:
            conditionMet = ((systemRegisters.PSW.CY | systemRegisters.PSW.Z) == 1);
            break;
        case kFlagHigher:
            conditionMet = ((systemRegisters.PSW.CY | systemRegisters.PSW.Z) == 0);
            break;
        case kFlagPositive:
            conditionMet = (systemRegisters.PSW.S == 1);
            break;
        case kFlagNegative:
            conditionMet = (systemRegisters.PSW.S == 0);
            break;
        case kFlagTrue:
            conditionMet = true;
            break;
        case kFlagFalse:
            conditionMet = false;
            break;
        case kFlagLessThan:
            conditionMet = ((systemRegisters.PSW.S ^ systemRegisters.PSW.OV) == 1);
            break;
        case kFlagGreaterOrEqual:
            conditionMet = ((systemRegisters.PSW.S ^ systemRegisters.PSW.OV) == 0);
            break;
        case kFlagLessOrEqual:
            conditionMet = (((systemRegisters.PSW.S ^ systemRegisters.PSW.OV) | systemRegisters.PSW.Z) == 1);
            break;
        case kFlagGreaterThan:
            conditionMet = (((systemRegisters.PSW.S ^ systemRegisters.PSW.OV) | systemRegisters.PSW.Z) == 0);
            break;
    }
    
    generalRegisters[reg2] = conditionMet;
    programCounter += 2;
    cycles += 1;
}

//Shift Left Form I
void CPU::v810::shiftLeft(uint8_t reg1, uint8_t reg2)
{
    d_printf("SHL: GR[%d] <- GR[%d](0x%X) << GR[%d](0x%X)\n", reg2, reg2, generalRegisters[reg2], reg1, generalRegisters[reg1]);
    uint32_t s = generalRegisters[reg1] & 0x1F;
    int32_t a = generalRegisters[reg2];
    int32_t result = a << s;
    generalRegisters[reg2] = result;
    
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.CY = s ? (a >> (32-s)) & 1 : 0;
    systemRegisters.PSW.S = result < 0;
    systemRegisters.PSW.Z = result == 0;
    programCounter += 2;
    cycles += 1;
}

//Shift Left Form II
void CPU::v810::shiftLeftImmediate(uint8_t imm5, uint8_t reg2)
{
    d_printf("SHL: GR[%d] <- GR[%d](0x%X) << 0x%X\n", reg2, reg2, generalRegisters[reg2], imm5);
    int32_t s = imm5;
    int32_t a = generalRegisters[reg2];
    int32_t result = a << s;
    generalRegisters[reg2] = result;
    
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.CY = s ? (a >> (32-s)) & 1 : 0;
    systemRegisters.PSW.S = result < 0;
    systemRegisters.PSW.Z = result == 0;
    programCounter += 2;
    cycles += 1;
}

//Shift Right Form I
void CPU::v810::shiftRight(uint8_t reg1, uint8_t reg2)
{
    d_printf("SHR: GR[%d] <- GR[%d](0x%X) >> GR[%d](0x%X)\n", reg2, reg2, generalRegisters[reg2], reg1, generalRegisters[reg1]);
    uint32_t s = generalRegisters[reg1] & 0x1F;
    uint32_t a = generalRegisters[reg2];
    int32_t result = a >> s;
    generalRegisters[reg2] = result;
    
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.CY = s ? (a >> (s-1)) & 1 : 0;
    systemRegisters.PSW.S = result < 0;
    systemRegisters.PSW.Z = result == 0;
    programCounter += 2;
    cycles += 1;
}

//Shift Right Form II
void CPU::v810::shiftRightImmediate(uint8_t imm5, uint8_t reg2)
{
    d_printf("SHR: GR[%d] <- GR[%d](0x%X) >> 0x%X\n", reg2, reg2, generalRegisters[reg2], imm5);
    uint32_t s = imm5;
    uint32_t a = generalRegisters[reg2];
    int32_t result = a >> s;
    generalRegisters[reg2] = result;
    
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.CY = imm5 ? (a >> (s-1)) & 1 : 0;
    systemRegisters.PSW.S = result < 0;
    systemRegisters.PSW.Z = result == 0;
    programCounter += 2;
    cycles += 1;
}

//Shift Arithmetic Right (signed shift)
void CPU::v810::shiftArithmeticRight(uint8_t reg1, uint8_t reg2)
{
    d_printf("SAR: GR[%d] <- GR[%d](0x%X) >>>  GR[%d](0x%X)\n", reg2, reg2, generalRegisters[reg2], reg1, generalRegisters[reg1]);
    uint32_t s = generalRegisters[reg1] & 0x1F;
    int32_t a = generalRegisters[reg2];
    int32_t result = a >> s;
    generalRegisters[reg2] = result;
    
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.CY = s ? (a >> (s-1)) & 1 : 0;
    systemRegisters.PSW.S = result < 0;
    systemRegisters.PSW.Z = result == 0;
    programCounter += 2;
    cycles += 1;
}

void CPU::v810::shiftArithmeticRightImmediate(uint8_t imm5, uint8_t reg2)
{
    d_printf("SAR: GR[%d] <- GR[%d](0x%X) >>> 0x%X\n", reg2, reg2, generalRegisters[reg2], imm5);
    uint32_t s = imm5;
    int32_t a = generalRegisters[reg2];
    int32_t result = a >> s;
    generalRegisters[reg2] = result;
    
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.CY = imm5 ? (a >> (s-1)) & 1 : 0;
    systemRegisters.PSW.S = result < 0;
    systemRegisters.PSW.Z = result == 0;
    programCounter += 2;  
    cycles += 1;
}

void CPU::v810::logicalOr(uint8_t reg1, uint8_t reg2)
{
    d_printf("OR: GR[%d] <- GR[%d](0x%X) | GR[%d](0x%X)\n", reg2, reg2, generalRegisters[reg2], reg1, generalRegisters[reg1]);
    int32_t result = generalRegisters[reg2] | generalRegisters[reg1];
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.S = (result < 0) ? 1 : 0;
    systemRegisters.PSW.Z = (result == 0) ? 1 : 0;
    generalRegisters[reg2] = result;
    programCounter += 2;
    cycles += 1;
}

void CPU::v810::logicalXor(uint8_t reg1, uint8_t reg2)
{
    d_printf("XOR: GR[%d] <- GR[%d](0x%X) ^ GR[%d](0x%X)\n", reg2, reg2, generalRegisters[reg2], reg1, generalRegisters[reg1]);
    int32_t result = generalRegisters[reg2] ^ generalRegisters[reg1];
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.S = (result < 0) ? 1 : 0;
    systemRegisters.PSW.Z = (result == 0) ? 1 : 0;
    generalRegisters[reg2] = result;
    programCounter += 2;
    cycles += 1;
}

void CPU::v810::logicalAnd(uint8_t reg1, uint8_t reg2)
{
    d_printf("AND: GR[%d] <- GR[%d](0x%X) & GR[%d](0x%X)\n", reg2, reg2, generalRegisters[reg2], reg1, generalRegisters[reg1]);
    int32_t result = generalRegisters[reg2] & generalRegisters[reg1];
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.S = (result < 0) ? 1 : 0;
    systemRegisters.PSW.Z = (result == 0) ? 1 : 0;
    generalRegisters[reg2] = result;
    programCounter += 2;
    cycles += 1;
}

void CPU::v810::logicalNot(uint8_t reg1, uint8_t reg2)
{
    d_printf("NOT: GR[%d] <- ~GR[%d](0x%X)\n", reg2, reg1, generalRegisters[reg1]);
    int32_t result = ~generalRegisters[reg1];
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.S = (result < 0) ? 1 : 0;
    systemRegisters.PSW.Z = (result == 0) ? 1 : 0;
    generalRegisters[reg2] = result;
    programCounter += 2;
    cycles += 1;
}

void CPU::v810::andImmediate(uint8_t reg1, uint8_t reg2, uint16_t imm16)
{
    d_printf("ANDI: GR[%d] <- GR[%d](0x%X) & 0x%X\n", reg2, reg1, generalRegisters[reg1], imm16);
    uint32_t res = generalRegisters[reg1] & imm16;
    generalRegisters[reg2] = res;
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.S = 0;
    systemRegisters.PSW.Z = (res == 0) ? 1 : 0;
    
    programCounter += 4;
    cycles += 1;
}

void CPU::v810::orImmediate(uint8_t reg1, uint8_t reg2, uint16_t imm16)
{
    d_printf("ORI: GR[%d] <- GR[%d](0x%X) | 0x%X\n", reg2, reg1, generalRegisters[reg1], imm16);
    generalRegisters[reg2] = generalRegisters[reg1] | imm16;
    
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.S = generalRegisters[reg2] < 0;
    systemRegisters.PSW.Z = (generalRegisters[reg2] == 0) ? 1 : 0;
    
    programCounter += 4;
    cycles += 1;
}

void CPU::v810::xorImmediate(uint8_t reg1, uint8_t reg2, uint16_t imm16)
{
    d_printf("XORI: GR[%d] <- GR[%d](0x%X) ^ 0x%X\n", reg2, reg1, generalRegisters[reg1], imm16);
    generalRegisters[reg2] = generalRegisters[reg1] ^ imm16;
    
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.S = generalRegisters[reg2] < 0;
    systemRegisters.PSW.Z = (generalRegisters[reg2] == 0) ? 1 : 0;
    
    programCounter += 4;
    cycles += 1;
}

void CPU::v810::jump(uint8_t reg1, uint8_t unused)
{
    d_printf("JMP: PC <- GR[%d](0x%X)\n", reg1, generalRegisters[reg1]);
#if VIRTUAL_PC
    programCounter = generalRegisters[reg1];
#else
    programCounter = memoryManagmentUnit.read<const char*>(generalRegisters[reg1]);
#endif
    cycles += 3;
}

void CPU::v810::jumpRelative(uint32_t disp26)
{
    int32_t relativeJump = sign_extend<26>(disp26);
    d_printf("JR: PC <- PC + 0x%X\n", relativeJump);
    programCounter += relativeJump;
    cycles += 3;
}

void CPU::v810::branchIfNoOverflow(uint16_t disp9)
{
    d_printf("%s\n", "BNV");
    if (systemRegisters.PSW.OV == 0)
    {
        programCounter += sign_extend<9>(disp9);
        cycles += 3;
    }
    else
    {
        programCounter += 2;
        cycles += 1;
    }
}

void CPU::v810::branchIfOverflow(uint16_t disp9)
{
    d_printf("%s\n", "BV");
    if (systemRegisters.PSW.OV)
    {
        programCounter += sign_extend<9>(disp9);
        cycles += 3;
    }
    else
    {
        programCounter += 2;
        cycles += 1;
    }
}

void CPU::v810::branchIfZero(uint16_t disp9)
{
    d_printf("%s\n", "BZ");
    if (systemRegisters.PSW.Z)
    {
        programCounter += sign_extend<9>(disp9);
        cycles += 3;
    }
    else
    {
        programCounter += 2;
        cycles += 1;
    }
}

void CPU::v810::branchIfNotZero(uint16_t disp9)
{
    d_printf("%s\n", "BNZ");
    if (systemRegisters.PSW.Z == 0)
    {
        programCounter += sign_extend<9>(disp9);
        cycles += 3;
    }
    else
    {
        programCounter += 2;
        cycles += 1;
    }
}


void CPU::v810::branchIfLessThan(uint16_t disp9)
{
    d_printf("%s\n", "BLT");
    if ((systemRegisters.PSW.S ^ systemRegisters.PSW.OV) == 1)
    {
        programCounter += sign_extend<9>(disp9);
        cycles += 3;
    }
    else
    {
        programCounter += 2;
        cycles += 1;
    }
}

void CPU::v810::branchIfNotHigher(uint16_t disp9)
{
    d_printf("%s\n", "BNH");
    if ((systemRegisters.PSW.Z | systemRegisters.PSW.CY) == 1)
    {
        programCounter += sign_extend<9>(disp9);
        cycles += 3;
    }
    else
    {
        programCounter += 2;   
        cycles += 1;
    }
}

void CPU::v810::branchIfNegative(uint16_t disp9)
{
    d_printf("%s\n", "BN");
    if (systemRegisters.PSW.S)
    {
        programCounter += sign_extend<9>(disp9);
        cycles += 3;
    }
    else
    {
        programCounter += 2;   
        cycles += 1;
    }
}

void CPU::v810::branchIfPositive(uint16_t disp9)
{
    d_printf("%s\n", "BP");
    if (systemRegisters.PSW.S == 0)
    {
        programCounter += sign_extend<9>(disp9);
        cycles += 3;
    }
    else
    {
        programCounter += 2;   
        cycles += 1;
    }
}


void CPU::v810::branchIfHigher(uint16_t disp9)
{
    d_printf("%s\n", "BH");
    if ((systemRegisters.PSW.Z | systemRegisters.PSW.CY) == 0)
    {
        programCounter += sign_extend<9>(disp9);
        cycles += 3;
    }
    else
    {
        programCounter += 2;   
        cycles += 1;
    }
}

void CPU::v810::branchIfGreaterOrEqual(uint16_t disp9)
{
    d_printf("BGE\n");
    if ((systemRegisters.PSW.S ^ systemRegisters.PSW.OV) == 0)
    {
        programCounter += sign_extend<9>(disp9);
        cycles += 3;
    }
    else
    {
        programCounter += 2;   
        cycles += 1;
    }
}

void CPU::v810::branchIfGreaterThan(uint16_t disp9)
{
    d_printf("BGT\n");
    if (((systemRegisters.PSW.S ^ systemRegisters.PSW.OV) | systemRegisters.PSW.Z) == 0)
    {
        programCounter += sign_extend<9>(disp9);
        cycles += 3;
    }
    else
    {
        programCounter += 2;   
        cycles += 1;
    }
}

void CPU::v810::branchIfLessOrEqual(uint16_t disp9)
{
    d_printf("BLE\n");
    if (((systemRegisters.PSW.S ^ systemRegisters.PSW.OV) | systemRegisters.PSW.Z) == 1)
    {
        programCounter += sign_extend<9>(disp9);
        cycles += 3;
    }
    else
    {
        programCounter += 2;   
        cycles += 1;
    }
}

void CPU::v810::nop(uint16_t disp9)
{
    d_printf("NOP\n");
    programCounter += 2;
    cycles += 1;
}

void CPU::v810::branchIfCarry(uint16_t disp9)
{
    d_printf("BC\n");
    if (systemRegisters.PSW.CY)
    {
        programCounter += sign_extend<9>(disp9);
        cycles += 3;
    }
    else
    {
        programCounter += 2;   
        cycles += 1;
    }
}

void CPU::v810::branchIfNoCarry(uint16_t disp9)
{
    d_printf("BNC\n");
    if (systemRegisters.PSW.CY == 0)
    {
        programCounter += sign_extend<9>(disp9);
        cycles += 3;
    }
    else
    {
        programCounter += 2;   
        cycles += 1;
    }
}

void CPU::v810::branch(uint16_t disp9)
{
    d_printf("BR\n");
    programCounter += sign_extend<9>(disp9);
    cycles += 3;
}

void CPU::v810::moveHigh(uint8_t reg1, uint8_t reg2, uint16_t imm16)
{
    d_printf("MOVEHI: GR[%d] <- GR[%d](0x%X) + 0x%X\n", reg2, reg1, generalRegisters[reg1], (imm16 << 16));
    generalRegisters[reg2] = generalRegisters[reg1] + (imm16 << 16);
    programCounter += 4;
    cycles += 1;
}

void CPU::v810::moveAddImmediate(uint8_t reg1, uint8_t reg2, uint16_t imm16)
{
    d_printf("MOVEA: GR[%d] <- GR[%d](0x%X) + 0x%X\n", reg2, reg1, generalRegisters[reg1], sign_extend<16>(imm16));
    generalRegisters[reg2] = generalRegisters[reg1] + sign_extend<16>(imm16);
    programCounter += 4;
    cycles += 1;
}

void CPU::v810::storeWord(uint8_t reg1, uint8_t reg2, uint16_t disp16)
{
    uint32_t address = (generalRegisters[reg1] + (int16_t)disp16) & 0xFFFFFFFC;
    d_printf("ST.W: [GR[%d](0x%X) + %d](0x%X) <- [GR[%d](0x%X)\n", reg1, generalRegisters[reg1], sign_extend<16>(disp16) & 0xFFFFFFFC, address, reg2, generalRegisters[reg2]);

    memoryManagmentUnit.store<uint32_t>(generalRegisters[reg2], address);
    programCounter += 4;
    cycles += 1;
}

void CPU::v810::storeHWord(uint8_t reg1, uint8_t reg2, uint16_t disp16)
{
    uint32_t address = (generalRegisters[reg1] + (int16_t)disp16) & 0xFFFFFFFE;
    d_printf("ST.H: [GR[%d](0x%X) + %d](0x%X) <- [GR[%d](0x%X)\n", reg1, generalRegisters[reg1], sign_extend<16>(disp16) & 0xFFFFFFFE, address, reg2, generalRegisters[reg2]);

    
    memoryManagmentUnit.store<uint16_t>(generalRegisters[reg2] & 0xFFFF, address);
    programCounter += 4;
    cycles += 1;
}

void CPU::v810::storeByte(uint8_t reg1, uint8_t reg2, uint16_t disp16)
{
    uint32_t address = (generalRegisters[reg1] + (int16_t)disp16);
    d_printf("ST.B: [GR[%d](0x%X) + %d](0x%X) <- [GR[%d](0x%X)\n", reg1, generalRegisters[reg1], sign_extend<16>(disp16), address, reg2, generalRegisters[reg2]);

//    if (address == 0x5004194)
//        printf("Writing %d val\n", generalRegisters[reg2] & 0xFF);
    memoryManagmentUnit.store<uint8_t>(generalRegisters[reg2] & 0xFF, address);
    programCounter += 4;
    cycles += 1;
}

void CPU::v810::outWord(uint8_t reg1, uint8_t reg2, uint16_t disp16)
{
    uint32_t address = (generalRegisters[reg1] + (int16_t)disp16) & 0xFFFFFFFC;
    d_printf("OUT.W: [GR[%d](0x%X) + %d](0x%X) <- [GR[%d](0x%X)\n", reg1, generalRegisters[reg1], sign_extend<16>(disp16) & 0xFFFFFFFC, address, reg2, generalRegisters[reg2]);
    
    memoryManagmentUnit.store<uint32_t>(generalRegisters[reg2], address);
    programCounter += 4;
    cycles += 1;
}

void CPU::v810::outHWord(uint8_t reg1, uint8_t reg2, uint16_t disp16)
{
    uint32_t address = (generalRegisters[reg1] + (int16_t)disp16) & 0xFFFFFFFE;
    d_printf("OUT.H: [GR[%d](0x%X) + %d](0x%X) <- [GR[%d](0x%X)\n", reg1, generalRegisters[reg1], sign_extend<16>(disp16) & 0xFFFFFFFE, address, reg2, generalRegisters[reg2]);
    
    
    memoryManagmentUnit.store<uint16_t>(generalRegisters[reg2] & 0xFFFF, address);
    programCounter += 4;
    cycles += 1;
}

void CPU::v810::outByte(uint8_t reg1, uint8_t reg2, uint16_t disp16)
{
    uint32_t address = (generalRegisters[reg1] + (int16_t)disp16);
    d_printf("OUT.B: [GR[%d](0x%X) + %d](0x%X) <- [GR[%d](0x%X)\n", reg1, generalRegisters[reg1], sign_extend<16>(disp16), address, reg2, generalRegisters[reg2]);
    
    //    if (address == 0x5004194)
    //        printf("Writing %d val\n", generalRegisters[reg2] & 0xFF);
    memoryManagmentUnit.store<uint8_t>(generalRegisters[reg2] & 0xFF, address);
    programCounter += 4;
    cycles += 1;
}


void CPU::v810::loadByte(uint8_t reg1, uint8_t reg2, uint16_t disp16)
{
    d_printf("LD.B: GR[%d] <- [GR[%d](0x%X) + %d\n", reg2, reg1, generalRegisters[reg1], sign_extend<16>(disp16));

    uint32_t address = (generalRegisters[reg1] + (int16_t)disp16);
    
    int8_t val = memoryManagmentUnit.read<int8_t>(address);
//    if (address == 0x5004194)
//        printf("Reading %d val\n", val);
    
    generalRegisters[reg2] = sign_extend<8>(val);
    programCounter += 4;    
    cycles += 3;
}

void CPU::v810::loadHWord(uint8_t reg1, uint8_t reg2, uint16_t disp16)
{
    uint32_t address = (generalRegisters[reg1] + (int16_t)disp16);
    address &= 0xFFFFFFFE;
    d_printf("LD.H: GR[%d] <- [GR[%d](0x%X) + %d]\n", reg2, reg1, generalRegisters[reg1], sign_extend<16>(disp16));
    
    generalRegisters[reg2] = sign_extend<16>(memoryManagmentUnit.read<int16_t>(address));
    programCounter += 4;    
    cycles += 3;
}

void CPU::v810::loadWord(uint8_t reg1, uint8_t reg2, uint16_t disp16)
{
    uint32_t address = (generalRegisters[reg1] + (int16_t)disp16) & 0xFFFFFFFC;
    d_printf("LD.W: GR[%d] <- [GR[%d](0x%X) + %d]\n", reg2, reg1, generalRegisters[reg1], sign_extend<16>(disp16));
    
    generalRegisters[reg2] = memoryManagmentUnit.read<int32_t>(address);
    programCounter += 4;    
    cycles += 3;
}

void CPU::v810::inByte(uint8_t reg1, uint8_t reg2, uint16_t disp16)
{
    d_printf("IN.B: GR[%d] <- [GR[%d](0x%X) + %d]\n", reg2, reg1, generalRegisters[reg1], sign_extend<16>(disp16));
    
    uint32_t address = (generalRegisters[reg1] + (int16_t)disp16);
    
    uint8_t val = memoryManagmentUnit.read<uint8_t>(address);
    //    if (address == 0x5004194)
    //        printf("Reading %d val\n", val);
    
    generalRegisters[reg2] = val;
    programCounter += 4;    
    cycles += 3;
}

void CPU::v810::inHWord(uint8_t reg1, uint8_t reg2, uint16_t disp16)
{
    uint32_t address = (generalRegisters[reg1] + (int16_t)disp16);
    address &= 0xFFFFFFFE;
    d_printf("IN.H: GR[%d] <- [GR[%d](0x%X) + %d\n", reg2, reg1, generalRegisters[reg1], sign_extend<16>(disp16));
    
    generalRegisters[reg2] = memoryManagmentUnit.read<uint16_t>(address);
    programCounter += 4;    
    cycles += 3;
}

void CPU::v810::inWord(uint8_t reg1, uint8_t reg2, uint16_t disp16)
{
    uint32_t address = (generalRegisters[reg1] + (int16_t)disp16) & 0xFFFFFFFC;
    d_printf("IN.W: GR[%d] <- [GR[%d](0x%X) + %d\n", reg2, reg1, generalRegisters[reg1], sign_extend<16>(disp16));
    
    generalRegisters[reg2] = memoryManagmentUnit.read<uint32_t>(address);
    programCounter += 4;    
    cycles += 3;
}

void CPU::v810::loadSystemRegister(uint8_t reg1, uint8_t reg2)
{
    d_printf("LDSR %d\n", reg1);
    //TODO: Prevent writing to write disabled registers
    systemRegisters[reg1] = generalRegisters[reg2];
    programCounter += 2;
    cycles += 1;
}

void CPU::v810::storeSystemRegister(uint8_t reg1, uint8_t reg2)
{
    d_printf("STSR %d\n", reg1);
    //TODO: Prevent reading from reserved registers
    generalRegisters[reg2] = systemRegisters[reg1];
    programCounter += 2;   
    cycles += 1;
}

void CPU::v810::jumpAndLink(uint32_t disp26)
{
    d_printf("JAL\n");
#if VIRTUAL_PC
    uint32_t currentPC = programCounter + 4;
#else
    uint32_t currentPC = (0x07000000 + (uint32_t)(((char*)programCounter + 4) - ((char*)memoryManagmentUnit.rom.data)));
#endif
    generalRegisters[31] = currentPC;
    currentPC -= 4;
    int32_t relativeJump = sign_extend<26>(disp26);
    currentPC += relativeJump;
    
#if VIRTUAL_PC
    programCounter = currentPC;
#else
    programCounter = memoryManagmentUnit.read<const char*>(currentPC);
#endif
    cycles += 3;
}

void CPU::v810::outWrite(uint8_t reg1, uint8_t reg2, uint16_t disp16)
{
    d_printf("OUT.W\n");
    programCounter += 4;
}

void CPU::v810::setInterruptDisable(uint8_t reg1, uint8_t reg2)
{
    d_printf("SEI\n");
    systemRegisters.PSW.ID = 1;
    programCounter += 2;
    cycles += 1;
}

void CPU::v810::clearInterruptDisable(uint8_t reg1, uint8_t reg2)
{
    d_printf("CLI\n");
    systemRegisters.PSW.ID = 0;
    programCounter += 2;
    cycles += 1;
}

void CPU::v810::returnFromTrap(uint8_t imm5, uint8_t reg2)
{
    d_printf("RETI\n");
    if (systemRegisters.PSW.NP)
    {
#if VIRTUAL_PC
        programCounter = systemRegisters.FEPC;
#else
        programCounter = memoryManagmentUnit.read<const char*>(systemRegisters.FEPC);
#endif
        systemRegisters.PSW = systemRegisters.FEPSW;
    }
    else
    {
#if VIRTUAL_PC
        programCounter = systemRegisters.EIPC;
#else
        programCounter = memoryManagmentUnit.read<const char*>(systemRegisters.EIPC);
#endif

        systemRegisters.PSW = systemRegisters.EIPSW;            
    }
    cycles += 10;
}

void CPU::v810::compareFloat(uint8_t reg1, uint8_t reg2)
{
    d_printf("CMPF.S\n");
    float f = generalRegistersFloat[reg2] - generalRegistersFloat[reg1];
    systemRegisters.PSW.Z = (f == 0.0f);
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.S = (f < 0.0f);
    systemRegisters.PSW.CY = (f < 0.0f);  
    programCounter += 4;
    cycles += 7;
//        systemRegisters.PSW.FRO = 
}

void CPU::v810::convertWordToFloat(uint8_t reg1, uint8_t reg2)
{
    d_printf("CVT.WS\n");
    float f = generalRegisters[reg1];
    generalRegistersFloat[reg2] = f;
    
    systemRegisters.PSW.Z = (f == 0.0f);
    systemRegisters.PSW.S = (f < 0.0f);
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.CY = (f < 0.0f); 
    
    programCounter += 4;
    cycles += 5;
}

void CPU::v810::convertFloatToWord(uint8_t reg1, uint8_t reg2)
{
    d_printf("CVT.SW\n");
    float f = generalRegistersFloat[reg1];
    int32_t t = 0;
    if (f >= 0)
        t = floorf(f + .5f);
    else
        t = ceilf(f - .5f);
    
    generalRegisters[reg2] = t;
    
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.S = (t < 0);
    systemRegisters.PSW.Z = (t == 0);
    
    programCounter += 4;
    cycles += 9;
}

void CPU::v810::addFloat(uint8_t reg1, uint8_t reg2)
{
    d_printf("ADDF.S\n");
    
    float f = generalRegistersFloat[reg2] + generalRegistersFloat[reg1];
    
    generalRegistersFloat[reg2] = f;
    
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.CY = (f < 0.0f);
    systemRegisters.PSW.S = (f < 0.0f);
    systemRegisters.PSW.Z = (f == 0.0f);
    programCounter += 4;
    cycles += 9;
}

void CPU::v810::subtractFloat(uint8_t reg1, uint8_t reg2)
{
    d_printf("SUBF.S\n");
    
    float f = generalRegistersFloat[reg2] - generalRegistersFloat[reg1];
    
    generalRegistersFloat[reg2] = f;
    
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.CY = (f < 0.0f);
    systemRegisters.PSW.S = (f < 0.0f);
    systemRegisters.PSW.Z = (f == 0.0f);
    programCounter += 4;
    cycles += 12;
}

void CPU::v810::multiplyFloat(uint8_t reg1, uint8_t reg2)
{
    d_printf("MULF.S\n");
    
    float f = generalRegistersFloat[reg2] * generalRegistersFloat[reg1];
    
    generalRegistersFloat[reg2] = f;
    
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.CY = (f < 0.0f);
    systemRegisters.PSW.S = (f < 0.0f);
    systemRegisters.PSW.Z = (f == 0.0f);
    programCounter += 4;
    cycles += 8;
}

void CPU::v810::divideFloat(uint8_t reg1, uint8_t reg2)
{
    d_printf("DIVF.S\n");
    
    float f = generalRegistersFloat[reg2] / generalRegistersFloat[reg1];
    
    generalRegistersFloat[reg2] = f;
    
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.CY = (f < 0.0f);
    systemRegisters.PSW.S = (f < 0.0f);
    systemRegisters.PSW.Z = (f == 0.0f);
    programCounter += 4;
    cycles += 44;
}

void CPU::v810::truncateFloat(uint8_t reg1, uint8_t reg2)
{
    d_printf("TRNC.SW\n");
    float f = generalRegistersFloat[reg1];
    int32_t t = 0;
    if (f >= 0)
        t = floorf(f);
    else
        t = ceilf(f);
    
    generalRegisters[reg2] = t;
    
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.S = (t < 0);
    systemRegisters.PSW.Z = (t == 0);
    
    programCounter += 4;
    cycles += 8;
}

void CPU::v810::exchangeByte(uint8_t reg1, uint8_t reg2)
{
    d_printf("XB\n");
    
    int32_t val = generalRegisters[reg2];
    val = (val & 0xFFFF0000) | ((val << 8) & 0x0000FF00) | ((val >> 8) & 0x000000FF);
    generalRegisters[reg2] = val;
    programCounter += 4;
    cycles += 1;
}

void CPU::v810::exchangeHalfWord(uint8_t reg1, uint8_t reg2)
{
    d_printf("XH\n");
    int32_t val = generalRegisters[reg2];
    val = ((val << 16) & 0xFFFF0000) | ((val >> 16) & 0x0000FFFF);
    generalRegisters[reg2] = val;
    programCounter += 4;
    cycles += 1;
}

void CPU::v810::reverseWord(uint8_t reg1, uint8_t reg2)
{
    d_printf("REV\n");
    int32_t word = generalRegisters[reg1];
    int32_t out = 0;
    for (int i = 0; i < 32; ++i)
    {
        out |= (word & 1);
        out = out << 1;
        word = word >> 1;
    }
    
    generalRegisters[reg2] = word;
    programCounter += 4;
    cycles += 1;
}

void CPU::v810::multiplyHalfWord(uint8_t reg1, uint8_t reg2)
{
    d_printf("MPYHW\n");
    
    generalRegisters[reg2] = (int32_t)(int16_t)(generalRegisters[reg2] & 0xFFFF) * (int32_t)(int16_t)(generalRegisters[reg1] & 0xFFFF);
    
    programCounter += 4;
    cycles += 9;
}

void CPU::v810::moveBitString(uint8_t reg1, uint8_t reg2)
{
    d_printf("MOVBSU\n");
    uint32_t startAddress = generalRegisters[30] & 0xFFFFFFFC;
    uint32_t destAddress = generalRegisters[29] & 0xFFFFFFFC;
    uint32_t length = generalRegisters[28];
    uint32_t offsetInSource = generalRegisters[27];
    uint32_t offsetInDest = generalRegisters[26];
    
    Bitstring source(memoryManagmentUnit, startAddress, offsetInSource, length);
    Bitstring dest(memoryManagmentUnit, destAddress, offsetInDest, length);
    
    uint32_t data;
    uint8_t bits;
    
    
    while (source.HasData())
    {
        source.Read(data, bits);
        dest.SetNext(Mover(), data, bits);
    }
    
    assert(!source.HasData() && !dest.HasData());
    
    generalRegisters[30] = source.currentLocation;
    generalRegisters[29] = dest.currentLocation;
    generalRegisters[28] = source.stringLength;
    generalRegisters[27] = source.offset;
    generalRegisters[26] = dest.offset;
    
    programCounter += 2;
}

void CPU::v810::andBitString(uint8_t reg1, uint8_t reg2)
{
    d_printf("ANDBSU\n");
    uint32_t startAddress = generalRegisters[30];
    uint32_t destAddress = generalRegisters[29];
    uint32_t length = generalRegisters[28];
    uint32_t offsetInSource = generalRegisters[27];
    uint32_t offsetInDest = generalRegisters[26];
    
    Bitstring source(memoryManagmentUnit, startAddress, offsetInSource, length);
    Bitstring dest(memoryManagmentUnit, destAddress, offsetInDest, length);
    
    uint32_t data;
    uint8_t bits;
    
    while (source.HasData())
    {
        source.Read(data, bits);
        dest.SetNext(Ander(), data, bits);
    }
    
    assert(!source.HasData() && !dest.HasData());
    generalRegisters[30] = source.currentLocation;
    generalRegisters[29] = dest.currentLocation;
    generalRegisters[28] = source.stringLength;
    generalRegisters[27] = source.offset;
    generalRegisters[26] = dest.offset;
    programCounter += 2;
}

void CPU::v810::orBitString(uint8_t reg1, uint8_t reg2)
{
    d_printf("ORBSU\n");
    uint32_t startAddress = generalRegisters[30];
    uint32_t destAddress = generalRegisters[29];
    uint32_t length = generalRegisters[28];
    uint32_t offsetInSource = generalRegisters[27];
    uint32_t offsetInDest = generalRegisters[26];
    d_printf("\tsource=0x%X\n", startAddress);
    d_printf("\tdest=0x%X\n", destAddress);
    d_printf("\tlength=%u\n", length);
    d_printf("\tsourceoff=%u\n", offsetInSource);
    d_printf("\tdestoff=%u\n", offsetInDest);
    
    Bitstring source(memoryManagmentUnit, startAddress, offsetInSource, length);
    Bitstring dest(memoryManagmentUnit, destAddress, offsetInDest, length);
    
    uint32_t data;
    uint8_t bits;
    
    while (source.HasData())
    {
        source.Read(data, bits);
        dest.SetNext(Orer(), data, bits);
    }
    
    generalRegisters[30] = source.currentLocation;
    generalRegisters[29] = dest.currentLocation;
    generalRegisters[28] = source.stringLength;
    generalRegisters[27] = source.offset;
    generalRegisters[26] = dest.offset;
    
    assert(!source.HasData() && !dest.HasData());
    
    programCounter += 2;
}

void CPU::v810::xorBitString(uint8_t reg1, uint8_t reg2)
{
    d_printf("XORBSU\n");
    uint32_t startAddress = generalRegisters[30];
    uint32_t destAddress = generalRegisters[29];
    uint32_t length = generalRegisters[28];
    uint32_t offsetInSource = generalRegisters[27];
    uint32_t offsetInDest = generalRegisters[26];
    
    Bitstring source(memoryManagmentUnit, startAddress, offsetInSource, length);
    Bitstring dest(memoryManagmentUnit, destAddress, offsetInDest, length);
    
    uint32_t data;
    uint8_t bits;
    
    while (source.HasData())
    {
        source.Read(data, bits);
        dest.SetNext(Xorer(), data, bits);
    }
    
    assert(!source.HasData() && !dest.HasData());
    generalRegisters[30] = source.currentLocation;
    generalRegisters[29] = dest.currentLocation;
    generalRegisters[28] = source.stringLength;
    generalRegisters[27] = source.offset;
    generalRegisters[26] = dest.offset;
    programCounter += 2;
}

void CPU::v810::andNotBitString(uint8_t reg1, uint8_t reg2)
{
    d_printf("ANDNBSU\n");
    uint32_t startAddress = generalRegisters[30];
    uint32_t destAddress = generalRegisters[29];
    uint32_t length = generalRegisters[28];
    uint32_t offsetInSource = generalRegisters[27];
    uint32_t offsetInDest = generalRegisters[26];
    
    Bitstring source(memoryManagmentUnit, startAddress, offsetInSource, length);
    Bitstring dest(memoryManagmentUnit, destAddress, offsetInDest, length);
    
    uint32_t data;
    uint8_t bits;
    

    while (source.HasData())
    {
        source.Read(data, bits);
        dest.SetNext(AndNoter(), data, bits);
    }
    
    assert(!source.HasData() && !dest.HasData());
    generalRegisters[30] = source.currentLocation;
    generalRegisters[29] = dest.currentLocation;
    generalRegisters[28] = source.stringLength;
    generalRegisters[27] = source.offset;
    generalRegisters[26] = dest.offset;
    programCounter += 2;
}

void CPU::v810::orNotBitString(uint8_t reg1, uint8_t reg2)
{
    d_printf("ORNBSU\n");
    uint32_t startAddress = generalRegisters[30];
    uint32_t destAddress = generalRegisters[29];
    uint32_t length = generalRegisters[28];
    uint32_t offsetInSource = generalRegisters[27];
    uint32_t offsetInDest = generalRegisters[26];
    
    Bitstring source(memoryManagmentUnit, startAddress, offsetInSource, length);
    Bitstring dest(memoryManagmentUnit, destAddress, offsetInDest, length);
    
    uint32_t data;
    uint8_t bits;

    
    while (source.HasData())
    {
        source.Read(data, bits);
        dest.SetNext(OrNoter(), data, bits);
    }
    
    assert(!source.HasData() && !dest.HasData());
    generalRegisters[30] = source.currentLocation;
    generalRegisters[29] = dest.currentLocation;
    generalRegisters[28] = source.stringLength;
    generalRegisters[27] = source.offset;
    generalRegisters[26] = dest.offset;
    programCounter += 2;
}

void CPU::v810::xorNotBitString(uint8_t reg1, uint8_t reg2)
{
    d_printf("XORNBSU\n");
    uint32_t startAddress = generalRegisters[30];
    uint32_t destAddress = generalRegisters[29];
    uint32_t length = generalRegisters[28];
    uint32_t offsetInSource = generalRegisters[27];
    uint32_t offsetInDest = generalRegisters[26];
    
    Bitstring source(memoryManagmentUnit, startAddress, offsetInSource, length);
    Bitstring dest(memoryManagmentUnit, destAddress, offsetInDest, length);
    
    uint32_t data;
    uint8_t bits;
    
    while (source.HasData())
    {
        source.Read(data, bits);
        dest.SetNext(XorNoter(), data, bits);
    }
    generalRegisters[30] = source.currentLocation;
    generalRegisters[29] = dest.currentLocation;
    generalRegisters[28] = source.stringLength;
    generalRegisters[27] = source.offset;
    generalRegisters[26] = dest.offset;
    assert(!source.HasData() && !dest.HasData());
    
    programCounter += 2;
}

void CPU::v810::notBitString(uint8_t reg1, uint8_t reg2)
{
    d_printf("XORNBSU\n");
    uint32_t startAddress = generalRegisters[30];
    uint32_t destAddress = generalRegisters[29];
    uint32_t length = generalRegisters[28];
    uint32_t offsetInSource = generalRegisters[27];
    uint32_t offsetInDest = generalRegisters[26];
    
    Bitstring source(memoryManagmentUnit, startAddress, offsetInSource, length);
    Bitstring dest(memoryManagmentUnit, destAddress, offsetInDest, length);
    
    uint32_t data;
    uint8_t bits;
    
    while (source.HasData())
    {
        source.Read(data, bits);
        dest.SetNext(Noter(), data, bits);
    }
    
    assert(!source.HasData() && !dest.HasData());
    generalRegisters[30] = source.currentLocation;
    generalRegisters[29] = dest.currentLocation;
    generalRegisters[28] = source.stringLength;
    generalRegisters[27] = source.offset;
    generalRegisters[26] = dest.offset;
    programCounter += 2;
}


#endif
