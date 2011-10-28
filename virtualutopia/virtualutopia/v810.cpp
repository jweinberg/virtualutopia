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
#include "vsu.h"

//std::ofstream outFile("/Users/jweinberg/log.asm");
volatile bool debugOutput = false;

int32_t sign_extend(int bits, uint32_t rawValue)
{
    bits -= 1;
    uint32_t signBitMask = 1 << bits;
    char signValue = (rawValue & signBitMask) >> bits;
    return rawValue | (signValue ? (0xFFFFFFFF << bits) : 0);
}


namespace CPU
{
    v810::v810(MMU &_mmu, VIP::VIP &_vip, NVC::NVC &_nvc, VSU::VSU &_vsu) : memoryManagmentUnit(_mmu), vip(_vip), nvc(_nvc), vsu(_vsu)
    {
        systemRegisters.TKCW = 0x000000E0;
        systemRegisters.PIR = 0x00005346;
    }
    
    void v810::reset()
    {   
        programCounter = &memoryManagmentUnit.read<char>(0xFFFFFFF0);
        systemRegisters.PSW = 0x00008000;
        generalRegisters[0] = 0;
        cycles = 0;
        nvc.Reset();
    }
    
    void v810::fetchAndDecode()
    {
        
//        uint32_t address =  (0x07000000 + (uint32_t)((char*)programCounter - ((char*)memoryManagmentUnit.rom.data)));
//        if (address == 0x070000E0)
//            debugOutput = true;
////        
        const uint16_t& partialDecode = *(uint16_t*)programCounter;
        OpcodeMnumonic opcode;
        uint8_t arg1, arg2;
        uint32_t arg3;
        switch (partialDecode & 0xE000)
        {
            case 0x0000:
            case 0x2000:
            case 0x4000:
            case 0x6000:
                arg1 = partialDecode & 0x1F;
                arg2 = (partialDecode >> 5) & 0x1F;
                opcode = (OpcodeMnumonic)(partialDecode >> 10);
                break;
            case 0x8000:
                arg3 = partialDecode & 0x1FF;
                opcode = (OpcodeMnumonic)(partialDecode >> 9);
                break;
            case 0xA000:
                opcode = (OpcodeMnumonic)(partialDecode >> 10);
                arg3 = *(uint16_t*)(programCounter + 2);
                switch (opcode)
                {
                    case JR:
                    case JAL:
                        arg3 |= ((uint32_t)(partialDecode & 0x3FF)) << 16;
                        break;
                    default:
                        arg1 = partialDecode & 0x1F;
                        arg2 = (partialDecode >> 5) & 0x1F;
                        break;
                }
                break;
            case 0xC000:
                opcode = (OpcodeMnumonic)(partialDecode >> 10);
                arg1 = partialDecode & 0x1F;
                arg2 = (partialDecode >> 5) & 0x1F;
                arg3 = *(uint16_t*)(programCounter + 2);
                break;
            case 0xE000:
                opcode = (OpcodeMnumonic)(partialDecode >> 10);
                arg1 = partialDecode & 0x1F;
                arg2 = (partialDecode >> 5) & 0x1F;
                arg3 = (*(uint16_t*)(programCounter + 2)) >> 10;
                break;
        }
        
#define OPCODE_DECODE_I(OPCODE, FUNCTION) OPCODE: \
        FUNCTION(arg1, arg2); goto END;
#define OPCODE_DECODE_II(OPCODE, FUNCTION) OPCODE: \
        FUNCTION(arg1, arg2); goto END;
#define OPCODE_DECODE_III(OPCODE, FUNCTION) OPCODE: \
        FUNCTION(arg3); goto END;
#define OPCODE_DECODE_IV(OPCODE, FUNCTION) OPCODE: \
        FUNCTION(arg3); goto END;
#define OPCODE_DECODE_V(OPCODE, FUNCTION) OPCODE: \
        FUNCTION(arg1, arg2, arg3); goto END;
#define OPCODE_DECODE_VI(OPCODE, FUNCTION) OPCODE: \
        FUNCTION(arg1, arg2, arg3); goto END;
        
        static const void * jmp_entry[] =
        {
            &&MOV_1,
            &&ADD_1,
            &&SUB,  
            &&CMP_1,
            &&SHL_1,
            &&SHR_1,
            &&JMP,  
            &&SAR_1,
            &&MUL,  
            &&DIV,  
            &&MULU, 
            &&DIVU, 
            &&OR,   
            &&AND,  
            &&XOR,  
            &&NOT,  
            &&MOV_2,
            &&ADD_2,
            &&SETF, 
            &&CMP_2,
            &&SHL_2,
            &&SHR_2,
            &&CLI,  
            &&SAR_2,
            &&NOP, 
            &&RETI, 
            &&NOP, 
            &&NOP,   
            &&LDSR, 
            &&STSR, 
            &&SEI,  
            &&Bstr, 
            &&NOP,   
            &&NOP,   
            &&NOP,   
            &&NOP,   
            &&NOP,   
            &&NOP,   
            &&NOP,   
            &&NOP,   
            &&MOVEA,
            &&ADDI, 
            &&JR,   
            &&JAL,  
            &&ORI,  
            &&ANDI, 
            &&XORI, 
            &&MOVHI,
            &&LD_B, 
            &&LD_H, 
            &&NOP,   
            &&LD_W, 
            &&ST_B, 
            &&ST_H, 
            &&NOP,   
            &&ST_W, 
            &&IN_B, 
            &&IN_H, 
            &&CAXI, 
            &&IN_W, 
            &&OUT_B,
            &&OUT_H,
            &&Fpp,  
            &&OUT_W,
            &&BV,   
            &&BC,   
            &&BZ,   
            &&BNH,  
            &&BN,   
            &&BR,   
            &&BLT,  
            &&BLE,  
            &&BNV,  
            &&BNC,  
            &&BNZ,  
            &&BH,   
            &&BP,   
            &&NOP,  
            &&BGE,  
            &&BGT, 
        };

        goto *jmp_entry[opcode];
        
            OPCODE_DECODE_I(MOV_1, move);
            OPCODE_DECODE_I(ADD_1, add); 
            OPCODE_DECODE_I(SUB, subtract);   
            OPCODE_DECODE_I(CMP_1, compare); 
            OPCODE_DECODE_I(SHL_1, shiftLeft); 
            OPCODE_DECODE_I(SHR_1, shiftRight); 
            OPCODE_DECODE_I(JMP, jump);   
            OPCODE_DECODE_I(SAR_1, shiftArithmeticRight); 
            OPCODE_DECODE_I(MUL, multiply);   
            OPCODE_DECODE_I(DIV, divide);   
            OPCODE_DECODE_I(MULU, multiplyUnsigned);  
            OPCODE_DECODE_I(DIVU, divideUnsigned);  
            OPCODE_DECODE_I(OR, logicalOr);    
            OPCODE_DECODE_I(AND, logicalAnd);   
            OPCODE_DECODE_I(XOR, logicalXor);   
            OPCODE_DECODE_I(NOT, logicalNot);   
            OPCODE_DECODE_II(MOV_2, moveImmediate); 
            OPCODE_DECODE_II(ADD_2, addImmediate5); 
            OPCODE_DECODE_II(SETF, setFlag);  
            OPCODE_DECODE_II(CMP_2, compareImmediate); 
            OPCODE_DECODE_II(SHL_2, shiftLeftImmediate); 
            OPCODE_DECODE_II(SHR_2, shiftRightImmediate); 
            OPCODE_DECODE_II(CLI, clearInterruptDisable);   
            OPCODE_DECODE_II(SAR_2, shiftArithmeticRightImmediate); 
            //OPCODE_DECODE_II(TRAP, nop); 
            OPCODE_DECODE_II(RETI, returnFromTrap);  
            //OPCODE_DECODE_II(HALT, <#FUNCTION#>);  
            OPCODE_DECODE_II(LDSR, loadSystemRegister);  
            OPCODE_DECODE_II(STSR, storeSystemRegister);  
            OPCODE_DECODE_II(SEI, setInterruptDisable);   
            Bstr:
                switch ((BinaryStringMnumonic)partialDecode & 0x1F)
                {
//                    OPCODE_DECODE_II(SCH0BSU, nop); 
//                    OPCODE_DECODE_II(SCH0BSD, nop); 
//                    OPCODE_DECODE_II(SCH1BSU, nop); 
//                    OPCODE_DECODE_II(SCH1BSD, nop); 
                    case OPCODE_DECODE_II(ORBSU, orBitString);   
                    case OPCODE_DECODE_II(ANDBSU, andBitString); 
                    case OPCODE_DECODE_II(XORBSU, xorBitString);  
                    case OPCODE_DECODE_II(MOVBSU, moveBitString);  
                    case OPCODE_DECODE_II(ORNBSU, orNotBitString);  
                    case OPCODE_DECODE_II(ANDNBSU, andNotBitString); 
                    case OPCODE_DECODE_II(XORNBSU, xorBitString); 
                    case OPCODE_DECODE_II(NOTBSU, notBitString);  
                }
            OPCODE_DECODE_V(MOVEA, moveAddImmediate); 
            OPCODE_DECODE_V(ADDI, addImmediate);  
            OPCODE_DECODE_IV(JR, jumpRelative);    
            OPCODE_DECODE_IV(JAL, jumpAndLink);   
            OPCODE_DECODE_V(ORI, orImmediate);   
            OPCODE_DECODE_V(ANDI, andImmediate);  
            OPCODE_DECODE_V(XORI, xorImmediate);  
            OPCODE_DECODE_V(MOVHI, moveHigh); 
            OPCODE_DECODE_V(LD_B, loadByte);  
            OPCODE_DECODE_VI(LD_H, loadHWord);  
            OPCODE_DECODE_VI(LD_W, loadWord);  
            OPCODE_DECODE_VI(ST_B, storeByte);  
            OPCODE_DECODE_VI(ST_H, storeHWord);  
            OPCODE_DECODE_VI(ST_W, storeWord);  
            OPCODE_DECODE_VI(IN_B, outWrite);  
            OPCODE_DECODE_VI(IN_H, outWrite);  
            OPCODE_DECODE_VI(CAXI, outWrite);  
            OPCODE_DECODE_VI(IN_W, outWrite);  
            OPCODE_DECODE_VI(OUT_B, outWrite); 
            OPCODE_DECODE_VI(OUT_H, outWrite); 
            Fpp:
        
                switch ((FloatingPointMnumonic)arg3)
                {
                    case OPCODE_DECODE_I(CMPF_S, compareFloat);
                    case OPCODE_DECODE_I(CVT_WS, convertWordToFloat);
                    case OPCODE_DECODE_I(CVT_SW, convertFloatToWord);
                    case OPCODE_DECODE_I(ADDF_S, addFloat);
                    case OPCODE_DECODE_I(SUBF_S, subtractFloat);
                    case OPCODE_DECODE_I(MULF_S, multiplyFloat);
                    case OPCODE_DECODE_I(DIVF_S, divideFloat);
                    case OPCODE_DECODE_I(XB, exchangeByte);
                    case OPCODE_DECODE_I(XH, exchangeHalfWord);
                    case OPCODE_DECODE_I(REV, reverseWord);
                    case OPCODE_DECODE_I(TRNC_SW, truncateFloat);
                    case OPCODE_DECODE_I(MPYHW, multiplyHalfWord);
                }
            OPCODE_DECODE_VI(OUT_W, outWrite);  
            OPCODE_DECODE_III(BV, branchIfOverflow);    
            OPCODE_DECODE_III(BC, branchIfCarry);    
            OPCODE_DECODE_III(BZ, branchIfZero);    
            OPCODE_DECODE_III(BNH, branchIfNotHigher);   
            OPCODE_DECODE_III(BN, branchIfNegative);    
            OPCODE_DECODE_III(BR, branch);    
            OPCODE_DECODE_III(BLT, branchIfLessThan);   
            OPCODE_DECODE_III(BLE, branchIfLessOrEqual);   
            OPCODE_DECODE_III(BNV, branchIfNoOverflow);
            OPCODE_DECODE_III(BNC, branchIfNoCarry);
            OPCODE_DECODE_III(BNZ, branchIfNotZero);
            OPCODE_DECODE_III(BH, branchIfHigher);
            OPCODE_DECODE_III(BP, branchIfPositive);    
            OPCODE_DECODE_III(NOP, nop);
            OPCODE_DECODE_III(BGE, branchIfGreaterOrEqual);
            OPCODE_DECODE_III(BGT, branchIfGreaterThan);
#undef OPCODE_DECODE
    END:
        return;
    }
    
