#include <iostream>
#include <boost/format.hpp>
#include <cstddef>

using namespace std; 

/****************************************************************************
 * Internal CPU bus
 ****************************************************************************/
class InternalBus {
private:
    unsigned char value;
public:
    inline void write (unsigned char _value) {
        value = _value;
    };
    inline unsigned char read (void) const {
        return value;
    };
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

/****************************************************************************
 * General Register - is connected to the InternalBus
 ****************************************************************************/
class Register {
private:
    InternalBus &bus;
    char enable_mask, load_mask;
protected:
    char value;
public:
    Register (InternalBus &_bus, char _enable_mask = 0xff, char _load_mask = 0xff) : bus (_bus) {
        enable_mask = _enable_mask;
        load_mask   = _load_mask;
    };
    void reset (void) {
        value = 0;
    };
    virtual void enable (void) {
        bus.write (value & enable_mask);
    };
    virtual void load (void) {
        value = bus.read () & load_mask;
    };
    virtual unsigned char get (void) {
        return value;
    };
};

/****************************************************************************
 * Output Register - no LED's but some output nontheless
 ****************************************************************************/
class OutputRegister: public Register {
public:
    OutputRegister (InternalBus &bus): Register (bus) { };

    void load (void) {
        Register::load ();

        cout << "OUTPUT:" << int(value) << endl;
    }
};

/****************************************************************************
 * Counter Register - currently only PC, could be SP in future too
 ****************************************************************************/
class Counter: public Register {
public:
    Counter (InternalBus &bus): Register (bus) { };
    void clock (void) {
        value++;
    };
};

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

const char *Alu::OpNames[] = { "-", "ADD", "SUB" };

/****************************************************************************
 * Ram Register - is connected to the RAM (data)
 ****************************************************************************/
class RamRegister : public Register {
    unsigned char addr;
    unsigned char *data;
private:
    unsigned char truncate (unsigned char _addr) const {
        return _addr & 0x0f;
    }
public:
    RamRegister (InternalBus &bus, unsigned char *_data) : Register (bus) {
        addr = 0;
        data = _data;
    };

    void set_addr (char _addr) {
        addr = truncate (_addr);
    }

    void enable (void) {
        value = data[addr];

        Register::enable ();
    };

    void load (void) {
        Register::load ();

        data[addr] = value;
    };

    unsigned char get (unsigned char _addr) const {
        return data[truncate (_addr)];
    };
};

/****************************************************************************
 * Memory Address Register - passes address info to RAM
 ****************************************************************************/
class MemAddrRegister : public Register {
private:
    RamRegister &RAM;
public:
    MemAddrRegister (InternalBus &bus, RamRegister &_RAM) : Register (bus), RAM(_RAM) { };

    void load (void) {
        Register::load ();
        RAM.set_addr (value);
    };
};

/****************************************************************************
 * Instead of using string assiciative arrays, use numbers for all registers
 ****************************************************************************/
enum RegisterID {
    R_none = 0
,   R_PC
,   R_IR
,   R_MAR
,   R_RAM
,   R_A
,   R_B
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

const RegisterInfo register_info[] = {
    {1, "-"}
,   {0, "PC"}
,   {1, "IR"}
,   {1, "MAR"}
,   {1, "RAM"}
,   {0, "A"}
,   {0, "B"}
,   {1, "ALU"}
,   {1, "OUT"}
};

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
 * Tie it all together to a functional CPU
 ****************************************************************************/
class CPU {
private:
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

    InternalBus bus;
    Counter PC;
    Register IR, A, B;
    OutputRegister OUT;
    RamRegister RAM;
    MemAddrRegister MAR;
    Alu ALU;
    FlagsRegister F;
    int halted;

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

    int micro_ptr;

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
    CPU (unsigned char *_ram)
        : bus ()
        , PC (bus)
        , IR (bus, 0x0f)
        , RAM (bus, _ram)
        , MAR (bus, RAM)
        , A (bus)
        , B (bus)
        , OUT (bus)
        , ALU (bus, A, B, F)
    {
        registers[R_PC]    = &PC;
        registers[R_IR]    = &IR;
        registers[R_MAR]   = &MAR;
        registers[R_RAM]   = &RAM;
        registers[R_A]     = &A;
        registers[R_B]     = &B;
        registers[R_ADDER] = &ALU;
        registers[R_OUT]   = &OUT;

        micro_ptr = 0;

        initMicroInstructions ();

        PC.reset ();
        IR.reset ();

        halted = 0;
    };

