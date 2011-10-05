//
//  v810_instructions.cpp
//  virtualutopia
//

#include "v810.h"
#include "instruction.h"
#include "cpu_utils.h"
#include "mmu.h"
#include "bitstring.h"
#include <cmath>

//#define d_printf(A, ...)
#define d_printf(A, ...) do{ if(debugOutput) { printf("%X: " A, programCounter, ##__VA_ARGS__); } }while(0)

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

namespace CPU
{
    //Move Form I
    void v810::move(const Instruction& instruction)
    {
        d_printf("MOV: GR[%d] <- GR[%d](0x%X)\n", instruction.reg2(), instruction.reg1(), generalRegisters[instruction.reg1()]);
        generalRegisters[instruction.reg2()] = generalRegisters[instruction.reg1()];
        programCounter += 2;
        cycles += 1;
    }
    
    //Move Form II
    void v810::moveImmediate(const Instruction& instruction)
    {
        d_printf("MOV: GR[%d] <- GR[%d](0x%X)\n", instruction.reg2(), instruction.reg1(), sign_extend(5, generalRegisters[instruction.imm5()]));
        generalRegisters[instruction.reg2()] = sign_extend(5, generalRegisters[instruction.imm5()]);
        programCounter += 2;
        cycles += 1;
    }
    
    //Add Form I
    void v810::add(const Instruction& instruction)
    {
        d_printf("ADD: GR[%d] <- GR[%d](0x%X) + GR[%d](0x%X)\n", instruction.reg2(), instruction.reg2(), generalRegisters[instruction.reg2()], instruction.reg1(), generalRegisters[instruction.reg1()]);
        int32_t a = generalRegisters[instruction.reg1()];
        int32_t b = generalRegisters[instruction.reg2()];
        int64_t r = a+b;
        int32_t truncatedResult = (int32_t)r;
        generalRegisters[instruction.reg2()] = truncatedResult;           
        
        systemRegisters.PSW.OV = (sign(a) == sign(b)) ? (sign(truncatedResult) != sign(a)) : 0;
        systemRegisters.PSW.CY = r > INT32_MAX || r < INT32_MIN;
        systemRegisters.PSW.S = truncatedResult < 0;
        systemRegisters.PSW.Z = truncatedResult == 0;

        programCounter += 2;
        cycles += 1;
    }
    
    //Add Form II
    void v810::addImmediate5(const Instruction& instruction)
    {
        d_printf("ADD: GR[%d] <- GR[%d](0x%X) + 0x%X\n", instruction.reg2(), instruction.reg2(), generalRegisters[instruction.reg2()], sign_extend(5, instruction.imm5()));
        int32_t a = sign_extend(5, instruction.imm5());
        int32_t b = generalRegisters[instruction.reg2()];
        int64_t r = a+b;
        int32_t truncatedResult = (int32_t)r;
        generalRegisters[instruction.reg2()] = truncatedResult;           
        
        systemRegisters.PSW.OV = (sign(a) == sign(b)) ? (sign(truncatedResult) != sign(a)) : 0;
        systemRegisters.PSW.CY = r > INT32_MAX || r < INT32_MIN;
        systemRegisters.PSW.S = truncatedResult < 0;
        systemRegisters.PSW.Z = truncatedResult == 0;
        
        programCounter += 2;
        cycles += 1;
    }
    
    
    void v810::addImmediate(const Instruction& instruction)
    {
        d_printf("ADDI: GR[%d] <- GR[%d](0x%X) + 0x%X\n", instruction.reg2(), instruction.reg1(), generalRegisters[instruction.reg1()], sign_extend(16, instruction.imm16()));
        
        int32_t a = sign_extend(16, instruction.imm16());
        int32_t b = generalRegisters[instruction.reg2()];
        int64_t r = a+b;
        int32_t truncatedResult = (int32_t)r;
        generalRegisters[instruction.reg2()] = truncatedResult;           
        
        systemRegisters.PSW.OV = (sign(a) == sign(b)) ? (sign(truncatedResult) != sign(a)) : 0;
        systemRegisters.PSW.CY = r > INT32_MAX || r < INT32_MIN;
        systemRegisters.PSW.S = truncatedResult < 0;
        systemRegisters.PSW.Z = truncatedResult == 0;
        programCounter += 4;
        cycles += 1;
    }
    
    
    //Subtract
    void v810::subtract(const Instruction& instruction)
    {
        d_printf("SUB: GR[%d] <- GR[%d](0x%X) - GR[%d](0x%X)\n", instruction.reg2(), instruction.reg2(), generalRegisters[instruction.reg2()], instruction.reg1(), generalRegisters[instruction.reg1()]);
        int32_t a = generalRegisters[instruction.reg1()];
        int32_t b = generalRegisters[instruction.reg2()];
        int64_t r = b-a;
        int32_t truncatedResult = (int32_t)r;
        generalRegisters[instruction.reg2()] = truncatedResult;           
        
        systemRegisters.PSW.OV = calculate_overflow_subtract(a, b);
        systemRegisters.PSW.CY = r > INT32_MAX || r < INT32_MIN;
        systemRegisters.PSW.S = truncatedResult < 0;
        systemRegisters.PSW.Z = truncatedResult == 0;
        
        programCounter += 2;
        cycles += 1;
    }

