/*
 * This implements CPU2: a CPU emulation based on CPU1 (see cpu1.h) but with
 * some "enhancements":
 *   - 4 registers (A, B, C, D) instead of only A
 *   - Access to 256 bytes of RAM
 *   - To access all RAM many instructions are now multy-byte
 */

#ifndef H_CPU2
#define H_CPU2

#include "internalbus.h"
#include "register.h"
#include "ram.h"
#include "cpu.h"

class CPU2 : CPU {
private:
    /****************************************************************************
     * Instead of using string associative arrays, use numbers for all registers
     ****************************************************************************/
    enum RegisterID {
        R_none = 0
    ,   R_PC
    ,   R_IR
    ,   R_MAR
    ,   R_RAM
    ,   R_TMP
    ,   R_ADDER
    ,   R_OUT
    ,   R_A
    ,   R_B
    ,   R_C
    ,   R_D
    ,   R_NUM
    ,   R_ANY
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
    ,   I_LD
    ,   I_ADD
    ,   I_SUB
    ,   I_ST
    ,   I_LDI
    ,   I_JMP
    ,   I_LDA
    ,   I_STA
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
        char argument;
        char regid;
        char cond;
    };

    static const InstructionInfo instruction_info[I_NUM];

    /****************************************************************************
     * ALU - Arithmetic & Logical Unit. Now only Add & Subtract
     *       Connected to A, B and F registers
     ****************************************************************************/
    class Alu : public Register {
    private:
        Register &A, &B;
        FlagsRegister &F;
        char oper;
    public:
        enum Operation { OpNone, OpAdd, OpSub };
        static const char *OpNames[];

        Alu (InternalBus &bus, Register &_A, Register &_B, FlagsRegister &_F)
            : Register (bus)
            , A(_A)
            , B(_B)
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
                result = A.get () + B.get ();
                break;
            case OpSub:
                result = A.get () - B.get ();
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
     * - cond:      requirement for C/Z
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
        char cond;
        char end_instr;
    };

    Register A, B, C, D;
    RamRegister RAM;
    MemAddrRegister MAR;
    Alu ALU;
    FlagsRegister F;

    Register *registers[R_NUM];

    static const struct MicroInstruction prefix[];
    static const struct MicroInstruction inst_nop[];
    static const struct MicroInstruction inst_ld[];
    static const struct MicroInstruction inst_add[];
    static const struct MicroInstruction inst_sub[];
    static const struct MicroInstruction inst_st[];
    static const struct MicroInstruction inst_ldi[];
    static const struct MicroInstruction inst_jmp[];
    static const struct MicroInstruction inst_lda[];
    static const struct MicroInstruction inst_sta[];
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
    void debug_microcode (const struct MicroInstruction *ip, unsigned char regid) const;

    inline unsigned char map_reg (unsigned char reg, unsigned char regid) const
    {
        if (reg != CPU2::R_ANY) return reg;

        return R_A + regid;
    };
    void map_cond (unsigned char data, char &zero, char &carry, const char *&name) const;
    /*
     * Execute one microcode instruction
     */
    int execute_microcode(const struct MicroInstruction *ip, unsigned char regid, int debug_level);

public:
    /*
     * Constructor. Setup all components.
     */
    CPU2 (unsigned char *_ram)
        : CPU (_ram)
        , RAM (bus, 256, _ram)
        , MAR (bus, RAM)
        , A (bus)
        , B (bus)
        , C (bus)
        , D (bus)
        , ALU (bus, A, B, F)
    {
        registers[R_PC]    = &PC;
        registers[R_IR]    = &IR;
        registers[R_MAR]   = &MAR;
        registers[R_RAM]   = &RAM;
        registers[R_TMP]   = &B;
        registers[R_ADDER] = &ALU;
        registers[R_OUT]   = &OUT;
        registers[R_A]     = &A;
        registers[R_B]     = &B;
        registers[R_C]     = &C;
        registers[R_D]     = &D;
/*
        micro_ptr = 0;

        PC.reset ();
        IR.reset ();

        halted = 0;
*/


        initMicroInstructions ();
    };

    int clock (int debug_level);
};

#endif