    int clock (int debug_level);
};

const struct CPU::MicroInstruction CPU::prefix[] = {
        {.enable    = R_PC,    .load      = R_MAR                           }
    ,   {.enable    = R_RAM,   .load      = R_IR,      .clock_pc  = 1       }
};

const struct CPU::MicroInstruction CPU::inst_nop[] = {
        {.end_instr = 1      }
};

const struct CPU::MicroInstruction CPU::inst_lda[] = {
        {.enable    = R_IR,    .load      = R_MAR                           }
    ,   {.enable    = R_RAM,   .load      = R_A,       .end_instr = 1       }
};

const struct CPU::MicroInstruction CPU::inst_add[] = {
        {.enable    = R_IR,    .load      = R_MAR                           }
    ,   {.enable    = R_RAM,   .load      = R_B,     .oper = Alu::OpAdd     }
    ,   {.enable    = R_ADDER, .load      = R_A,     .end_instr = 1         }
};

const struct CPU::MicroInstruction CPU::inst_sub[] = {
        {.enable    = R_IR,    .load      = R_MAR                           }
    ,   {.enable    = R_RAM,   .load      = R_B,     .oper = Alu::OpSub     }
    ,   {.enable    = R_ADDER, .load      = R_A,     .end_instr = 1         }
};

const struct CPU::MicroInstruction CPU::inst_sta[] = {
        {.enable    = R_IR,    .load      = R_MAR                           }
    ,   {.enable    = R_A,     .load      = R_RAM,   .end_instr = 1         }
};

const struct CPU::MicroInstruction CPU::inst_ldi[] = {
        {.enable    = R_IR,    .load      = R_A,     .end_instr = 1         }
};

const struct CPU::MicroInstruction CPU::inst_jmp[] = {
        {.enable    = R_IR,    .load      = R_PC,    .end_instr = 1         }
};

const struct CPU::MicroInstruction CPU::inst_jc[] = {
        {.enable    = R_IR,    .load      = R_PC,    .carry = FLAG_1        }
    ,   {                                              .end_instr = 1         }
};

const struct CPU::MicroInstruction CPU::inst_jz[] = {
        {.enable    = R_IR,    .load      = R_PC,    .zero = FLAG_1         }
    ,   {                                              .end_instr = 1         }
};

const struct CPU::MicroInstruction CPU::inst_out[] = {
        {.enable    = R_A,     .load      = R_OUT,   .end_instr = 1         }
};

const struct CPU::MicroInstruction CPU::inst_hlt[] = {
        {.halt      = 1,                               .end_instr = 1         }
};

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
};

struct InstructionInfo {
    const char *name;
    int argument;
};

const InstructionInfo instruction_info[] = {
    { "NOP", 0 }
,   { "LDA", 1 }
,   { "ADD", 1 }
,   { "SUB", 1 }
,   { "STA", 1 }
,   { "LDI", 1 }
,   { "JMP", 1 }
,   { "JC",  1 }
,   { "JZ",  1 }
,   { "R09", 0 }
,   { "R0A", 0 }
,   { "R0B", 0 }
,   { "R0C", 0 }
,   { "R0D", 0 }
,   { "OUT", 0 }
,   { "HLT", 0 }
,   { "NOP", 0 }
};

void CPU::initMicroInstructions (void)
{
    instructions[I_NOP] = initMicroInstruction (inst_nop);
    instructions[I_LDA] = initMicroInstruction (inst_lda);
    instructions[I_ADD] = initMicroInstruction (inst_add);
    instructions[I_SUB] = initMicroInstruction (inst_sub);
    instructions[I_STA] = initMicroInstruction (inst_sta);
    instructions[I_LDI] = initMicroInstruction (inst_ldi);
    instructions[I_JMP] = initMicroInstruction (inst_jmp);
    instructions[I_JC]  = initMicroInstruction (inst_jc);
    instructions[I_JZ]  = initMicroInstruction (inst_jz);
    instructions[I_R09] = initMicroInstruction (inst_hlt);
    instructions[I_R0A] = initMicroInstruction (inst_hlt);
    instructions[I_R0B] = initMicroInstruction (inst_hlt);
    instructions[I_R0C] = initMicroInstruction (inst_hlt);
    instructions[I_R0D] = initMicroInstruction (inst_hlt);
    instructions[I_OUT] = initMicroInstruction (inst_out);
    instructions[I_HLT] = initMicroInstruction (inst_hlt);
};

void CPU::dump_registers (const char *prefix, int internal) const
{
    int i;

    cout << prefix;
    for (i = 1; i < R_NUM; i++) {
        if (internal != register_info[i].internal) continue;
        cout << boost::format("%s:%02X ") % register_info[i].name % int(registers[i]->get());
    }
}

void CPU::debug (int addr) const
{
    unsigned char data;
    int i;
    const InstructionInfo *info;

    data = RAM.get (addr);
    info = instruction_info + (data >> 4);

    dump_registers (" » ", 0);

    cout << boost::format(" [%02X] %02X  %-3s") % addr % int(data) % info->name;
    if (info->argument) cout << boost::format(" 0x%02X") % (data & 0x0f);
    cout << endl;
};

void CPU::debug_microcode (const struct MicroInstruction *ip) const
{
    char Z, C;

    F.get (Z, C);

    dump_registers ("   » ", 1);
    cout <<  "BUS:" << boost::format("%02X") % int (bus.read ())
         <<  ", C:"   << int(C)
         <<  ", Z:"   << int(Z)
         <<  boost::format("  [%02X]") % int(micro_ptr);
    if (ip->enable) {
        cout <<  boost::format(" enable:%-3s") % register_info[ip->enable].name;
    }
    if (ip->load) {
        cout <<  boost::format(" load:%-3s") % register_info[ip->load].name;
    }
    if (ip->clock_pc) {
        cout <<  " clockPC";
    }
    if (ip->oper) {
        cout <<  " operation:" << Alu::OpNames[ip->oper];
    }
    if (ip->carry) {
        cout <<  " C=" << (ip->carry-1);
    }
    if (ip->zero) {
        cout <<  " Z=" << (ip->zero-1);
    }
    if (ip->halt) {
        cout <<  " HALT";
    }
    if (ip->end_instr) {
        cout <<  " END";
    }
    cout << endl;
};

/*
 * Execute one microcode instruction
 */
int CPU::execute_microcode(const struct MicroInstruction *ip, int debug_level)
{
    char Z, C;

    F.get (Z, C);

    if (debug_level && !micro_ptr) debug (PC.get ());
    if (debug_level >= 2) debug_microcode (ip);

    if (ip->carry && C != ip->carry -1) return 0;
    if (ip->zero  && Z != ip->zero  -1) return 0;

    if (ip->oper)      ALU.SetOperation (ip->oper);
    if (ip->enable)    registers[ip->enable]->enable ();
    if (ip->load)      registers[ip->load]->load ();
    if (ip->clock_pc)  PC.clock ();
    if (ip->halt)      halted = 1;

    return ip->end_instr;
};

struct CPU::MicroInstruction *CPU::initMicroInstruction (const struct MicroInstruction *_inst)
{
    const int prefix_sz = sizeof(prefix)/sizeof(MicroInstruction);

    const struct MicroInstruction *i;
    struct MicroInstruction *j, *ret;

    for (i = _inst; !i->end_instr; i++);

    ret = new MicroInstruction[prefix_sz + (i - _inst) + 1];

    for (i = prefix, j = ret; i < prefix + prefix_sz; *j++ = *i++);
    for (i = _inst; !i->end_instr; *j++ = *i++);
    *j++ = *i++;

    return ret;
};

int CPU::clock (int debug_level)
{
    const struct MicroInstruction *ip;

    ip = instructions[(IR.get() >> 4) & 0xf] + micro_ptr;

    if (execute_microcode(ip, debug_level)) {
        micro_ptr = 0;
    } else {
        micro_ptr++;
    }

    return halted;
};

int main (void)
{
    unsigned char ram[16] = {0x51, 0x4e, 0x55, 0xe0, 0x3e, 0xe0, 0x88, 0x64, 0xf0};

    CPU cpu (ram);

    while (!cpu.clock (2)) {};
}