    void v810::divide(const Instruction& instruction)
    {
        d_printf("DIV: GR[%d] <- GR[%d](0x%X) / GR[%d](0x%X)\n", instruction.reg2(), instruction.reg2(), generalRegisters[instruction.reg2()], instruction.reg1(), generalRegisters[instruction.reg1()]);
        int32_t a = generalRegisters[instruction.reg1()];
        int32_t b = generalRegisters[instruction.reg2()];
        
        if (a == 0)
        {
            throwException(DIV0);
            return;
        }
        
        int32_t result = b / a;
        
        generalRegisters[30] = b % a;
        generalRegisters[instruction.reg2()] = result;
        
        systemRegisters.PSW.OV = (b == 0x80000000 && a == 0xFFFFFFF) ? 1 : 0;
        systemRegisters.PSW.S = (generalRegisters[instruction.reg2()] < 0) ? 1 : 0;
        systemRegisters.PSW.Z = (generalRegisters[instruction.reg2()] == 0) ? 1 : 0;
        
        programCounter += 2;
        cycles += 38;
    }
    
    void v810::divideUnsigned(const Instruction& instruction)
    {
        d_printf("DIVU: GR[%d] <- GR[%d](0x%X) / GR[%d](0x%X)\n", instruction.reg2(), instruction.reg2(), generalRegisters[instruction.reg2()], instruction.reg1(), generalRegisters[instruction.reg1()]);
        uint32_t a = generalRegisters[instruction.reg1()];
        int32_t b = generalRegisters[instruction.reg2()];
        
        if (a == 0)
        {
            throwException(DIV0);
            return;
        }
        
        int32_t result = b / a;
        
        generalRegisters[30] = b % a;
        generalRegisters[instruction.reg2()] = result;
        
        systemRegisters.PSW.OV = 0;
        systemRegisters.PSW.S = (generalRegisters[instruction.reg2()] < 0) ? 1 : 0;
        systemRegisters.PSW.Z = (generalRegisters[instruction.reg2()] == 0) ? 1 : 0;
        
        programCounter += 2;
        cycles += 36;
    }
    
    void v810::multiply(const Instruction& instruction)
    {
        d_printf("MUL: GR[%d] <- GR[%d](0x%X) * GR[%d](0x%X)\n", instruction.reg2(), instruction.reg2(), generalRegisters[instruction.reg2()], instruction.reg1(), generalRegisters[instruction.reg1()]);
        int32_t a = generalRegisters[instruction.reg1()];
        int32_t b = generalRegisters[instruction.reg2()];
        int64_t result = b * a;
        
        generalRegisters[30] = (int32_t)(result >> 32);
        generalRegisters[instruction.reg2()] = (int32_t)(result & 0xFFFFFFFF);
        
        systemRegisters.PSW.OV = (generalRegisters[30] == 0) ? 0 : 1;
        systemRegisters.PSW.S = (generalRegisters[instruction.reg2()] < 0) ? 1 : 0;
        systemRegisters.PSW.Z = (generalRegisters[instruction.reg2()] == 0) ? 1 : 0;
        
        programCounter += 2;
        cycles += 13;
    }
    
    void v810::multiplyUnsigned(const Instruction& instruction)
    {
        d_printf("MULU: GR[%d] <- GR[%d](0x%X) * GR[%d](0x%X)\n", instruction.reg2(), instruction.reg2(), generalRegisters[instruction.reg2()], instruction.reg1(), generalRegisters[instruction.reg1()]);
        uint32_t a = generalRegisters[instruction.reg1()];
        uint32_t b = generalRegisters[instruction.reg2()];
        uint64_t result = b * a;
        
        generalRegisters[30] = (int32_t)(result >> 32);
        generalRegisters[instruction.reg2()] = (int32_t)(result & 0xFFFFFFFF);
        
        systemRegisters.PSW.OV = (generalRegisters[30] == 0) ? 0 : 1;
        systemRegisters.PSW.S = (generalRegisters[instruction.reg2()] < 0) ? 1 : 0;
        systemRegisters.PSW.Z = (generalRegisters[instruction.reg2()] == 0) ? 1 : 0;
        
        programCounter += 2;
        cycles += 13;
    }
    