    void v810::step()
    {    
        //Some instructions think its FUNNY to assign to reg 0 as an optimization
        generalRegisters[0] = 0;   
        
        nvc.Step(cycles);
        vip.Step(cycles);
//        
//        if (vip.INTENB & vip.INTPND)
//            processInterrupt(INTVPU);
//        
        fetchAndDecode();
    }
    
    void v810::processInterrupt(InterruptCode interruptCode)
    {
        if (systemRegisters.PSW.NP) return;
        if (systemRegisters.PSW.EP) return;
        if (systemRegisters.PSW.ID) return;
        if (systemRegisters.PSW.IntLevel > interruptCode) return;
        
        
        d_printf("Proccessing interrupt:\n");
        switch (interruptCode)
        {
            case INTKEY:
                d_printf("\tINTKEY\n");
                break;
            case INTTIM:
                d_printf("\tINTTIM\n");
                break;
            case INTVPU:                
                d_printf("\tINTVPU:\n"
                       "\t\tSCANERR: %d\n"
                       "\t\tLFBEND: %d\n"
                       "\t\tRFBEND: %d\n"
                       "\t\tGAMESTART: %d\n"
                       "\t\tFRAMESTART: %d\n"
                       "\t\tSBHIT: %d\n"
                       "\t\tXPEND: %d\n"
                       "\t\tTIMERR: %d\n",vip.INTPND.SCANERR, vip.INTPND.LFBEND, vip.INTPND.RFBEND, vip.INTPND.GAMESTART, vip.INTPND.FRAMESTART, vip.INTPND.SBHIT, vip.INTPND.XPEND, vip.INTPND.TIMERR);
                break;
            default:
                break;
        }
        
        systemRegisters.EIPC = (0x07000000 + (uint32_t)(programCounter - memoryManagmentUnit.rom.data));
        systemRegisters.EIPSW = systemRegisters.PSW;
        systemRegisters.ECR.EICC = interruptCode;
        systemRegisters.PSW.EP = 1;
        systemRegisters.PSW.ID = 1;
        systemRegisters.PSW.AE = 0;
        
        systemRegisters.PSW.IntLevel = min(interruptCode+1, 16);
        programCounter = &memoryManagmentUnit.read<char>(0xFFFFFE00 | interruptCode << 4);
    }
    
