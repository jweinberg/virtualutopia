#include <iostream>
#include <cmath>
#include "v810.h"
#include "cpu_utils.h"
#include "mmu.h"
#include "vip.h"
#include "instruction.h"
#include <sstream>
#include <string>
#include <algorithm>
#include <iomanip>
#include <iterator>

namespace CPU
{
    v810::v810(MMU &_mmu, VIP::VIP &_vip, NVC::NVC &_nvc) : memoryManagmentUnit(_mmu), vip(_vip), nvc(_nvc)
    {
        debugOutput = false;
        systemRegisters.TKCW = 0x000000E0;
        systemRegisters.PIR = 0x00005346;
    }
    
    void v810::reset()
    {   
        programCounter = (char*)&memoryManagmentUnit.read<uint32_t>(0xFFFFFFF0);
        systemRegisters.PSW = 0x00008000;
        generalRegisters[0] = 0;
        nvc.Reset();
    }
    
    void v810::decode(uint32_t data)
    {
        const Instruction &instruction(data);
#define OPCODE_DECODE(OPCODE, FUNCTION) case OPCODE: FUNCTION(instruction); break;

        switch (instruction.opcode())
        {
            OPCODE_DECODE(MOV_1, move);
            OPCODE_DECODE(ADD_1, add); 
            OPCODE_DECODE(SUB, subtract);   
            OPCODE_DECODE(CMP_1, compare); 
            OPCODE_DECODE(SHL_1, shiftLeft); 
            OPCODE_DECODE(SHR_1, shiftRight); 
            OPCODE_DECODE(JMP, jump);   
            OPCODE_DECODE(SAR_1, shiftArithmeticRight); 
            OPCODE_DECODE(MUL, multiply);   
            OPCODE_DECODE(DIV, divide);   
            OPCODE_DECODE(MULU, multiplyUnsigned);  
            OPCODE_DECODE(DIVU, divideUnsigned);  
            OPCODE_DECODE(OR, logicalOr);    
            OPCODE_DECODE(AND, logicalAnd);   
            OPCODE_DECODE(XOR, logicalXor);   
            OPCODE_DECODE(NOT, logicalNot);   
            OPCODE_DECODE(MOV_2, moveImmediate); 
            OPCODE_DECODE(ADD_2, addImmediate5); 
            OPCODE_DECODE(SETF, setFlag);  
            OPCODE_DECODE(CMP_2, compareImmediate); 
            OPCODE_DECODE(SHL_2, shiftLeftImmediate); 
            OPCODE_DECODE(SHR_2, shiftRightImmediate); 
            OPCODE_DECODE(CLI, clearInterruptDisable);   
            OPCODE_DECODE(SAR_2, shiftArithmeticRightImmediate); 
            //OPCODE_DECODE(TRAP, nop); 
            OPCODE_DECODE(RETI, returnFromTrap);  
            //OPCODE_DECODE(HALT, <#FUNCTION#>);  
            OPCODE_DECODE(LDSR, loadSystemRegister);  
            OPCODE_DECODE(STSR, storeSystemRegister);  
            OPCODE_DECODE(SEI, setInterruptDisable);   
            case Bstr:
                switch ((BinaryStringMnumonic)instruction.reg1)
                {
                    OPCODE_DECODE(SCH0BSU, nop); 
                    OPCODE_DECODE(SCH0BSD, nop); 
                    OPCODE_DECODE(SCH1BSU, nop); 
                    OPCODE_DECODE(SCH1BSD, nop); 
                    OPCODE_DECODE(ORBSU, orBitString);   
                    OPCODE_DECODE(ANDBSU, andBitString); 
                    OPCODE_DECODE(XORBSU, xorBitString);  
                    OPCODE_DECODE(MOVBSU, moveBitString);  
                    OPCODE_DECODE(ORNBSU, orNotBitString);  
                    OPCODE_DECODE(ANDNBSU, andNotBitString); 
                    OPCODE_DECODE(XORNBSU, xorBitString); 
                    OPCODE_DECODE(NOTBSU, notBitString);  
                    default:
                        printf("%X: Decoding uknown bstr: (0x%X)\n", (0x07000000 + (uint32_t)((char*)programCounter - ((char*)memoryManagmentUnit.rom.data))), instruction.reg1);
                        exit(-1);
                }
                break;
            OPCODE_DECODE(MOVEA, moveAddImmediate); 
            OPCODE_DECODE(ADDI, addImmediate);  
            OPCODE_DECODE(JR, jumpRelative);    
            OPCODE_DECODE(JAL, jumpAndLink);   
            OPCODE_DECODE(ORI, orImmediate);   
            OPCODE_DECODE(ANDI, andImmediate);  
            OPCODE_DECODE(XORI, xorImmediate);  
            OPCODE_DECODE(MOVHI, moveHigh); 
            OPCODE_DECODE(LD_B, loadByte);  
            OPCODE_DECODE(LD_H, loadHWord);  
            OPCODE_DECODE(LD_W, loadWord);  
            OPCODE_DECODE(ST_B, storeByte);  
            OPCODE_DECODE(ST_H, storeHWord);  
            OPCODE_DECODE(ST_W, storeWord);  
            OPCODE_DECODE(IN_B, outWrite);  
            OPCODE_DECODE(IN_H, outWrite);  
            OPCODE_DECODE(CAXI, outWrite);  
            OPCODE_DECODE(IN_W, outWrite);  
            OPCODE_DECODE(OUT_B, outWrite); 
            OPCODE_DECODE(OUT_H, outWrite); 
            case Fpp:
                switch ((FloatingPointMnumonic)instruction.subopcode())
                {
                    OPCODE_DECODE(CMPF_S, compareFloat);
                    OPCODE_DECODE(CVT_WS, convertWordToFloat);
                    OPCODE_DECODE(CVT_SW, convertFloatToWord);
                    OPCODE_DECODE(ADDF_S, addFloat);
                    OPCODE_DECODE(SUBF_S, subtractFloat);
                    OPCODE_DECODE(MULF_S, multiplyFloat);
                    OPCODE_DECODE(DIVF_S, divideFloat);
                    OPCODE_DECODE(XB, exchangeByte);
                    OPCODE_DECODE(XH, exchangeHalfWord);
                    OPCODE_DECODE(REV, reverseWord);
                    OPCODE_DECODE(TRNC_SW, truncateFloat);
                    OPCODE_DECODE(MPYHW, multiplyHalfWord);
                    default:
                        printf("%X: Decoding uknown fppcode: (0x%X)\n", (0x07000000 + (uint32_t)((char*)programCounter - ((char*)memoryManagmentUnit.rom.data))), instruction.subopcode());
                        exit(-1);
                }
                break;
            OPCODE_DECODE(OUT_W, outWrite);  
            OPCODE_DECODE(BV, branchIfOverflow);    
            OPCODE_DECODE(BC, branchIfCarry);    
            OPCODE_DECODE(BZ, branchIfZero);    
            OPCODE_DECODE(BNH, branchIfNotHigher);   
            OPCODE_DECODE(BN, branchIfNegative);    
            OPCODE_DECODE(BR, branch);    
            OPCODE_DECODE(BLT, branchIfLessThan);   
            OPCODE_DECODE(BLE, branchIfLessOrEqual);   
            OPCODE_DECODE(BNV, branchIfNoOverflow);
            OPCODE_DECODE(BNC, branchIfNoCarry);
            OPCODE_DECODE(BNZ, branchIfNotZero);
            OPCODE_DECODE(BH, branchIfHigher);
            OPCODE_DECODE(BP, branchIfPositive);    
            OPCODE_DECODE(NOP, nop);
            OPCODE_DECODE(BGE, branchIfGreaterOrEqual);
            OPCODE_DECODE(BGT, branchIfGreaterThan);
            default:
                printf("%X: Decoding uknown opcode: (0x%X)\n", (0x07000000 + (uint32_t)((char*)programCounter - ((char*)memoryManagmentUnit.rom.data))),  instruction.opcode());
                exit(-1);
        }
#undef OPCODE_DECODE
    }
    
