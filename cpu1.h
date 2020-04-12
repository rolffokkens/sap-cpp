/*
 * This implements CPU1: a CPU emulation based on Ben Eaters "8-bit computer
 * from scratch", see https://eater.net/8bit
 *
 * It's is microcode based, has 16 bytes of RAM and it runs the same
 * instruction set, has globally the same structure, but that's where all
 * similarities.
 *
 * Microcode is based on "bits" but the execution engine calls methods
 * based on these bits instead of assuming that all units instantly expose
 * the right behaviour, for details see execute_microcode ()
 */

#ifndef H_CPU1
#define H_CPU1

#include "internalbus.h"
#include "register.h"
#include "ram.h"
#include "cpu.h"

class CPU1 : CPU {
private:
    /****************************************************************************
     * Instead of using string assicoative arrays, use numbers for all registers
     ****************************************************************************/
    enum RegisterID {
        R_none = 0
    ,   R_PC
    ,   R_IR
    ,   R_MAR
    ,   R_RAM
    ,   R_A
    ,   R_TMP
    ,   R_ADDER
    ,   R_OUT
    ,   R_NUM
    };

    /****************************************************************************
     * And give them names too
     ****************************************************************************/
    struct RegisterInfo {
        int internal;
        const char *name;
    };

    static const RegisterInfo register_info[R_NUM];

    enum InstructionID {
        I_NOP = 0
    ,   I_LDA
    ,   I_ADD
    ,   I_SUB
    ,   I_STA
    ,   I_LDI
    ,   I_JMP
    ,   I_JC
    ,   I_JZ
    ,   I_R09
    ,   I_R0A
    ,   I_R0B
    ,   I_R0C
    ,   I_R0D
    ,   I_OUT
    ,   I_HLT
    ,   I_NUM
    };

    struct InstructionInfo {
        const char *name;
        int argument;
    };

    static const InstructionInfo instruction_info[I_NUM];

    /****************************************************************************
     * ALU - Arithmetic & Logical Unit. Now only Add & Subtract
     *       Connected to A, B and F registers
     ****************************************************************************/
    class Alu : public Register {
    private:
        Register &A, &TMP;
        FlagsRegister &F;
        char oper;
    public:
        enum Operation { OpNone, OpAdd, OpSub };
        static const char *OpNames[];

        Alu (InternalBus &bus, Register &_A, Register &_B, FlagsRegister &_F)
            : Register (bus)
            , A(_A)
            , TMP(_B)
            , F(_F)
        {
            oper = OpNone;
        };

        void SetOperation (char _oper)
        {
            oper = _oper;
        };

        void enable (void) {
            int result;
            char Z, C;

            switch (oper) {
            case OpAdd:
                result = A.get () + TMP.get ();
                break;
            case OpSub:
                result = A.get () - TMP.get ();
                break;
            };

            value = result & 0xff;

            C = (value != result);
            Z = (value == 0);
            F.set (Z, C);

            Register::enable ();
        };
    };

    /****************************************************************************
     * One micocode instruction:
     * - enable:    register to pass data to   the internal bus
     * - load:      register to get  data from the internal bus
     * - clock_pc:  Clock (Increment) the PC
     * - oper:      specify ALU operation
     * - halt:      stop execution
     * - carry:     requirement for C: FLAG_NONE, FLAG_0 or FLAG_1
     * - zero:      requirement for Z: FLAG_NONE, FLAG_0 or FLAG_1
     * - end_instr: end of microcode sequence, start next one
     ****************************************************************************/
    struct MicroInstruction {
        char enable;
        char load;
        char clock_pc;
        char oper;
        char halt;
        char carry;
        char zero;
        char end_instr;
    };

    Register A, TMP;
    RamRegister RAM;
    MemAddrRegister MAR;
    Alu ALU;
    FlagsRegister F;

    Register *registers[R_NUM];

    static const struct MicroInstruction prefix[];
    static const struct MicroInstruction inst_nop[];
    static const struct MicroInstruction inst_lda[];
    static const struct MicroInstruction inst_add[];
    static const struct MicroInstruction inst_sub[];
    static const struct MicroInstruction inst_sta[];
    static const struct MicroInstruction inst_ldi[];
    static const struct MicroInstruction inst_jmp[];
    static const struct MicroInstruction inst_jc[];
    static const struct MicroInstruction inst_jz[];
    static const struct MicroInstruction inst_out[];
    static const struct MicroInstruction inst_hlt[];

    struct MicroInstruction *instructions[16];

    struct MicroInstruction *initMicroInstruction (const struct MicroInstruction *_inst);

    /*
     * Initialize Microcode
     */
    void initMicroInstructions (void);

    /*
     * Dump registers
     */
    void dump_registers (const char *prefix, int internal) const;

    /*
     * Debugging output at sap level
     */
    void debug (int addr) const;

    /*
     * Debugging output at microcode_level
     */
    void debug_microcode (const struct MicroInstruction *ip) const;

    /*
     * Execute one microcode instruction
     */
    int execute_microcode(const struct MicroInstruction *ip, int debug_level);

public:
    /*
     * Constructor. Setup all components.
     */
    CPU1 (unsigned char *_ram)
        : CPU ()
        , RAM (bus, 16, _ram)
        , MAR (bus, RAM)
        , TMP (bus)
        , A (bus)
        , ALU (bus, A, TMP, F)
    {
        registers[R_PC]    = &PC;
        registers[R_IR]    = &IR;
        registers[R_MAR]   = &MAR;
        registers[R_RAM]   = &RAM;
        registers[R_A]     = &A;
        registers[R_TMP]   = &TMP;
        registers[R_ADDER] = &ALU;
        registers[R_OUT]   = &OUT;

        initMicroInstructions ();
    };

    int clock (int debug_level);
};

#endif
