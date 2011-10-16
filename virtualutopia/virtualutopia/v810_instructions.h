//
//  v810_instructions.cpp
//  virtualutopia
//

//#include "v810.h"
//#include "instruction.h"
//#include "cpu_utils.h"
//#include "mmu.h"
//#include "bitstring.h"
//#include <cmath>

#ifndef v810_instructions_guard
#define v810_instructions_guard
//#define d_printf(A, ...)
#define d_printf(A, ...) do{ if(debugOutput) { printf("%X: " A, (0x07000000 + (uint32_t)((char*)programCounter - ((char*)memoryManagmentUnit.rom.data))), ##__VA_ARGS__); } }while(0)

struct OrNoter
{
    OrNoter() {}
    const uint32_t operator()(uint32_t a, uint32_t b) const { return a | (~b); };
};

struct XorNoter
{
    XorNoter() {}
    const uint32_t operator()(uint32_t a, uint32_t b) const { return a | (~b); };
};

struct Noter
{
    Noter() {}
    const uint32_t operator()(uint32_t a, uint32_t b) const { return ~b; };
};

struct Mover
{
    Mover() {}
    const uint32_t operator()(uint32_t a, uint32_t b) const { return b; };
};

struct Xorer
{
    Xorer() {}
    const uint32_t operator()(uint32_t a, uint32_t b) const { return a ^ b; };
};

struct AndNoter
{
    AndNoter() {}
    const uint32_t operator()(uint32_t a, uint32_t b) const { return a & (~b); };
};

struct Ander
{
    Ander() {}
    const uint32_t operator()(uint32_t a, uint32_t b) const { return a & b; };
};

