//
//  instruction.h
//  virtualutopia
//

#ifndef virtualutopia_instruction_h
#define virtualutopia_instruction_h

namespace CPU
{
    enum OpcodeMnumonic
    {
        MOV_1 = 0x00,//0b000000,
        ADD_1 = 0x01,//0b000001,
        SUB   = 0x02,//0b000010,
        CMP_1 = 0x03,//0b000011,
        SHL_1 = 0x04,//0b000100,
        SHR_1 = 0x05,//0b000101,
        JMP   = 0x06,//0b000110,
        SAR_1 = 0x07,//0b000111,
        MUL   = 0x08,//0b001000,
        DIV   = 0x09,//0b001001,
        MULU  = 0x0A,//0b001010,
        DIVU  = 0x0B,//0b001011,
        OR    = 0x0C,//0b001100,
        AND   = 0x0D,//0b001101,
        XOR   = 0x0E,//0b001110,
        NOT   = 0x0F,//0b001111,
        
        MOV_2 = 0x10,//0b010000,
        ADD_2 = 0x11,//0b010001,
        SETF  = 0x12,//0b010010,
        CMP_2 = 0x13,//0b010011,
        SHL_2 = 0x14,//0b010100,
        SHR_2 = 0x15,//0b010101,
        CLI   = 0x16,//0b010110,
        SAR_2 = 0x17,//0b010111,
        TRAP  = 0x18,//0b011000,
        RETI  = 0x19,//0b011001,
        HALT  = 0x1A,//0b011010,
        //      0x1B,//0b011011,
        LDSR  = 0x1C,//0b011100,
        STSR  = 0x1D,//0b011101,
        SEI   = 0x1E,//0b011110,
        Bstr  = 0x1F,//0b011111,
        //      0x20,//0b100000,
        //      0x21,//0b100001,
        //      0x22,//0b100010,
        //      0x23,//0b100011,
        //      0x24,//0b100100,
        //      0x25,//0b100101,
        //      0x26,//0b100110,
        //      0x27,//0b100111,
        MOVEA = 0x28,//0b101000,
        ADDI  = 0x29,//0b101001,
        JR    = 0x2A,//0b101010,
        JAL   = 0x2B,//0b101011,
        ORI   = 0x2C,//0b101100,
        ANDI  = 0x2D,//0b101101,
        XORI  = 0x2E,//0b101110,
        MOVHI = 0x2F,//0b101111,
        
        LD_B  = 0x30,//0b110000,
        LD_H  = 0x31,//0b110001,
        //      0x32,//0b110010,
        LD_W  = 0x33,//0b110011,
        ST_B  = 0x34,//0b110100,
        ST_H  = 0x35,//0b110101,
        //      0x36,//0b110110,
        ST_W  = 0x37,//0b110111,
        IN_B  = 0x38,//0b111000,
        IN_H  = 0x39,//0b111001,
        CAXI  = 0x3A,//0b111010,
        IN_W  = 0x3B,//0b111011,
        OUT_B = 0x3C,//0b111100,
        OUT_H = 0x3D,//0b111101,
        Fpp   = 0x3E,//0b111110,
        OUT_W = 0x3F,//0b111111,

        BV    = 0x40,//0b1000000,
        BC    = 0x41,//0b1000001,
        BZ    = 0x42,//0b1000010,
        BNH   = 0x43,//0b1000011,
        BN    = 0x44,//0b1000100,
        BR    = 0x45,//0b1000101,
        BLT   = 0x46,//0b1000110,
        BLE   = 0x47,//0b1000111,
        BNV   = 0x48,//0b1001000,
        BNC   = 0x49,//0b1001001,
        BNZ   = 0x4A,//0b1001010,
        BH    = 0x4B,//0b1001011,
        BP    = 0x4C,//0b1001100,
        NOP   = 0x4D,//0b1001101,
        BGE   = 0x4E,//0b1001110,
        BGT   = 0x4F,//0b1001111,
    };
    
    enum BinaryStringMnumonic
    {
        SCH0BSU = 0x00,//0b00000,
        SCH0BSD = 0x01,//0b00001,
        SCH1BSU = 0x02,//0b00010,
        SCH1BSD = 0x03,//0b00011,
        //      = 0x04,//0b00100,
        //      = 0x05,//0b00101,
        //      = 0x06,//0b00110,
        //      = 0x07,//0b00111,
        ORBSU   = 0x08,//0b01000,
        ANDBSU  = 0x09,//0b01001,,
        XORBSU  = 0x0A,//0b01010,,
        MOVBSU  = 0x0B,//0b01011,,
        ORNBSU  = 0x0C,//0b01100,,
        ANDNBSU = 0x0D,//0b01101,,
        XORNBSU = 0x0E,//0b01110,,
        NOTBSU  = 0x0F,//0b01111,
    };
    
    enum FloatingPointMnumonic
    {
        CMPF_S  = 0x00,//0b000000,
        //      = 0x01,//0b000001,
        CVT_WS  = 0x02,//0b000010,
        CVT_SW  = 0x03,//0b000011,
        ADDF_S  = 0x04,//0b000100,
        SUBF_S  = 0x05,//0b000101,
        MULF_S  = 0x06,//0b000110,
        DIVF_S  = 0x07,//0b000111,
        XB      = 0x08,//0b001000,
        XH      = 0x09,//0b001001,
        REV     = 0x0A,//0b001010,
        TRNC_SW = 0x0B,//0b001011,
        MPYHW   = 0x0C,//0b001100,
    };
    
    class Instruction 
    {
    public:
        union
        {
            const uint32_t instruction;
            struct
            {
                struct
                {
                    uint16_t reg1:5;
                    uint16_t reg2:5;
                    uint16_t store:6;
                };
                union
                {
                    uint16_t imm16;
                    uint16_t disp16;
                };
            };
        };
    public:
        uint32_t disp26() const
        {
            return ((instruction & 0x3FF) << 16) | (instruction >> 16);
        }

        uint16_t disp9() const
        {
            return instruction & 0x1FF;
        }
        
        uint8_t subopcode() const
        {
            return instruction >> 26;
        }
        
        OpcodeMnumonic opcode() const
        {
            return (OpcodeMnumonic)((instruction & 0xE000) == 0x8000 ? (instruction & 0xFFFF) >> 9 : (instruction & 0xFFFF) >> 10);
        }

    public:
        Instruction(const uint32_t _instruction) : 
            instruction(_instruction)
            {
//                if (opcode & 0x
//                ((OpcodeMnumonic)((instruction & 0xFFFF) >> (((instruction >> 13) & 0x7) == 4 ? 9 : 10)));
            }
    };
}


#endif