    //Compare Form I
    void v810::compare(const Instruction& instruction)
    {
        int32_t a = generalRegisters[instruction.reg1()];
        int32_t b = generalRegisters[instruction.reg2()];
        
        d_printf("CMP: GR[%d](0x%X) and GR[%d](0x%X)\n", instruction.reg1(), a, instruction.reg2(), b);
        int64_t result = b-a;
        int32_t truncatedResult = (int32_t)result;
        
        systemRegisters.PSW.Z = (truncatedResult == 0) ? 1 : 0;
        systemRegisters.PSW.S = (truncatedResult < 0) ? 1 : 0;
        systemRegisters.PSW.OV = calculate_overflow_subtract(a, b);
        systemRegisters.PSW.CY = result < INT32_MIN || result > INT32_MAX;
        programCounter += 2;
        cycles += 1;
    }
    
    //Compare Form II
    void v810::compareImmediate(const Instruction &instruction)
    {
        int32_t a = sign_extend(5, instruction.imm5());
        int32_t b = generalRegisters[instruction.reg2()];
        
        d_printf("CMP: 0x%X and GR[%d](0x%X)\n", a, instruction.reg2(), b);
        int64_t result = b-a;
        int32_t truncatedResult = (int32_t)result;
        
        systemRegisters.PSW.Z = (truncatedResult == 0) ? 1 : 0;
        systemRegisters.PSW.S = (truncatedResult < 0) ? 1 : 0;
        systemRegisters.PSW.OV = calculate_overflow_subtract(a, b);
        systemRegisters.PSW.CY = result < INT32_MIN || result > INT32_MAX;
        programCounter += 2;
        cycles += 1;
    }
    
