/*
 * This implements CPU16: a CPU emulation based on CPU1 (see cpu1.h) but with
 * some "enhancements":
 *   - 4 registers (A, B, C, D) instead of only A (and hidden B)
 *   - Access to 256 bytes of RAM
 *   - To access all RAM many instructions are now multy-byte
 */

#ifndef H_CPU16
#define H_CPU16

#include "internalbus16.h"
#include "register16.h"
#include "ram8.h"

class CPU {
protected:
    /****************************************************************************
     * Micro code is always executed conditional, conditions specified as
     * requirenents for Z en C flags. FLAG_NONE means: no requirement.
     ****************************************************************************/
    enum FlagRequire {
        FLAG_NONE = 0
    ,   FLAG_0    = 1
    ,   FLAG_1    = 2
    };

    /****************************************************************************
     * FlagsRegister - could be part of ALU
     ****************************************************************************/
    class FlagsRegister {
    private:
        char Z, C;
    public:
        FlagsRegister () {
            Z = 0;
            C = 0;
        };
        inline void set (char _Z, char _C) {
            Z = _Z;
            C = _C;
        };
        inline void get (char &_Z, char &_C) const {
            _Z = Z;
            _C = C;
        };
    };

    InternalBus16 bus;
    Counter16 PC;
    Register16 IR;
    OutputRegister16 OUT;
    FlagsRegister F;
    int halted;
    int micro_ptr;

    /*
     * Constructor. Setup all components.
     */
    CPU (unsigned char *_ram)
        : bus ()
        , PC (bus)
        , IR (bus, 0x0f)
        , OUT (bus)
    {
        micro_ptr = 0;

        PC.reset ();
        IR.reset ();

        halted = 0;
    };

public:
    virtual int clock (int debug_level) = 0;
};

class CPU16 : CPU {
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
    ,   R_TL
    ,   R_TR
    ,   R_ALU
    ,   R_OUT
    ,   R_SP
    ,   R_FP
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
        I_SETFP  = 0
    ,   I_SETSP
    ,   I_PUSHI
    ,   I_PUSHG
    ,   I_PUSHL
    ,   I_POPG
    ,   I_POPL
    ,   I_CALLI
    ,   I_CALL
    ,   I_RET
    ,   I_MATH

    ,   I_PSH
    ,   I_POP
    ,   I_R0D
    ,   I_R0E
    ,   I_OUT
    ,   I_HLT
    ,   I_NUM
    };

    struct InstructionInfo {
        const char *name;
        char argument;
        char cond;
    };

    static const InstructionInfo instruction_info[I_NUM];

    /****************************************************************************
     * ALU - Arithmetic & Logical Unit. Now only Add & Subtract
     *       Connected to A, TMP and F registers
     ****************************************************************************/
    class Alu16 : public Register16 {
    private:
        Register16 &TL, &TR;
        FlagsRegister &F;
        char oper;
    public:
        enum Operation { OpNone, OpInd, OpAdd, OpSub };
        static const char *OpNames[];

        Alu16 (InternalBus16 &bus, Register16 &_TL, Register16 &_TR, FlagsRegister &_F)
            : Register16 (bus)
            , TL(_TL)
            , TR(_TR)
            , F(_F)
        {
            oper = OpNone;
        };

        void SetOperation (char _oper, char _ir)
        {
            oper = (_oper == OpInd ? _ir & 0x07 : _oper);
        };
        void enable (int partial);
    };

    /****************************************************************************
     * One micocode instruction:
     * - enable:    register to pass data to   the internal bus
     * - load:      register to get  data from the internal bus
     * - clk_pc:    Clock (Inc/Dec) the PC
     * - clk_sp:    Clock (Inc/Dec) the SP
     * - clk_mar:   Clock (Inc/Dec) the MAR
     * - oper:      specify ALU operation
     * - halt:      stop execution
     * - cond:      requirement for C/Z
     * - pen:       enable 8 bits of full 16 bits
     * - pld:       load   8 bits of full 16 bits
     * - end_instr: end of microcode sequence, start next one
     ****************************************************************************/
    struct MicroInstruction {
        char enable;
        char load;
        char clk_pc;
        char clk_sp;
        char oper;
        char halt;
        char cond;
        char pen;
        char pld;
        char clk_mar;
        char end_instr;
    };

    Register16 TL, TR;
    Counter16 SP;
    Register16 FP;
    RamRegister8 RAM;
    MemAddrRegister16 MAR;
    Alu16 ALU;
    FlagsRegister F;

    Register16 *registers[R_NUM];

    static const struct MicroInstruction prefix[];
    static const struct MicroInstruction inst_setfp[];
    static const struct MicroInstruction inst_setsp[];
    static const struct MicroInstruction inst_pushi[];
    static const struct MicroInstruction inst_pushg[];
    static const struct MicroInstruction inst_pushl[];
    static const struct MicroInstruction inst_popg[];
    static const struct MicroInstruction inst_popl[];
    static const struct MicroInstruction inst_calli[];
    static const struct MicroInstruction inst_call[];
    static const struct MicroInstruction inst_ret[];
    static const struct MicroInstruction inst_math[];
    static const struct MicroInstruction inst_sti[];
    static const struct MicroInstruction inst_psh[];
    static const struct MicroInstruction inst_pop[];
    static const struct MicroInstruction inst_out[];
    static const struct MicroInstruction inst_hlt[];

    struct MicroInstruction *instructions[16];

    struct MicroInstruction *initMicroInstruction (const struct MicroInstruction *_inst);

    /*
     * Initialize register array
     */
    void initRegisters (void);

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
    void debug (u_int16_t addr) const;

    /*
     * Debugging output at microcode_level
     */
    void debug_microcode (const struct MicroInstruction *ip, unsigned char ir) const;

    inline unsigned char map_reg (unsigned char reg, unsigned char regid) const
    {
        regid = (regid & 0x0f) % 5;
        if (reg != CPU16::R_ANY) return reg;

        return R_SP + regid;
    };
    void map_cond (unsigned char data, char &zero, char &carry, const char *&name) const;
    /*
     * Execute one microcode instruction
     */
    int execute_microcode(const struct MicroInstruction *ip, unsigned char ir, int debug_level);

public:
    /*
     * Constructor. Setup all components.
     */
    CPU16 (unsigned char *_ram)
        : CPU (_ram)
        , RAM (bus, 65536, _ram)
        , MAR (bus, RAM)
        , SP (bus)
        , FP (bus)
        , TL (bus)
        , TR (bus)
        , ALU (bus, TL, TR, F)
    {
        initRegisters ();
        initMicroInstructions ();
    };

    int clock (int debug_level);
};

#endif