    void v810::step()
    {    
        //Some instructions think its FUNNY to assign to reg 0 as an optimization
        generalRegisters[0] = 0;   
//        nvc.SCR.STAT = 1;
//        nvc.SDHR.STA = 1;
//        if (!nvc.SCR.DIS && (nvc.SDHR != 0 || nvc.SDLR & 0xF8))
//            processInterrupt((InterruptCode)0);
//        
        nvc.Step(cycles);
        vip.Step(cycles);
        decode(*(uint32_t*)programCounter);
    }
    
    void v810::processInterrupt(InterruptCode interruptCode)
    {
        if (systemRegisters.PSW.NP) return;
        if (systemRegisters.PSW.EP) return;
        if (systemRegisters.PSW.ID) return;
        if (systemRegisters.PSW.IntLevel > interruptCode) return;
        
        systemRegisters.EIPC = (0x07000000 + (uint32_t)((char*)programCounter - ((char*)memoryManagmentUnit.rom.data)));
        systemRegisters.EIPSW = systemRegisters.PSW;
        systemRegisters.ECR.EICC = interruptCode;
        systemRegisters.PSW.EP = 1;
        systemRegisters.PSW.ID = 1;
        systemRegisters.PSW.AE = 0;
        
        systemRegisters.PSW.IntLevel = min(interruptCode+1, 16);
        programCounter = (char*)&memoryManagmentUnit.read<uint32_t>(0xFFFFFE00 | interruptCode << 4);
    }
    