    //Set Flag Condition
    void v810::setFlag(const Instruction &instruction)
    {
        d_printf("SETF: GR[%d] <- Condition (0x%X)\n", instruction.reg2(), instruction.imm5());
        bool conditionMet = false;
        switch ((FlagCondition)instruction.imm5()) {
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
        
        generalRegisters[instruction.reg2()] = conditionMet;
        programCounter += 2;
        cycles += 1;
    }
  
    //Shift Left Form I
    void v810::shiftLeft(const Instruction& instruction)
    {
        d_printf("SHL: GR[%d] <- GR[%d](0x%X) << GR[%d](0x%X)\n", instruction.reg2(), instruction.reg2(), generalRegisters[instruction.reg2()], instruction.reg1(), generalRegisters[instruction.reg1()]);
        int32_t s = generalRegisters[instruction.reg1()];
        int32_t a = generalRegisters[instruction.reg2()];
        int32_t result = a << s;
        generalRegisters[instruction.reg2()] = result;
        
        systemRegisters.PSW.OV = 0;
        systemRegisters.PSW.CY = instruction.imm5() ? (a >> (32-s)) & 1 : 0;
        systemRegisters.PSW.S = result < 0;
        systemRegisters.PSW.Z = result == 0;
        programCounter += 2;
        cycles += 1;
    }
    
    //Shift Left Form II
    void v810::shiftLeftImmediate(const Instruction& instruction)
    {
        d_printf("SHL: GR[%d] <- GR[%d](0x%X) << 0x%X\n", instruction.reg2(), instruction.reg2(), generalRegisters[instruction.reg2()], instruction.imm5());
        int32_t s = instruction.imm5();
        int32_t a = generalRegisters[instruction.reg2()];
        int32_t result = a << s;
        generalRegisters[instruction.reg2()] = result;
        
        systemRegisters.PSW.OV = 0;
        systemRegisters.PSW.CY = instruction.imm5() ? (a >> (32-s)) & 1 : 0;
        systemRegisters.PSW.S = result < 0;
        systemRegisters.PSW.Z = result == 0;
        programCounter += 2;
        cycles += 1;
    }
    
    //Shift Right Form I
    void v810::shiftRight(const Instruction& instruction)
    {
        d_printf("SHR: GR[%d] <- GR[%d](0x%X) >> GR[%d](0x%X)\n", instruction.reg2(), instruction.reg2(), generalRegisters[instruction.reg2()], instruction.reg1(), generalRegisters[instruction.reg1()]);
        uint32_t s = generalRegisters[instruction.reg1()];
        uint32_t a = generalRegisters[instruction.reg2()];
        int32_t result = a >> s;
        generalRegisters[instruction.reg2()] = result;
        
        systemRegisters.PSW.OV = 0;
        systemRegisters.PSW.CY = instruction.imm5() ? (a >> (a-1)) & 1 : 0;
        systemRegisters.PSW.S = result < 0;
        systemRegisters.PSW.Z = result == 0;
        programCounter += 2;
        cycles += 1;
    }
    
    //Shift Right Form II
    void v810::shiftRightImmediate(const Instruction &instruction)
    {
        d_printf("SHR: GR[%d] <- GR[%d](0x%X) >> 0x%X\n", instruction.reg2(), instruction.reg2(), generalRegisters[instruction.reg2()], instruction.imm5());
        uint32_t s = instruction.imm5();
        uint32_t a = generalRegisters[instruction.reg2()];
        int32_t result = a >> s;
        generalRegisters[instruction.reg2()] = result;
        
        systemRegisters.PSW.OV = 0;
        systemRegisters.PSW.CY = instruction.imm5() ? (a >> (a-1)) & 1 : 0;
        systemRegisters.PSW.S = result < 0;
        systemRegisters.PSW.Z = result == 0;
        programCounter += 2;
        cycles += 1;
    }
    
    //Shift Arithmetic Right (signed shift)
    void v810::shiftArithmeticRight(const Instruction& instruction)
    {
        d_printf("SAR: GR[%d] <- GR[%d](0x%X) >>>  GR[%d](0x%X)\n", instruction.reg2(), instruction.reg2(), generalRegisters[instruction.reg2()], instruction.reg1(), generalRegisters[instruction.reg1()]);
        int32_t s = generalRegisters[instruction.reg1()];
        int32_t a = generalRegisters[instruction.reg2()];
        int32_t result = a >> s;
        generalRegisters[instruction.reg2()] = result;
        
        systemRegisters.PSW.OV = 0;
        systemRegisters.PSW.CY = instruction.imm5() ? (a >> (a-1)) & 1 : 0;
        systemRegisters.PSW.S = result < 0;
        systemRegisters.PSW.Z = result == 0;
        programCounter += 2;
        cycles += 1;
    }
    
    void v810::shiftArithmeticRightImmediate(const Instruction& instruction)
    {
        d_printf("SAR: GR[%d] <- GR[%d](0x%X) >>> 0x%X\n", instruction.reg2(), instruction.reg2(), generalRegisters[instruction.reg2()], instruction.imm5());
        int32_t s = instruction.imm5();
        int32_t a = generalRegisters[instruction.reg2()];
        int32_t result = a >> s;
        generalRegisters[instruction.reg2()] = result;
        
        systemRegisters.PSW.OV = 0;
        systemRegisters.PSW.CY = instruction.imm5() ? (a >> (a-1)) & 1 : 0;
        systemRegisters.PSW.S = result < 0;
        systemRegisters.PSW.Z = result == 0;
        programCounter += 2;  
        cycles += 1;
    }
    
    void v810::logicalOr(const Instruction& instruction)
    {
        d_printf("OR: GR[%d] <- GR[%d](0x%X) | GR[%d](0x%X)\n", instruction.reg2(), instruction.reg2(), generalRegisters[instruction.reg2()], instruction.reg1(), generalRegisters[instruction.reg1()]);
        int32_t result = generalRegisters[instruction.reg2()] | generalRegisters[instruction.reg1()];
        systemRegisters.PSW.OV = 0;
        systemRegisters.PSW.S = (result < 0) ? 1 : 0;
        systemRegisters.PSW.Z = (result == 0) ? 1 : 0;
        generalRegisters[instruction.reg2()] = result;
        programCounter += 2;
        cycles += 1;
    }
    
    void v810::logicalXor(const Instruction& instruction)
    {
        d_printf("XOR: GR[%d] <- GR[%d](0x%X) ^ GR[%d](0x%X)\n", instruction.reg2(), instruction.reg2(), generalRegisters[instruction.reg2()], instruction.reg1(), generalRegisters[instruction.reg1()]);
        int32_t result = generalRegisters[instruction.reg2()] ^ generalRegisters[instruction.reg1()];
        systemRegisters.PSW.OV = 0;
        systemRegisters.PSW.S = (result < 0) ? 1 : 0;
        systemRegisters.PSW.Z = (result == 0) ? 1 : 0;
        generalRegisters[instruction.reg2()] = result;
        programCounter += 2;
        cycles += 1;
    }
    
    void v810::logicalAnd(const Instruction& instruction)
    {
        d_printf("AND: GR[%d] <- GR[%d](0x%X) & GR[%d](0x%X)\n", instruction.reg2(), instruction.reg2(), generalRegisters[instruction.reg2()], instruction.reg1(), generalRegisters[instruction.reg1()]);
        int32_t result = generalRegisters[instruction.reg2()] ^ generalRegisters[instruction.reg1()];
        systemRegisters.PSW.OV = 0;
        systemRegisters.PSW.S = (result < 0) ? 1 : 0;
        systemRegisters.PSW.Z = (result == 0) ? 1 : 0;
        generalRegisters[instruction.reg2()] = result;
        programCounter += 2;
        cycles += 1;
    }
    
    void v810::logicalNot(const Instruction& instruction)
    {
        d_printf("NOT: GR[%d] <- ~GR[%d](0x%X)\n", instruction.reg2(), instruction.reg1(), generalRegisters[instruction.reg1()]);
        int32_t result = ~generalRegisters[instruction.reg1()];
        systemRegisters.PSW.OV = 0;
        systemRegisters.PSW.S = (result < 0) ? 1 : 0;
        systemRegisters.PSW.Z = (result == 0) ? 1 : 0;
        generalRegisters[instruction.reg2()] = result;
        programCounter += 2;
        cycles += 1;
    }
    
    void v810::andImmediate(const Instruction &instruction)
    {
        d_printf("ANDI: GR[%d] <- GR[%d](0x%X) & 0x%X\n", instruction.reg2(), instruction.reg1(), generalRegisters[instruction.reg1()], instruction.imm16());
        generalRegisters[instruction.reg2()] = generalRegisters[instruction.reg1()] & instruction.imm16();
        
        systemRegisters.PSW.OV = 0;
        systemRegisters.PSW.S = 0;
        systemRegisters.PSW.Z = (generalRegisters[instruction.reg2()] == 0) ? 1 : 0;
        
        programCounter += 4;
        cycles += 1;
    }
    
    void v810::orImmediate(const Instruction &instruction)
    {
        d_printf("ORI: GR[%d] <- GR[%d](0x%X) | 0x%X\n", instruction.reg2(), instruction.reg1(), generalRegisters[instruction.reg1()], instruction.imm16());
        generalRegisters[instruction.reg2()] = generalRegisters[instruction.reg1()] | instruction.imm16();
        
        systemRegisters.PSW.OV = 0;
        systemRegisters.PSW.S = generalRegisters[instruction.reg2()] < 0;
        systemRegisters.PSW.Z = (generalRegisters[instruction.reg2()] == 0) ? 1 : 0;
        
        programCounter += 4;
        cycles += 1;
    }
    
    void v810::xorImmediate(const Instruction &instruction)
    {
        d_printf("XORI: GR[%d] <- GR[%d](0x%X) ^ 0x%X\n", instruction.reg2(), instruction.reg1(), generalRegisters[instruction.reg1()], instruction.imm16());
        generalRegisters[instruction.reg2()] = generalRegisters[instruction.reg1()] ^ instruction.imm16();
        
        systemRegisters.PSW.OV = 0;
        systemRegisters.PSW.S = generalRegisters[instruction.reg2()] < 0;
        systemRegisters.PSW.Z = (generalRegisters[instruction.reg2()] == 0) ? 1 : 0;
        
        programCounter += 4;
        cycles += 1;
    }
    
    void v810::jump(const Instruction& instruction)
    {
        d_printf("JMP: PC <- GR[%d](0x%X)\n", instruction.reg1(), generalRegisters[instruction.reg1()]);
        programCounter = generalRegisters[instruction.reg1()];
        cycles += 3;
    }
    
    void v810::jumpRelative(const Instruction &instruction)
    {
        int32_t relativeJump = sign_extend(26, instruction.disp26());
        d_printf("JMP: PC <- PC + 0x%X\n", relativeJump);
        programCounter += relativeJump;
        cycles += 3;
    }

    void v810::branchIfNoOverflow(const Instruction& instruction)
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
    
    void v810::branchIfOverflow(const Instruction& instruction)
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
    
    void v810::branchIfZero(const Instruction& instruction)
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
    
    void v810::branchIfNotZero(const Instruction& instruction)
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
    
    
    void v810::branchIfLessThan(const Instruction& instruction)
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

    void v810::branchIfNotHigher(const Instruction& instruction)
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
    
    void v810::branchIfNegative(const Instruction& instruction)
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
    
    void v810::branchIfPositive(const Instruction& instruction)
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
    
    
    void v810::branchIfHigher(const Instruction& instruction)
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
    
    void v810::branchIfGreaterOrEqual(const Instruction& instruction)
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

    void v810::branchIfGreaterThan(const Instruction& instruction)
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
    
    void v810::branchIfLessOrEqual(const Instruction& instruction)
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
    
    void v810::branchIfCarry(const Instruction& instruction)
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

    void v810::branchIfNoCarry(const Instruction& instruction)
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
    
    void v810::branch(const Instruction& instruction)
    {
        d_printf("BR\n");
        programCounter += sign_extend(9, instruction.disp9());
        cycles += 3;
    }
    
    void v810::moveHigh(const Instruction& instruction)
    {
        d_printf("MOVEHI: GR[%d] <- GR[%d](0x%X) + 0x%X\n", instruction.reg2(), instruction.reg1(), generalRegisters[instruction.reg1()], (instruction.imm16() << 16));
        generalRegisters[instruction.reg2()] = generalRegisters[instruction.reg1()] + (instruction.imm16() << 16);
        programCounter += 4;
        cycles += 1;
    }
    
    void v810::moveAddImmediate(const Instruction& instruction)
    {
        d_printf("MOVEA: GR[%d] <- GR[%d](0x%X) + 0x%X\n", instruction.reg2(), instruction.reg1(), generalRegisters[instruction.reg1()], sign_extend(16, instruction.imm16()));
        generalRegisters[instruction.reg2()] = generalRegisters[instruction.reg1()] + sign_extend(16, instruction.imm16());
        programCounter += 4;
        cycles += 1;
    }
    
    void v810::storeWord(const Instruction& instruction)
    {
        uint32_t address = (generalRegisters[instruction.reg1()] + sign_extend(16, instruction.disp16())) & 0xFFFFFFFC;
        d_printf("ST.W: [GR[%d](0x%X) + %d](0x%X) <- [GR[%d](0x%X)\n", instruction.reg1(), generalRegisters[instruction.reg1()], sign_extend(16, instruction.disp16()) & 0xFFFFFFFC, address, instruction.reg2(), generalRegisters[instruction.reg2()]);

        memoryManagmentUnit.StoreWord(address, generalRegisters[instruction.reg2()]);
        programCounter += 4;
    }
    
    void v810::storeHWord(const Instruction& instruction)
    {
        uint32_t address = (generalRegisters[instruction.reg1()] + sign_extend(16, instruction.disp16())) & 0xFFFFFFFE;
        d_printf("ST.H: [GR[%d](0x%X) + %d](0x%X) <- [GR[%d](0x%X)\n", instruction.reg1(), generalRegisters[instruction.reg1()], sign_extend(16, instruction.disp16()) & 0xFFFFFFFE, address, instruction.reg2(), generalRegisters[instruction.reg2()]);

        memoryManagmentUnit.StoreHWord(address, generalRegisters[instruction.reg2()] & 0xFFFF);
        programCounter += 4;
    }
    
    void v810::storeByte(const Instruction& instruction)
    {
        uint32_t address = (generalRegisters[instruction.reg1()] + sign_extend(16, instruction.disp16()));
        d_printf("ST.B: [GR[%d](0x%X) + %d](0x%X) <- [GR[%d](0x%X)\n", instruction.reg1(), generalRegisters[instruction.reg1()], sign_extend(16, instruction.disp16()), address, instruction.reg2(), generalRegisters[instruction.reg2()]);


        memoryManagmentUnit.StoreHWord(address, generalRegisters[instruction.reg2()] & 0xFF);
        programCounter += 4;
    }
    
    void v810::loadByte(const Instruction& instruction)
    {
        d_printf("LD.B: GR[%d] <- [GR[%d](0x%X) + %d\n", instruction.reg2(), instruction.reg1(), generalRegisters[instruction.reg1()], sign_extend(16, instruction.disp16()));

        uint32_t address = (generalRegisters[instruction.reg1()] + sign_extend(16, instruction.disp16()));
        generalRegisters[instruction.reg2()] = memoryManagmentUnit.GetData<int8_t>(address);
        programCounter += 4;    
    }
    
    void v810::loadHWord(const Instruction& instruction)
    {
        uint32_t address = (generalRegisters[instruction.reg1()] + sign_extend(16, instruction.disp16()));
        address &= 0xFFFFFFFE;
        d_printf("LD.H: GR[%d] <- [GR[%d](0x%X) + %d\n", instruction.reg2(), instruction.reg1(), generalRegisters[instruction.reg1()], sign_extend(16, instruction.disp16()));
        
        generalRegisters[instruction.reg2()] = memoryManagmentUnit.GetData<int16_t>(address);
        programCounter += 4;    
    }
    
    void v810::loadWord(const Instruction& instruction)
    {
        uint32_t address = (generalRegisters[instruction.reg1()] + sign_extend(16, instruction.disp16())) & 0xFFFFFFFC;
        d_printf("LD.W\n");
        generalRegisters[instruction.reg2()] = memoryManagmentUnit.GetData<int32_t>(address);
        programCounter += 4;    
    }
    
    void v810::loadSystemRegister(const CPU::Instruction& instruction)
    {
        d_printf("LDSR\n");
        //TODO: Prevent writing to write disabled registers
        uint8_t regID = instruction.reg1();
        systemRegisters[regID] = generalRegisters[instruction.reg2()];
        programCounter += 2;
    }

    void v810::storeSystemRegister(const CPU::Instruction& instruction)
    {
        d_printf("STSR\n");
        //TODO: Prevent reading from reserved registers
        uint8_t regID = instruction.reg1();
        generalRegisters[instruction.reg2()] = systemRegisters[regID];
        programCounter += 2;   
    }
    
    void v810::jumpAndLink(const Instruction &instruction)
    {
        d_printf("JAL\n");
        generalRegisters[31] = programCounter + 4;
        int32_t relativeJump = sign_extend(26, instruction.disp26());
        programCounter += relativeJump;
        cycles += 3;
    }
    
    void v810::outWrite(const Instruction& instruction)
    {
        d_printf("OUT.W\n");
        programCounter += 4;
    }
    
    void v810::nop(const Instruction& instruction)
    {
        d_printf("NOP\n");
        programCounter += 2;
        cycles += 1;
    }
    
    void v810::setInterruptDisable(const Instruction& instruction)
    {
        d_printf("SEI\n");
        systemRegisters.PSW.ID = 1;
        programCounter += 2;
        cycles += 1;
    }
    
    void v810::clearInterruptDisable(const Instruction& instruction)
    {
        d_printf("CLI\n");
        systemRegisters.PSW.ID = 0;
        programCounter += 2;
        cycles += 1;
    }
    
    void v810::returnFromTrap(const Instruction& instruction)
    {
        d_printf("RETI\n");
        if (systemRegisters.PSW.NP)
        {
            programCounter = systemRegisters.FEPC;
            systemRegisters.PSW = systemRegisters.FEPSW;
        }
        else
        {
            programCounter = systemRegisters.EIPC;
            systemRegisters.PSW = systemRegisters.EIPSW;            
        }
        cycles += 10;
    }
    
    void v810::compareFloat(const Instruction& instruction)
    {
        d_printf("CMPF.S\n");
        float f = generalRegistersFloat[instruction.reg2()] - generalRegistersFloat[instruction.reg1()];
        systemRegisters.PSW.Z = (f == 0.0f);
        systemRegisters.PSW.OV = 0;
        systemRegisters.PSW.S = (f < 0.0f);
        systemRegisters.PSW.CY = (f < 0.0f);  
        programCounter += 4;
        cycles += 7;
//        systemRegisters.PSW.FRO = 
    }
    
    void v810::convertWordToFloat(const Instruction& instruction)
    {
        d_printf("CVT.WS\n");
        float f = generalRegisters[instruction.reg1()];
        generalRegistersFloat[instruction.reg2()] = f;
        
        systemRegisters.PSW.Z = (f == 0.0f);
        systemRegisters.PSW.S = (f < 0.0f);
        systemRegisters.PSW.OV = 0;
        systemRegisters.PSW.CY = (f < 0.0f); 
        
        programCounter += 4;
        cycles += 5;
    }
    
    void v810::convertFloatToWord(const Instruction& instruction)
    {
        d_printf("CVT.SW\n");
        float f = generalRegistersFloat[instruction.reg1()];
        int32_t t = 0;
        if (f >= 0)
            t = floorf(f + .5f);
        else
            t = ceilf(f - .5f);
        
        generalRegisters[instruction.reg2()] = t;
        
        systemRegisters.PSW.OV = 0;
        systemRegisters.PSW.S = (t < 0);
        systemRegisters.PSW.Z = (t == 0);
        
        programCounter += 4;
        cycles += 9;
    }
    
    void v810::addFloat(const Instruction &instruction)
    {
        d_printf("ADDF.S\n");
        
        float f = generalRegistersFloat[instruction.reg2()] + generalRegistersFloat[instruction.reg1()];
        
        generalRegistersFloat[instruction.reg2()] = f;
        
        systemRegisters.PSW.OV = 0;
        systemRegisters.PSW.CY = (f < 0.0f);
        systemRegisters.PSW.S = (f < 0.0f);
        systemRegisters.PSW.Z = (f == 0.0f);
        programCounter += 4;
        cycles += 9;
    }
    
    void v810::subtractFloat(const Instruction &instruction)
    {
        d_printf("SUBF.S\n");
        
        float f = generalRegistersFloat[instruction.reg2()] - generalRegistersFloat[instruction.reg1()];
        
        generalRegistersFloat[instruction.reg2()] = f;
        
        systemRegisters.PSW.OV = 0;
        systemRegisters.PSW.CY = (f < 0.0f);
        systemRegisters.PSW.S = (f < 0.0f);
        systemRegisters.PSW.Z = (f == 0.0f);
        programCounter += 4;
        cycles += 12;
    }
    
    void v810::multiplyFloat(const Instruction &instruction)
    {
        d_printf("MULF.S\n");
        
        float f = generalRegistersFloat[instruction.reg2()] * generalRegistersFloat[instruction.reg1()];
        
        generalRegistersFloat[instruction.reg2()] = f;
        
        systemRegisters.PSW.OV = 0;
        systemRegisters.PSW.CY = (f < 0.0f);
        systemRegisters.PSW.S = (f < 0.0f);
        systemRegisters.PSW.Z = (f == 0.0f);
        programCounter += 4;
        cycles += 8;
    }
    
    void v810::divideFloat(const Instruction &instruction)
    {
        d_printf("DIVF.S\n");
        
        float f = generalRegistersFloat[instruction.reg2()] / generalRegistersFloat[instruction.reg1()];
        
        generalRegistersFloat[instruction.reg2()] = f;
        
        systemRegisters.PSW.OV = 0;
        systemRegisters.PSW.CY = (f < 0.0f);
        systemRegisters.PSW.S = (f < 0.0f);
        systemRegisters.PSW.Z = (f == 0.0f);
        programCounter += 4;
        cycles += 44;
    }
    
    void v810::truncateFloat(const Instruction& instruction)
    {
        d_printf("TRNC.SW\n");
        float f = generalRegistersFloat[instruction.reg1()];
        int32_t t = 0;
        if (f >= 0)
            t = floorf(f);
        else
            t = ceilf(f);
        
        generalRegisters[instruction.reg2()] = t;
        
        systemRegisters.PSW.OV = 0;
        systemRegisters.PSW.S = (t < 0);
        systemRegisters.PSW.Z = (t == 0);
        
        programCounter += 4;
        cycles += 8;
    }
    
    void v810::exchangeByte(const Instruction& instruction)
    {
        d_printf("XB\n");
        
        int32_t val = generalRegisters[instruction.reg2()];
        val = (val & 0xFFFF0000) | ((val << 8) & 0x0000FF00) | ((val >> 8) & 0x000000FF);
        generalRegisters[instruction.reg2()] = val;
        programCounter += 4;
        cycles += 1;
    }
    
    void v810::exchangeHalfWord(const Instruction &instruction)
    {
        d_printf("XH\n");
        int32_t val = generalRegisters[instruction.reg2()];
        val = ((val << 16) & 0xFFFF0000) | ((val >> 16) & 0x0000FFFF);
        generalRegisters[instruction.reg2()] = val;
        programCounter += 4;
        cycles += 1;
    }
    
    void v810::reverseWord(const Instruction &instruction)
    {
        d_printf("REV\n");
        int32_t word = generalRegisters[instruction.reg1()];
        int32_t out = 0;
        for (int i = 0; i < 32; ++i)
        {
            out |= (word & 1);
            out = out << 1;
            word = word >> 1;
        }
        
        generalRegisters[instruction.reg2()] = word;
        programCounter += 4;
        cycles += 1;
    }
    
    void v810::multiplyHalfWord(const Instruction &instruction)
    {
        d_printf("MPYHW\n");
        
        generalRegisters[instruction.reg2()] = generalRegisters[instruction.reg2()] * generalRegisters[instruction.reg1()];
        
        programCounter += 4;
        cycles += 9;
    }
    
    void v810::moveBitString(const Instruction& instruction)
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
        
        struct Mover
        {
            Mover() {}
            const uint32_t operator()(uint32_t a, uint32_t b) const { return b; };
        };
        
        while (source.HasData())
        {
            source.Read(data, bits);
            dest.SetNext<Mover>(data, bits);
        }
        
        assert(!source.HasData() && !dest.HasData());
        
        programCounter += 4;
    }
    