    void v810::throwException(ExceptionCode exceptionCode)
    {
        //Fatal exception
        if (systemRegisters.PSW.NP)
        {
            memoryManagmentUnit.store((uint32_t)(0xFFFF0000 | exceptionCode), (uint32_t)0x0);
            memoryManagmentUnit.store(systemRegisters.PSW, 0x4);
            memoryManagmentUnit.store(0x07000000 + (uint32_t)(programCounter - memoryManagmentUnit.rom.data), 0x8);
            return;
        }
        
        //Duplexed Exception
        if (systemRegisters.PSW.EP)
        {
            systemRegisters.FEPC = (0x07000000 + (uint32_t)(programCounter - memoryManagmentUnit.rom.data));
            systemRegisters.FEPSW = systemRegisters.PSW;
            systemRegisters.ECR.FECC = exceptionCode;
            systemRegisters.PSW.NP = 1;
            systemRegisters.PSW.ID = 1;
            systemRegisters.PSW.AE = 0;
            programCounter = &memoryManagmentUnit.read<char>(0xFFFFFFD0);
        }
        else
        {
            systemRegisters.EIPC = (0x07000000 + (uint32_t)(programCounter - memoryManagmentUnit.rom.data));
            systemRegisters.EIPSW = systemRegisters.PSW;
            systemRegisters.ECR.EICC = exceptionCode;
            systemRegisters.PSW.EP = 1;
            systemRegisters.PSW.ID = 1;
            systemRegisters.PSW.AE = 0;
            
            //TODO: Lookup the proper handler, this is DIV0
            programCounter = &memoryManagmentUnit.read<char>(0xFFFFFF80);
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