struct Orer
{
    Orer() {}
    const uint32_t operator()(uint32_t a, uint32_t b) const { return a | b; };
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
inline void move(const Instruction& instruction)
{
    d_printf("MOV: GR[%d] <- GR[%d](0x%X)\n", instruction.reg2, instruction.reg1, generalRegisters[instruction.reg1]);
    generalRegisters[instruction.reg2] = generalRegisters[instruction.reg1];
    programCounter += 2;
    cycles += 1;
}

//Move Form II
inline void moveImmediate(const Instruction& instruction)
{
    d_printf("MOV: GR[%d] <- (0x%X)\n", instruction.reg2, sign_extend(5, instruction.reg1));
    generalRegisters[instruction.reg2] = sign_extend(5, instruction.reg1);
    programCounter += 2;
    cycles += 1;
}

//Add Form I
inline void add(const Instruction& instruction)
{
    d_printf("ADD: GR[%d] <- GR[%d](0x%X) + GR[%d](0x%X)\n", instruction.reg2, instruction.reg2, generalRegisters[instruction.reg2], instruction.reg1, generalRegisters[instruction.reg1]);
    int32_t a = generalRegisters[instruction.reg1];
    int32_t b = generalRegisters[instruction.reg2];
    int64_t r = a+b;
    int32_t truncatedResult = (int32_t)r;
    generalRegisters[instruction.reg2] = truncatedResult;           
    
    systemRegisters.PSW.OV = (sign(a) == sign(b)) ? (sign(truncatedResult) != sign(a)) : 0;
    systemRegisters.PSW.CY = r > INT32_MAX || r < INT32_MIN;
    systemRegisters.PSW.S = truncatedResult < 0;
    systemRegisters.PSW.Z = truncatedResult == 0;

    programCounter += 2;
    cycles += 1;
}

//Add Form II
inline void addImmediate5(const Instruction& instruction)
{
    d_printf("ADD: GR[%d] <- GR[%d](0x%X) + 0x%X\n", instruction.reg2, instruction.reg2, generalRegisters[instruction.reg2], sign_extend(5, instruction.reg1));
    int32_t a = sign_extend(5, instruction.reg1);
    int32_t b = generalRegisters[instruction.reg2];
    int64_t r = a+b;
    int32_t truncatedResult = (int32_t)r;
    generalRegisters[instruction.reg2] = truncatedResult;           
    
    systemRegisters.PSW.OV = (sign(a) == sign(b)) ? (sign(truncatedResult) != sign(a)) : 0;
    systemRegisters.PSW.CY = r > INT32_MAX || r < INT32_MIN;
    systemRegisters.PSW.S = truncatedResult < 0;
    systemRegisters.PSW.Z = truncatedResult == 0;
    
    programCounter += 2;
    cycles += 1;
}


inline void addImmediate(const Instruction& instruction)
{
    d_printf("ADDI: GR[%d] <- GR[%d](0x%X) + 0x%X\n", instruction.reg2, instruction.reg1, generalRegisters[instruction.reg1], sign_extend(16, instruction.imm16));
    
    int32_t a = sign_extend(16, instruction.imm16);
    int32_t b = generalRegisters[instruction.reg1];
    int64_t r = a+b;
    int32_t truncatedResult = (int32_t)r;
    generalRegisters[instruction.reg2] = truncatedResult;           
    
    systemRegisters.PSW.OV = (sign(a) == sign(b)) ? (sign(truncatedResult) != sign(a)) : 0;
    systemRegisters.PSW.CY = r > INT32_MAX || r < INT32_MIN;
    systemRegisters.PSW.S = truncatedResult < 0;
    systemRegisters.PSW.Z = truncatedResult == 0;
    programCounter += 4;
    cycles += 1;
}


//Subtract
inline void subtract(const Instruction& instruction)
{
    d_printf("SUB: GR[%d] <- GR[%d](0x%X) - GR[%d](0x%X)\n", instruction.reg2, instruction.reg2, generalRegisters[instruction.reg2], instruction.reg1, generalRegisters[instruction.reg1]);
    int32_t a = generalRegisters[instruction.reg1];
    int32_t b = generalRegisters[instruction.reg2];
    uint32_t r = b-a;
    generalRegisters[instruction.reg2] = (int32_t)r;           
    
    systemRegisters.PSW.OV = calculate_overflow_subtract(a, b);
    systemRegisters.PSW.CY = r > b;
    systemRegisters.PSW.S = (int32_t)r < 0;
    systemRegisters.PSW.Z = (int32_t)r == 0;
    
    programCounter += 2;
    cycles += 1;
}

inline void divide(const Instruction& instruction)
{
    d_printf("DIV: GR[%d] <- GR[%d](0x%X) / GR[%d](0x%X)\n", instruction.reg2, instruction.reg2, generalRegisters[instruction.reg2], instruction.reg1, generalRegisters[instruction.reg1]);
    int32_t a = generalRegisters[instruction.reg1];
    int32_t b = generalRegisters[instruction.reg2];
    
    if (a == 0)
    {
        throwException(DIV0);
        return;
    }
    
    int32_t result = b / a;
    
    generalRegisters[30] = b % a;
    generalRegisters[instruction.reg2] = result;
    
    systemRegisters.PSW.OV = (b == 0x80000000 && a == 0xFFFFFFF) ? 1 : 0;
    systemRegisters.PSW.S = (generalRegisters[instruction.reg2] < 0) ? 1 : 0;
    systemRegisters.PSW.Z = (generalRegisters[instruction.reg2] == 0) ? 1 : 0;
    
    programCounter += 2;
    cycles += 38;
}

inline void divideUnsigned(const Instruction& instruction)
{
    d_printf("DIVU: GR[%d] <- GR[%d](0x%X) / GR[%d](0x%X)\n", instruction.reg2, instruction.reg2, generalRegisters[instruction.reg2], instruction.reg1, generalRegisters[instruction.reg1]);
    uint32_t a = generalRegisters[instruction.reg1];
    int32_t b = generalRegisters[instruction.reg2];
    
    if (a == 0)
    {
        throwException(DIV0);
        return;
    }
    
    int32_t result = b / a;
    
    generalRegisters[30] = b % a;
    generalRegisters[instruction.reg2] = result;
    
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.S = (generalRegisters[instruction.reg2] < 0) ? 1 : 0;
    systemRegisters.PSW.Z = (generalRegisters[instruction.reg2] == 0) ? 1 : 0;
    
    programCounter += 2;
    cycles += 36;
}

inline void multiply(const Instruction& instruction)
{
    d_printf("MUL: GR[%d] <- GR[%d](0x%X) * GR[%d](0x%X)\n", instruction.reg2, instruction.reg2, generalRegisters[instruction.reg2], instruction.reg1, generalRegisters[instruction.reg1]);
    int32_t a = generalRegisters[instruction.reg1];
    int32_t b = generalRegisters[instruction.reg2];
    int64_t result = b * a;
    
    generalRegisters[30] = (int32_t)(result >> 32);
    generalRegisters[instruction.reg2] = (int32_t)(result & 0xFFFFFFFF);
    
    systemRegisters.PSW.OV = (generalRegisters[30] == 0) ? 0 : 1;
    systemRegisters.PSW.S = (generalRegisters[instruction.reg2] < 0) ? 1 : 0;
    systemRegisters.PSW.Z = (generalRegisters[instruction.reg2] == 0) ? 1 : 0;
    
    programCounter += 2;
    cycles += 13;
}

inline void multiplyUnsigned(const Instruction& instruction)
{
    d_printf("MULU: GR[%d] <- GR[%d](0x%X) * GR[%d](0x%X)\n", instruction.reg2, instruction.reg2, generalRegisters[instruction.reg2], instruction.reg1, generalRegisters[instruction.reg1]);
    uint32_t a = generalRegisters[instruction.reg1];
    uint32_t b = generalRegisters[instruction.reg2];
    uint64_t result = b * a;
    
    generalRegisters[30] = (int32_t)(result >> 32);
    generalRegisters[instruction.reg2] = (int32_t)(result & 0xFFFFFFFF);
    
    systemRegisters.PSW.OV = (generalRegisters[30] == 0) ? 0 : 1;
    systemRegisters.PSW.S = (generalRegisters[instruction.reg2] < 0) ? 1 : 0;
    systemRegisters.PSW.Z = (generalRegisters[instruction.reg2] == 0) ? 1 : 0;
    
    programCounter += 2;
    cycles += 13;
}

//Compare Form I
inline void compare(const Instruction& instruction)
{
    int32_t a = generalRegisters[instruction.reg1];
    int32_t b = generalRegisters[instruction.reg2];
    
    d_printf("CMP: GR[%d](0x%X) and GR[%d](0x%X)\n", instruction.reg1, a, instruction.reg2, b);
    uint32_t result = b-a;

    
    systemRegisters.PSW.Z = (result == 0) ? 1 : 0;
    systemRegisters.PSW.S = ((int32_t)result < 0) ? 1 : 0;
    systemRegisters.PSW.OV = calculate_overflow_subtract((int32_t)a, (int32_t)b);
    systemRegisters.PSW.CY = (result > b);
    programCounter += 2;
    cycles += 1;
}

//Compare Form II
inline void compareImmediate(const Instruction &instruction)
{
    int32_t a = sign_extend(5, instruction.reg1);
    int32_t b = generalRegisters[instruction.reg2];
    
    d_printf("CMP: 0x%X and GR[%d](0x%X)\n", a, instruction.reg2, b);
    uint32_t result = b-a;
    
    
    systemRegisters.PSW.Z = (result == 0) ? 1 : 0;
    systemRegisters.PSW.S = ((int32_t)result < 0) ? 1 : 0;
    systemRegisters.PSW.OV = calculate_overflow_subtract((int32_t)a, (int32_t)b);
    systemRegisters.PSW.CY = (result > b);
    programCounter += 2;

    cycles += 1;
}

//Set Flag Condition
inline void setFlag(const Instruction &instruction)
{
    d_printf("SETF: GR[%d] <- Condition (0x%X)\n", instruction.reg2, instruction.reg1);
    bool conditionMet = false;
    switch ((FlagCondition)instruction.reg1) {
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
    
    generalRegisters[instruction.reg2] = conditionMet;
    programCounter += 2;
    cycles += 1;
}

//Shift Left Form I
inline void shiftLeft(const Instruction& instruction)
{
    d_printf("SHL: GR[%d] <- GR[%d](0x%X) << GR[%d](0x%X)\n", instruction.reg2, instruction.reg2, generalRegisters[instruction.reg2], instruction.reg1, generalRegisters[instruction.reg1]);
    int32_t s = generalRegisters[instruction.reg1];
    int32_t a = generalRegisters[instruction.reg2];
    int32_t result = a << s;
    generalRegisters[instruction.reg2] = result;
    
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.CY = instruction.reg1 ? (a >> (32-s)) & 1 : 0;
    systemRegisters.PSW.S = result < 0;
    systemRegisters.PSW.Z = result == 0;
    programCounter += 2;
    cycles += 1;
}

//Shift Left Form II
inline void shiftLeftImmediate(const Instruction& instruction)
{
    d_printf("SHL: GR[%d] <- GR[%d](0x%X) << 0x%X\n", instruction.reg2, instruction.reg2, generalRegisters[instruction.reg2], instruction.reg1);
    int32_t s = instruction.reg1;
    int32_t a = generalRegisters[instruction.reg2];
    int32_t result = a << s;
    generalRegisters[instruction.reg2] = result;
    
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.CY = s ? (a >> (32-s)) & 1 : 0;
    systemRegisters.PSW.S = result < 0;
    systemRegisters.PSW.Z = result == 0;
    programCounter += 2;
    cycles += 1;
}

//Shift Right Form I
inline void shiftRight(const Instruction& instruction)
{
    d_printf("SHR: GR[%d] <- GR[%d](0x%X) >> GR[%d](0x%X)\n", instruction.reg2, instruction.reg2, generalRegisters[instruction.reg2], instruction.reg1, generalRegisters[instruction.reg1]);
    uint32_t s = generalRegisters[instruction.reg1];
    uint32_t a = generalRegisters[instruction.reg2];
    int32_t result = a >> s;
    generalRegisters[instruction.reg2] = result;
    
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.CY = instruction.reg1 ? (a >> (a-1)) & 1 : 0;
    systemRegisters.PSW.S = result < 0;
    systemRegisters.PSW.Z = result == 0;
    programCounter += 2;
    cycles += 1;
}

//Shift Right Form II
inline void shiftRightImmediate(const Instruction &instruction)
{
    d_printf("SHR: GR[%d] <- GR[%d](0x%X) >> 0x%X\n", instruction.reg2, instruction.reg2, generalRegisters[instruction.reg2], instruction.reg1);
    uint32_t s = instruction.reg1;
    uint32_t a = generalRegisters[instruction.reg2];
    int32_t result = a >> s;
    generalRegisters[instruction.reg2] = result;
    
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.CY = instruction.reg1 ? (a >> (a-1)) & 1 : 0;
    systemRegisters.PSW.S = result < 0;
    systemRegisters.PSW.Z = result == 0;
    programCounter += 2;
    cycles += 1;
}

//Shift Arithmetic Right (signed shift)
inline void shiftArithmeticRight(const Instruction& instruction)
{
    d_printf("SAR: GR[%d] <- GR[%d](0x%X) >>>  GR[%d](0x%X)\n", instruction.reg2, instruction.reg2, generalRegisters[instruction.reg2], instruction.reg1, generalRegisters[instruction.reg1]);
    int32_t s = generalRegisters[instruction.reg1];
    int32_t a = generalRegisters[instruction.reg2];
    int32_t result = a >> s;
    generalRegisters[instruction.reg2] = result;
    
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.CY = instruction.reg1 ? (a >> (a-1)) & 1 : 0;
    systemRegisters.PSW.S = result < 0;
    systemRegisters.PSW.Z = result == 0;
    programCounter += 2;
    cycles += 1;
}

inline void shiftArithmeticRightImmediate(const Instruction& instruction)
{
    d_printf("SAR: GR[%d] <- GR[%d](0x%X) >>> 0x%X\n", instruction.reg2, instruction.reg2, generalRegisters[instruction.reg2], instruction.reg1);
    int32_t s = instruction.reg1;
    int32_t a = generalRegisters[instruction.reg2];
    int32_t result = a >> s;
    generalRegisters[instruction.reg2] = result;
    
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.CY = instruction.reg1 ? (a >> (a-1)) & 1 : 0;
    systemRegisters.PSW.S = result < 0;
    systemRegisters.PSW.Z = result == 0;
    programCounter += 2;  
    cycles += 1;
}

inline void logicalOr(const Instruction& instruction)
{
    d_printf("OR: GR[%d] <- GR[%d](0x%X) | GR[%d](0x%X)\n", instruction.reg2, instruction.reg2, generalRegisters[instruction.reg2], instruction.reg1, generalRegisters[instruction.reg1]);
    int32_t result = generalRegisters[instruction.reg2] | generalRegisters[instruction.reg1];
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.S = (result < 0) ? 1 : 0;
    systemRegisters.PSW.Z = (result == 0) ? 1 : 0;
    generalRegisters[instruction.reg2] = result;
    programCounter += 2;
    cycles += 1;
}

inline void logicalXor(const Instruction& instruction)
{
    d_printf("XOR: GR[%d] <- GR[%d](0x%X) ^ GR[%d](0x%X)\n", instruction.reg2, instruction.reg2, generalRegisters[instruction.reg2], instruction.reg1, generalRegisters[instruction.reg1]);
    int32_t result = generalRegisters[instruction.reg2] ^ generalRegisters[instruction.reg1];
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.S = (result < 0) ? 1 : 0;
    systemRegisters.PSW.Z = (result == 0) ? 1 : 0;
    generalRegisters[instruction.reg2] = result;
    programCounter += 2;
    cycles += 1;
}

inline void logicalAnd(const Instruction& instruction)
{
    d_printf("AND: GR[%d] <- GR[%d](0x%X) & GR[%d](0x%X)\n", instruction.reg2, instruction.reg2, generalRegisters[instruction.reg2], instruction.reg1, generalRegisters[instruction.reg1]);
    int32_t result = generalRegisters[instruction.reg2] & generalRegisters[instruction.reg1];
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.S = (result < 0) ? 1 : 0;
    systemRegisters.PSW.Z = (result == 0) ? 1 : 0;
    generalRegisters[instruction.reg2] = result;
    programCounter += 2;
    cycles += 1;
}

inline void logicalNot(const Instruction& instruction)
{
    d_printf("NOT: GR[%d] <- ~GR[%d](0x%X)\n", instruction.reg2, instruction.reg1, generalRegisters[instruction.reg1]);
    int32_t result = ~generalRegisters[instruction.reg1];
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.S = (result < 0) ? 1 : 0;
    systemRegisters.PSW.Z = (result == 0) ? 1 : 0;
    generalRegisters[instruction.reg2] = result;
    programCounter += 2;
    cycles += 1;
}

inline void andImmediate(const Instruction &instruction)
{
    d_printf("ANDI: GR[%d] <- GR[%d](0x%X) & 0x%X\n", instruction.reg2, instruction.reg1, generalRegisters[instruction.reg1], instruction.imm16);
    uint32_t res = generalRegisters[instruction.reg1] & instruction.imm16;
    generalRegisters[instruction.reg2] = res;
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.S = 0;
    systemRegisters.PSW.Z = (res == 0) ? 1 : 0;
    
    programCounter += 4;
    cycles += 1;
}

inline void orImmediate(const Instruction &instruction)
{
    d_printf("ORI: GR[%d] <- GR[%d](0x%X) | 0x%X\n", instruction.reg2, instruction.reg1, generalRegisters[instruction.reg1], instruction.imm16);
    generalRegisters[instruction.reg2] = generalRegisters[instruction.reg1] | instruction.imm16;
    
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.S = generalRegisters[instruction.reg2] < 0;
    systemRegisters.PSW.Z = (generalRegisters[instruction.reg2] == 0) ? 1 : 0;
    
    programCounter += 4;
    cycles += 1;
}

inline void xorImmediate(const Instruction &instruction)
{
    d_printf("XORI: GR[%d] <- GR[%d](0x%X) ^ 0x%X\n", instruction.reg2, instruction.reg1, generalRegisters[instruction.reg1], instruction.imm16);
    generalRegisters[instruction.reg2] = generalRegisters[instruction.reg1] ^ instruction.imm16;
    
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.S = generalRegisters[instruction.reg2] < 0;
    systemRegisters.PSW.Z = (generalRegisters[instruction.reg2] == 0) ? 1 : 0;
    
    programCounter += 4;
    cycles += 1;
}

inline void jump(const Instruction& instruction)
{
    d_printf("JMP: PC <- GR[%d](0x%X)\n", instruction.reg1, generalRegisters[instruction.reg1]);
    programCounter = (char*)&memoryManagmentUnit.read<uint32_t>(generalRegisters[instruction.reg1]);
    cycles += 3;
}

inline void jumpRelative(const Instruction &instruction)
{
    int32_t relativeJump = sign_extend(26, instruction.disp26());
    d_printf("JMP: PC <- PC + 0x%X\n", relativeJump);
    programCounter += relativeJump;
    cycles += 3;
}

inline void branchIfNoOverflow(const Instruction& instruction)
{
    d_printf("%s\n", "BNV");
    if (systemRegisters.PSW.OV == 0)
    {
        programCounter += sign_extend(9, instruction.disp9());
        cycles += 3;
    }
    else
    {
        programCounter += 2;
        cycles += 1;
    }
}

inline void branchIfOverflow(const Instruction& instruction)
{
    d_printf("%s\n", "BV");
    if (systemRegisters.PSW.OV)
    {
        programCounter += sign_extend(9, instruction.disp9());
        cycles += 3;
    }
    else
    {
        programCounter += 2;
        cycles += 1;
    }
}

inline void branchIfZero(const Instruction& instruction)
{
    d_printf("%s\n", "BZ");
    if (systemRegisters.PSW.Z)
    {
        programCounter += sign_extend(9, instruction.disp9());
        cycles += 3;
    }
    else
    {
        programCounter += 2;
        cycles += 1;
    }
}

inline void branchIfNotZero(const Instruction& instruction)
{
    d_printf("%s\n", "BNZ");
    if (systemRegisters.PSW.Z == 0)
    {
        programCounter += sign_extend(9, instruction.disp9());
        cycles += 3;
    }
    else
    {
        programCounter += 2;
        cycles += 1;
    }
}


inline void branchIfLessThan(const Instruction& instruction)
{
    d_printf("%s\n", "BLT");
    if ((systemRegisters.PSW.S ^ systemRegisters.PSW.OV) == 1)
    {
        programCounter += sign_extend(9, instruction.disp9());
        cycles += 3;
    }
    else
    {
        programCounter += 2;
        cycles += 1;
    }
}

inline void branchIfNotHigher(const Instruction& instruction)
{
    d_printf("%s\n", "BNH");
    if ((systemRegisters.PSW.Z | systemRegisters.PSW.CY) == 1)
    {
        programCounter += sign_extend(9, instruction.disp9());
        cycles += 3;
    }
    else
    {
        programCounter += 2;   
        cycles += 1;
    }
}

inline void branchIfNegative(const Instruction& instruction)
{
    d_printf("%s\n", "BN");
    if (systemRegisters.PSW.S)
    {
        programCounter += sign_extend(9, instruction.disp9());
        cycles += 3;
    }
    else
    {
        programCounter += 2;   
        cycles += 1;
    }
}

inline void branchIfPositive(const Instruction& instruction)
{
    d_printf("%s\n", "BP");
    if (systemRegisters.PSW.S == 0)
    {
        programCounter += sign_extend(9, instruction.disp9());
        cycles += 3;
    }
    else
    {
        programCounter += 2;   
        cycles += 1;
    }
}


inline void branchIfHigher(const Instruction& instruction)
{
    d_printf("%s\n", "BH");
    if ((systemRegisters.PSW.Z | systemRegisters.PSW.CY) == 0)
    {
        programCounter += sign_extend(9, instruction.disp9());
        cycles += 3;
    }
    else
    {
        programCounter += 2;   
        cycles += 1;
    }
}

inline void branchIfGreaterOrEqual(const Instruction& instruction)
{
    d_printf("BGE\n");
    if ((systemRegisters.PSW.S ^ systemRegisters.PSW.OV) == 0)
    {
        programCounter += sign_extend(9, instruction.disp9());
        cycles += 3;
    }
    else
    {
        programCounter += 2;   
        cycles += 1;
    }
}

inline void branchIfGreaterThan(const Instruction& instruction)
{
    d_printf("BGT\n");
    if (((systemRegisters.PSW.S ^ systemRegisters.PSW.OV) | systemRegisters.PSW.Z) == 0)
    {
        programCounter += sign_extend(9, instruction.disp9());
        cycles += 3;
    }
    else
    {
        programCounter += 2;   
        cycles += 1;
    }
}

inline void branchIfLessOrEqual(const Instruction& instruction)
{
    d_printf("BLE\n");
    if (((systemRegisters.PSW.S ^ systemRegisters.PSW.OV) | systemRegisters.PSW.Z) == 1)
    {
        programCounter += sign_extend(9, instruction.disp9());
        cycles += 3;
    }
    else
    {
        programCounter += 2;   
        cycles += 1;
    }
}

inline void branchIfCarry(const Instruction& instruction)
{
    d_printf("BC\n");
    if (systemRegisters.PSW.CY)
    {
        programCounter += sign_extend(9, instruction.disp9());
        cycles += 3;
    }
    else
    {
        programCounter += 2;   
        cycles += 1;
    }
}

inline void branchIfNoCarry(const Instruction& instruction)
{
    d_printf("BNC\n");
    if (systemRegisters.PSW.CY == 0)
    {
        programCounter += sign_extend(9, instruction.disp9());
        cycles += 3;
    }
    else
    {
        programCounter += 2;   
        cycles += 1;
    }
}

inline void branch(const Instruction& instruction)
{
    d_printf("BR\n");
    programCounter += sign_extend(9, instruction.disp9());
    cycles += 3;
}

inline void moveHigh(const Instruction& instruction)
{
    d_printf("MOVEHI: GR[%d] <- GR[%d](0x%X) + 0x%X\n", instruction.reg2, instruction.reg1, generalRegisters[instruction.reg1], (instruction.imm16 << 16));
    generalRegisters[instruction.reg2] = generalRegisters[instruction.reg1] + (instruction.imm16 << 16);
    programCounter += 4;
    cycles += 1;
}

inline void moveAddImmediate(const Instruction& instruction)
{
    d_printf("MOVEA: GR[%d] <- GR[%d](0x%X) + 0x%X\n", instruction.reg2, instruction.reg1, generalRegisters[instruction.reg1], sign_extend(16, instruction.imm16));
    generalRegisters[instruction.reg2] = generalRegisters[instruction.reg1] + sign_extend(16, instruction.imm16);
    programCounter += 4;
    cycles += 1;
}

inline void storeWord(const Instruction& instruction)
{
    uint32_t address = (generalRegisters[instruction.reg1] + (int16_t)instruction.disp16) & 0xFFFFFFFC;
    d_printf("ST.W: [GR[%d](0x%X) + %d](0x%X) <- [GR[%d](0x%X)\n", instruction.reg1, generalRegisters[instruction.reg1], sign_extend(16, instruction.disp16) & 0xFFFFFFFC, address, instruction.reg2, generalRegisters[instruction.reg2]);

    memoryManagmentUnit.store<uint32_t>(generalRegisters[instruction.reg2], address);
    programCounter += 4;
    cycles += 1;
}

inline void storeHWord(const Instruction& instruction)
{
    uint32_t address = (generalRegisters[instruction.reg1] + (int16_t)instruction.disp16) & 0xFFFFFFFE;
    d_printf("ST.H: [GR[%d](0x%X) + %d](0x%X) <- [GR[%d](0x%X)\n", instruction.reg1, generalRegisters[instruction.reg1], sign_extend(16, instruction.disp16) & 0xFFFFFFFE, address, instruction.reg2, generalRegisters[instruction.reg2]);

    
    memoryManagmentUnit.store<uint16_t>(generalRegisters[instruction.reg2] & 0xFFFF, address);
    programCounter += 4;
    cycles += 1;
}

inline void storeByte(const Instruction& instruction)
{
    uint32_t address = (generalRegisters[instruction.reg1] + (int16_t)instruction.disp16);
    d_printf("ST.B: [GR[%d](0x%X) + %d](0x%X) <- [GR[%d](0x%X)\n", instruction.reg1, generalRegisters[instruction.reg1], sign_extend(16, instruction.disp16), address, instruction.reg2, generalRegisters[instruction.reg2]);


    memoryManagmentUnit.store<uint8_t>(generalRegisters[instruction.reg2] & 0xFF, address);
    programCounter += 4;
    cycles += 1;
}

inline void loadByte(const Instruction& instruction)
{
    d_printf("LD.B: GR[%d] <- [GR[%d](0x%X) + %d\n", instruction.reg2, instruction.reg1, generalRegisters[instruction.reg1], sign_extend(16, instruction.disp16));

    uint32_t address = (generalRegisters[instruction.reg1] + (int16_t)instruction.disp16);
    generalRegisters[instruction.reg2] = sign_extend(8, memoryManagmentUnit.read<int8_t>(address));
    programCounter += 4;    
    cycles += 3;
}

inline void loadHWord(const Instruction& instruction)
{
    uint32_t address = (generalRegisters[instruction.reg1] + (int16_t)instruction.disp16);
    address &= 0xFFFFFFFE;
    d_printf("LD.H: GR[%d] <- [GR[%d](0x%X) + %d\n", instruction.reg2, instruction.reg1, generalRegisters[instruction.reg1], sign_extend(16, instruction.disp16));
    
    generalRegisters[instruction.reg2] = sign_extend(16, memoryManagmentUnit.read<int16_t>(address));
    programCounter += 4;    
    cycles += 3;
}

inline void loadWord(const Instruction& instruction)
{
    uint32_t address = (generalRegisters[instruction.reg1] + (int16_t)instruction.disp16) & 0xFFFFFFFC;
    d_printf("LD.W: GR[%d] <- [GR[%d](0x%X) + %d\n", instruction.reg2, instruction.reg1, generalRegisters[instruction.reg1], sign_extend(16, instruction.disp16));
    
    generalRegisters[instruction.reg2] = memoryManagmentUnit.read<int32_t>(address);
    programCounter += 4;    
    cycles += 3;
}

inline void loadSystemRegister(const CPU::Instruction& instruction)
{
    d_printf("LDSR\n");
    //TODO: Prevent writing to write disabled registers
    uint8_t regID = instruction.reg1;
    systemRegisters[regID] = generalRegisters[instruction.reg2];
    programCounter += 2;
    cycles += 1;
}

inline void storeSystemRegister(const CPU::Instruction& instruction)
{
    d_printf("STSR\n");
    //TODO: Prevent reading from reserved registers
    uint8_t regID = instruction.reg1;
    generalRegisters[instruction.reg2] = systemRegisters[regID];
    programCounter += 2;   
    cycles += 1;
}

inline void jumpAndLink(const Instruction &instruction)
{
    d_printf("JAL\n");
    generalRegisters[31] = (0x07000000 + (uint32_t)(((char*)programCounter + 4) - ((char*)memoryManagmentUnit.rom.data)));
    int32_t relativeJump = sign_extend(26, instruction.disp26());
    programCounter += relativeJump;
    cycles += 3;
}

inline void outWrite(const Instruction& instruction)
{
    d_printf("OUT.W\n");
    programCounter += 4;
}

inline void nop(const Instruction& instruction)
{
    d_printf("NOP\n");
    programCounter += 2;
    cycles += 1;
}

inline void setInterruptDisable(const Instruction& instruction)
{
    d_printf("SEI\n");
    systemRegisters.PSW.ID = 1;
    programCounter += 2;
    cycles += 1;
}

inline void clearInterruptDisable(const Instruction& instruction)
{
    d_printf("CLI\n");
    systemRegisters.PSW.ID = 0;
    programCounter += 2;
    cycles += 1;
}

inline void returnFromTrap(const Instruction& instruction)
{
    d_printf("RETI\n");
    if (systemRegisters.PSW.NP)
    {
        programCounter = (char*)&memoryManagmentUnit.read<uint32_t>(systemRegisters.FEPC);
        systemRegisters.PSW = systemRegisters.FEPSW;
    }
    else
    {
        programCounter = (char*)&memoryManagmentUnit.read<uint32_t>(systemRegisters.EIPC);
        systemRegisters.PSW = systemRegisters.EIPSW;            
    }
    cycles += 10;
}

inline void compareFloat(const Instruction& instruction)
{
    d_printf("CMPF.S\n");
    float f = generalRegistersFloat[instruction.reg2] - generalRegistersFloat[instruction.reg1];
    systemRegisters.PSW.Z = (f == 0.0f);
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.S = (f < 0.0f);
    systemRegisters.PSW.CY = (f < 0.0f);  
    programCounter += 4;
    cycles += 7;
//        systemRegisters.PSW.FRO = 
}

inline void convertWordToFloat(const Instruction& instruction)
{
    d_printf("CVT.WS\n");
    float f = generalRegisters[instruction.reg1];
    generalRegistersFloat[instruction.reg2] = f;
    
    systemRegisters.PSW.Z = (f == 0.0f);
    systemRegisters.PSW.S = (f < 0.0f);
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.CY = (f < 0.0f); 
    
    programCounter += 4;
    cycles += 5;
}

inline void convertFloatToWord(const Instruction& instruction)
{
    d_printf("CVT.SW\n");
    float f = generalRegistersFloat[instruction.reg1];
    int32_t t = 0;
    if (f >= 0)
        t = floorf(f + .5f);
    else
        t = ceilf(f - .5f);
    
    generalRegisters[instruction.reg2] = t;
    
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.S = (t < 0);
    systemRegisters.PSW.Z = (t == 0);
    
    programCounter += 4;
    cycles += 9;
}

inline void addFloat(const Instruction &instruction)
{
    d_printf("ADDF.S\n");
    
    float f = generalRegistersFloat[instruction.reg2] + generalRegistersFloat[instruction.reg1];
    
    generalRegistersFloat[instruction.reg2] = f;
    
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.CY = (f < 0.0f);
    systemRegisters.PSW.S = (f < 0.0f);
    systemRegisters.PSW.Z = (f == 0.0f);
    programCounter += 4;
    cycles += 9;
}

inline void subtractFloat(const Instruction &instruction)
{
    d_printf("SUBF.S\n");
    
    float f = generalRegistersFloat[instruction.reg2] - generalRegistersFloat[instruction.reg1];
    
    generalRegistersFloat[instruction.reg2] = f;
    
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.CY = (f < 0.0f);
    systemRegisters.PSW.S = (f < 0.0f);
    systemRegisters.PSW.Z = (f == 0.0f);
    programCounter += 4;
    cycles += 12;
}

inline void multiplyFloat(const Instruction &instruction)
{
    d_printf("MULF.S\n");
    
    float f = generalRegistersFloat[instruction.reg2] * generalRegistersFloat[instruction.reg1];
    
    generalRegistersFloat[instruction.reg2] = f;
    
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.CY = (f < 0.0f);
    systemRegisters.PSW.S = (f < 0.0f);
    systemRegisters.PSW.Z = (f == 0.0f);
    programCounter += 4;
    cycles += 8;
}

inline void divideFloat(const Instruction &instruction)
{
    d_printf("DIVF.S\n");
    
    float f = generalRegistersFloat[instruction.reg2] / generalRegistersFloat[instruction.reg1];
    
    generalRegistersFloat[instruction.reg2] = f;
    
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.CY = (f < 0.0f);
    systemRegisters.PSW.S = (f < 0.0f);
    systemRegisters.PSW.Z = (f == 0.0f);
    programCounter += 4;
    cycles += 44;
}

inline void truncateFloat(const Instruction& instruction)
{
    d_printf("TRNC.SW\n");
    float f = generalRegistersFloat[instruction.reg1];
    int32_t t = 0;
    if (f >= 0)
        t = floorf(f);
    else
        t = ceilf(f);
    
    generalRegisters[instruction.reg2] = t;
    
    systemRegisters.PSW.OV = 0;
    systemRegisters.PSW.S = (t < 0);
    systemRegisters.PSW.Z = (t == 0);
    
    programCounter += 4;
    cycles += 8;
}

inline void exchangeByte(const Instruction& instruction)
{
    d_printf("XB\n");
    
    int32_t val = generalRegisters[instruction.reg2];
    val = (val & 0xFFFF0000) | ((val << 8) & 0x0000FF00) | ((val >> 8) & 0x000000FF);
    generalRegisters[instruction.reg2] = val;
    programCounter += 4;
    cycles += 1;
}

inline void exchangeHalfWord(const Instruction &instruction)
{
    d_printf("XH\n");
    int32_t val = generalRegisters[instruction.reg2];
    val = ((val << 16) & 0xFFFF0000) | ((val >> 16) & 0x0000FFFF);
    generalRegisters[instruction.reg2] = val;
    programCounter += 4;
    cycles += 1;
}

inline void reverseWord(const Instruction &instruction)
{
    d_printf("REV\n");
    int32_t word = generalRegisters[instruction.reg1];
    int32_t out = 0;
    for (int i = 0; i < 32; ++i)
    {
        out |= (word & 1);
        out = out << 1;
        word = word >> 1;
    }
    
    generalRegisters[instruction.reg2] = word;
    programCounter += 4;
    cycles += 1;
}

inline void multiplyHalfWord(const Instruction &instruction)
{
    d_printf("MPYHW\n");
    
    generalRegisters[instruction.reg2] = (int32_t)(int16_t)(generalRegisters[instruction.reg2] & 0xFFFF) * (int32_t)(int16_t)(generalRegisters[instruction.reg1] & 0xFFFF);
    
    programCounter += 4;
    cycles += 9;
}

inline void moveBitString(const Instruction& instruction)
{
    d_printf("MOVBSU\n");
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
        dest.SetNext(Mover(), data, bits);
    }
    
    assert(!source.HasData() && !dest.HasData());
    
    programCounter += 4;
}

inline void andBitString(const Instruction& instruction)
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
    
    programCounter += 4;
}

inline void orBitString(const Instruction& instruction)
{
    d_printf("ORBSU\n");
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
        dest.SetNext(Orer(), data, bits);
    }
    
    generalRegisters[30] = source.currentLocation;
    generalRegisters[29] = dest.currentLocation;
    generalRegisters[28] = source.stringLength;
    generalRegisters[27] = source.offset;
    generalRegisters[26] = dest.offset;
    
    assert(!source.HasData() && !dest.HasData());
    
    programCounter += 4;
}

inline void xorBitString(const Instruction& instruction)
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
    
    programCounter += 4;
}

inline void andNotBitString(const Instruction& instruction)
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
    
    programCounter += 4;
}

inline void orNotBitString(const Instruction& instruction)
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
    
    programCounter += 4;
}

inline void xorNotBitString(const Instruction& instruction)
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
    
    assert(!source.HasData() && !dest.HasData());
    
    programCounter += 4;
}

inline void notBitString(const Instruction& instruction)
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
    
    programCounter += 4;
}


#endif