    void v810::throwException(ExceptionCode exceptionCode)
    {
        //Fatal exception
        if (systemRegisters.PSW.NP)
        {
            memoryManagmentUnit.store((uint32_t)(0xFFFF0000 | exceptionCode), (uint32_t)0x0);
            memoryManagmentUnit.store(systemRegisters.PSW, 0x4);
            memoryManagmentUnit.store(0x07000000 + (uint32_t)((char*)programCounter - ((char*)memoryManagmentUnit.rom.data)), 0x8);
            return;
        }
        
        //Duplexed Exception
        if (systemRegisters.PSW.EP)
        {
            systemRegisters.FEPC = (0x07000000 + (uint32_t)((char*)programCounter - ((char*)memoryManagmentUnit.rom.data)));
            systemRegisters.FEPSW = systemRegisters.PSW;
            systemRegisters.ECR.FECC = exceptionCode;
            systemRegisters.PSW.NP = 1;
            systemRegisters.PSW.ID = 1;
            systemRegisters.PSW.AE = 0;
            programCounter = (char*)&memoryManagmentUnit.read<uint32_t>(0xFFFFFFD0);
        }
        else
        {
            systemRegisters.EIPC = (0x07000000 + (uint32_t)((char*)programCounter - ((char*)memoryManagmentUnit.rom.data)));
            systemRegisters.EIPSW = systemRegisters.PSW;
            systemRegisters.ECR.EICC = exceptionCode;
            systemRegisters.PSW.EP = 1;
            systemRegisters.PSW.ID = 1;
            systemRegisters.PSW.AE = 0;
            
            //TODO: Lookup the proper handler, this is DIV0
            programCounter = (char*)&memoryManagmentUnit.read<uint32_t>(0xFFFFFF80);
        }
        
    }
    
    const std::string v810::registerDescription() const
    {
        char buffer[1024];
        char *p = buffer;
        for (int i = 0; i < 32; ++i)
        {
            p += sprintf(p, "GR[%d]:\t0x%X\n", i, generalRegisters[i]);
        }
        return buffer;
    }
}