    void v810::andBitString(const Instruction& instruction)
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
        
        struct Ander
        {
            Ander() {}
            const uint32_t operator()(uint32_t a, uint32_t b) const { return a & b; };
        };
        
        while (source.HasData())
        {
            source.Read(data, bits);
            dest.SetNext<Ander>(data, bits);
        }
        
        assert(!source.HasData() && !dest.HasData());
        
        programCounter += 4;
    }

    void v810::orBitString(const Instruction& instruction)
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
        
        struct Orer
        {
            Orer() {}
            const uint32_t operator()(uint32_t a, uint32_t b) const { return a & b; };
        };
        
        while (source.HasData())
        {
            source.Read(data, bits);
            dest.SetNext<Orer>(data, bits);
        }
        
        assert(!source.HasData() && !dest.HasData());
        
        programCounter += 4;
    }
    
    void v810::xorBitString(const Instruction& instruction)
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
        
        struct Xorer
        {
            Xorer() {}
            const uint32_t operator()(uint32_t a, uint32_t b) const { return a & b; };
        };
        
        while (source.HasData())
        {
            source.Read(data, bits);
            dest.SetNext<Xorer>(data, bits);
        }
        
        assert(!source.HasData() && !dest.HasData());
        
        programCounter += 4;
    }
    
    void v810::andNotBitString(const Instruction& instruction)
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
        
        struct AndNoter
        {
            AndNoter() {}
            const uint32_t operator()(uint32_t a, uint32_t b) const { return a & (~b); };
        };
        
        while (source.HasData())
        {
            source.Read(data, bits);
            dest.SetNext<AndNoter>(data, bits);
        }
        
        assert(!source.HasData() && !dest.HasData());
        
        programCounter += 4;
    }
    
    void v810::orNotBitString(const Instruction& instruction)
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
        
        struct OrNoter
        {
            OrNoter() {}
            const uint32_t operator()(uint32_t a, uint32_t b) const { return a | (~b); };
        };
        
        while (source.HasData())
        {
            source.Read(data, bits);
            dest.SetNext<OrNoter>(data, bits);
        }
        
        assert(!source.HasData() && !dest.HasData());
        
        programCounter += 4;
    }

    void v810::xorNotBitString(const Instruction& instruction)
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
        
        struct XorNoter
        {
            XorNoter() {}
            const uint32_t operator()(uint32_t a, uint32_t b) const { return a | (~b); };
        };
        
        while (source.HasData())
        {
            source.Read(data, bits);
            dest.SetNext<XorNoter>(data, bits);
        }
        
        assert(!source.HasData() && !dest.HasData());
        
        programCounter += 4;
    }

    void v810::notBitString(const Instruction& instruction)
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
        
        struct Noter
        {
            Noter() {}
            const uint32_t operator()(uint32_t a, uint32_t b) const { return ~b; };
        };
        
        while (source.HasData())
        {
            source.Read(data, bits);
            dest.SetNext<Noter>(data, bits);
        }
        
        assert(!source.HasData() && !dest.HasData());
        
        programCounter += 4;
    }

}
