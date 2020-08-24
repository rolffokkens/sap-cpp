#include <string>
#include "cpu16d.h"

const char *CPU16::Alu16::OpNames[] = { "-", "IND", "ADD", "SUB" };

const char CPU::FlagsRegister::map_flags[] = {
     (                               COMP_GE | COMP_GT | COMP_NE )
,    (           COMP_LE | COMP_EQ | COMP_GE                     )
,    ( COMP_LT | COMP_LE                               | COMP_NE )
,    ( 0                                                         )
};

const struct CPU16::MicroInstruction CPU16::prefix[] = {
        {.enable = R_PC,  .load = R_MAR                                                 }
,       {.enable = R_MDR, .load = R_IR,  .clk_pc = 1,                   .clk_mar = 1    }
};

const struct CPU16::MicroInstruction CPU16::inst_setfp[] = {
        {.enable = R_MDR, .load = R_FP,  .clk_pc = 1, .pld = REG_LOW,   .clk_mar = 1    }
,       {.enable = R_MDR, .load = R_FP,  .clk_pc = 1, .pld = REG_HIGH                   }
,       {.enable = R_FP,  .load = R_SP,                                 .end_instr = 1  }
};

const struct CPU16::MicroInstruction CPU16::inst_setsp[] = {
        {.enable = R_MDR, .load = R_TR,  .clk_pc = 1, .pld = REG_LOW,   .clk_mar = 1    }
,       {.enable = R_MDR, .load = R_TR,  .clk_pc = 1, .pld = REG_HIGH                   }
,       {.enable = R_FP,  .load = R_TL,   .oper = Alu16::OpAdd                          }
,       {.enable = R_ALU, .load = R_SP,                                 .end_instr = 1  }
};

const struct CPU16::MicroInstruction CPU16::inst_ldtrc[] = {
        {.enable = R_MDR, .load = R_TR,  .clk_pc = 1, .pld = REG_LOW,   .clk_mar = 1    }
,       {.enable = R_MDR, .load = R_TR,  .clk_pc = 1, .pld = REG_HIGH,  .end_instr = 1  }
};

const struct CPU16::MicroInstruction CPU16::inst_ldtrl[] = {
        {.enable = R_MDR, .load = R_TR,  .clk_pc = 1, .pld = REG_LOW,   .clk_mar = 1    }
,       {.enable = R_MDR, .load = R_TR,  .clk_pc = 1, .pld = REG_HIGH                   }
,       {.enable = R_ALU2,.load = R_TR,                                 .end_instr = 1  }
};

const struct CPU16::MicroInstruction CPU16::inst_fetch[] = {
        {.enable = R_TR,  .load = R_MAR                                                 }
,       {.enable = R_MDR, .load = R_TR,               .pld = REG_LOW,   .clk_mar = 1    }
,       {.enable = R_MDR, .load = R_TR,               .pld = REG_HIGH,  .end_instr = 1  }
};

const struct CPU16::MicroInstruction CPU16::inst_store[] = {
        {.enable = R_SP,  .load = R_MAR                                                 }
,       {.enable = R_MDR, .load = R_TL,  .clk_sp = 1, .pld = REG_LOW,   .clk_mar = 1    }
,       {.enable = R_MDR, .load = R_TL,  .clk_sp = 1, .pld = REG_HIGH,  .clk_mar = 1    }
,       {.enable = R_TR,  .load = R_MAR                                                 }
,       {.enable = R_TL,  .load = R_MDR,              .pen = REG_LOW,   .clk_mar = 1    }
,       {.enable = R_TL,  .load = R_MDR,              .pen = REG_HIGH,  .end_instr = 1  }
};

const struct CPU16::MicroInstruction CPU16::inst_push[] = {
        {.enable = R_SP,  .load = R_MAR, .clk_sp =-1,                   .clk_mar =-1    }
,       {.enable = R_TR,  .load = R_MDR, .clk_sp =-1, .pen = REG_HIGH,  .clk_mar =-1    }
,       {.enable = R_TR,  .load = R_MDR,              .pen = REG_LOW,   .end_instr = 1  }
};

const struct CPU16::MicroInstruction CPU16::inst_pop[] = {
        {.enable = R_SP,  .load = R_MAR                                                 }
,       {.enable = R_MDR, .load = R_TR,  .clk_sp = 1, .pld = REG_LOW,   .clk_mar = 1    }
,       {.enable = R_MDR, .load = R_TR,  .clk_sp = 1, .pld = REG_HIGH,  .end_instr = 1  }
};

const struct CPU16::MicroInstruction CPU16::inst_call[] = {
        {.enable = R_SP,  .load = R_MAR, .clk_sp =-1,                   .clk_mar =-1    }
,       {.enable = R_PC,  .load = R_MDR, .clk_sp =-1, .pen = REG_HIGH,  .clk_mar =-1    }
,       {.enable = R_PC,  .load = R_MDR, .clk_sp =-1, .pen = REG_LOW,   .clk_mar =-1    }
,       {.enable = R_FP,  .load = R_MDR, .clk_sp =-1, .pen = REG_HIGH,  .clk_mar =-1    }
,       {.enable = R_FP,  .load = R_MDR,              .pen = REG_LOW,                   }
,       {.enable = R_SP,  .load = R_FP,                                                 }
,       {.enable = R_TR,  .load = R_PC,                                 .end_instr = 1  }
};

const struct CPU16::MicroInstruction CPU16::inst_ret[] = {
        {.enable = R_FP,  .load = R_SP,                                                 }
,       {.enable = R_SP,  .load = R_MAR                                                 }
,       {.enable = R_MDR, .load = R_FP,  .clk_sp = 1, .pld = REG_LOW,   .clk_mar = 1    }
,       {.enable = R_MDR, .load = R_FP,  .clk_sp = 1, .pld = REG_HIGH,  .clk_mar = 1    }
,       {.enable = R_MDR, .load = R_PC,  .clk_sp = 1, .pld = REG_LOW,   .clk_mar = 1    }
,       {.enable = R_MDR, .load = R_PC,  .clk_sp = 1, .pld = REG_HIGH,  .end_instr = 1  }
};

const struct CPU16::MicroInstruction CPU16::inst_math[] = {
        {.enable = R_SP,  .load = R_MAR, .oper = Alu16::OpInd, .flags = 1               }
,       {.enable = R_MDR, .load = R_TL,  .clk_sp = 1, .pld = REG_LOW,   .clk_mar = 1    }
,       {.enable = R_MDR, .load = R_TL,  .clk_sp = 1, .pld = REG_HIGH,  .clk_mar = 1    }
,       {.enable = R_ALU, .load = R_TR,               .pen = REG_LOW,   .end_instr = 1  }
};

const struct CPU16::MicroInstruction CPU16::inst_cmp[] = {
        {.enable = R_SP,  .load = R_MAR, .oper = Alu16::OpSub, .flags = 1               }
,       {.enable = R_MDR, .load = R_TL,  .clk_sp = 1, .pld = REG_LOW,   .clk_mar = 1    }
,       {.enable = R_MDR, .load = R_TL,  .clk_sp = 1, .pld = REG_HIGH,  .clk_mar = 1    }
,       {.enable = R_ALU, .load = R_TL,                                                 }
,       {.enable = R_ONE, .load = R_TR,  .cond = 1,                     .end_instr = 1  }
,       {.enable = R_ZERO,.load = R_TR,                                 .end_instr = 1  }
,       {                                                               .end_instr = 1  }
};

const struct CPU16::MicroInstruction CPU16::inst_out[] = {
        {.enable = R_TR,  .load = R_OUT                                                 }
,       {                                                               .end_instr = 1  }
};

const struct CPU16::MicroInstruction CPU16::inst_jump[] = {
        {.enable = R_TR,  .load = R_PC,  .cond = 1                                      }
,       {                                                               .end_instr = 1  }
};

const struct CPU16::MicroInstruction CPU16::inst_halt[] = {
        {.halt      = 1,                                                .end_instr = 1  }
};

const CPU16::RegisterInfo CPU16::register_info[] = {
    {1, "-"}
,   {0, "PC"}
,   {1, "IR"}
,   {1, "MAR"}
,   {1, "MDR"}
,   {1, "TL"}
,   {0, "TR"}
,   {1, "ALU"}
,   {1, "OUT"}
,   {0, "SP"}
,   {0, "FP"}
,   {1, "ALU2"}
,   {1, "ZERO"}
,   {1, "ONE"}
};

const CPU16::InstructionInfo CPU16::instruction_info[] = {
    { "SETFP",  1, 0 }
,   { "SETSP",  1, 0 }
,   { "LDTRC",  1, 0 }
,   { "LDTRL",  1, 0 }
,   { "FETCH",  0, 0 }
,   { "STORE",  0, 0 }
,   { "PUSH",   0, 0 }
,   { "POP",    0, 0 }
,   { "CALL",   0, 0 }
,   { "RET",    0, 0 }
,   { "MATH",   0, 0 }
,   { "CMP",    0, 1 }
,   { "SWAP",   0, 0 }
,   { "OUT",    0, 0 }
,   { "JUMP",   0, 1 }
,   { "HALT",   0, 0 }
};

void CPU16::initRegisters (void)
{
    registers[R_PC]   = &PC;
    registers[R_IR]   = &IR;
    registers[R_MAR]  = &MAR;
    registers[R_MDR]  = &MDR;
    registers[R_TL]   = &TL;
    registers[R_TR]   = &TR;
    registers[R_ALU]  = &ALU;
    registers[R_OUT]  = &OUT;
    registers[R_SP]   = &SP;
    registers[R_FP]   = &FP;
    registers[R_ALU2] = &ALU2;
    registers[R_ZERO] = &ZERO;
    registers[R_ONE]  = &ONE;
};

void CPU16::initMicroInstructions (void)
{
    instructions[I_SETFP]  = initMicroInstruction (inst_setfp);
    instructions[I_SETSP]  = initMicroInstruction (inst_setsp);
    instructions[I_LDTRC]  = initMicroInstruction (inst_ldtrc);
    instructions[I_LDTRL]  = initMicroInstruction (inst_ldtrl);
    instructions[I_FETCH]  = initMicroInstruction (inst_fetch);
    instructions[I_STORE]  = initMicroInstruction (inst_store);
    instructions[I_PUSH]   = initMicroInstruction (inst_push);
    instructions[I_POP]    = initMicroInstruction (inst_pop);
    instructions[I_CALL]   = initMicroInstruction (inst_call);
    instructions[I_RET]    = initMicroInstruction (inst_ret);
    instructions[I_MATH]   = initMicroInstruction (inst_math);
    instructions[I_CMP]    = initMicroInstruction (inst_cmp);
    instructions[I_SWAP]   = initMicroInstruction (inst_halt);
    instructions[I_OUT]    = initMicroInstruction (inst_out);
    instructions[I_JUMP]   = initMicroInstruction (inst_jump);

    instructions[I_HALT]   = initMicroInstruction (inst_halt);
};

void CPU16::dump_registers (const char *prefix, int internal) const
{
    int i;
    char Z, C;

    F.get (Z, C);

    cout << prefix;
    for (i = 1; i < R_NUM; i++) {
        if (!internal && register_info[i].internal) continue;
        cout << boost::format("%s:%04X ") % register_info[i].name % int(registers[i]->get());
    }
    cout <<  "c:" << int(C) <<  " z:" << int(Z) << " ";
};

void CPU16::map_cond (unsigned char data, char &zero, char &carry, char &condflags, const char *&name) const
{
    static struct {int zero; int carry; const char *name;} cond_maps[] = {
        { FLAG_NONE, FLAG_NONE, NULL }
    ,   { FLAG_NONE, FLAG_NONE, NULL }
    ,   { FLAG_NONE, FLAG_NONE, NULL }
    ,   { FLAG_NONE, FLAG_NONE, NULL }
    ,   { FLAG_1,    FLAG_NONE, "Z"  }
    ,   { FLAG_0,    FLAG_NONE, "NZ" }
    ,   { FLAG_NONE, FLAG_1,    "C"  }
    ,   { FLAG_NONE, FLAG_0,    "NC" }
    };
    static struct {char flags; const char *name;} cond_flags[] = {
        { 0x7f,         NULL }
    ,   { CPU::COMP_LT, "LT" }
    ,   { CPU::COMP_LE, "LE" }
    ,   { CPU::COMP_EQ, "EQ" }
    ,   { CPU::COMP_GE, "GE" }
    ,   { CPU::COMP_GT, "GT" }
    ,   { CPU::COMP_NE, "NE" }
    ,   { 0,            NULL }
    };

    char i = data & 0x07;

    if (data & 0x08) {
        zero  = FLAG_NONE;
        carry = FLAG_NONE;

        name      = cond_flags[i].name;
        condflags = cond_flags[i].flags;
    } else {
        zero      = cond_maps[i].zero;
        carry     = cond_maps[i].carry;
        name      = cond_maps[i].name;

        condflags = 0;
    };
};

static const char *instrs [] =
{    ""
,    ""
,    "ADD"
,    "SUB"
};

void CPU16::debug (u_int16_t addr) const
{
    unsigned char data;
    char condflags;
    u_int16_t fp;
    int i, i_instr;
    const InstructionInfo *info;
    const char *sep = " ", *instr;

    data    = RAM.get (addr, 1);
    i_instr = data >> 4;
    info    = instruction_info + (i_instr);

    instr   = info->name;
    if (i_instr == I_MATH) {
        instr = instrs[data & 0x07];
    }

    dump_registers (" » ", 0);

    cout << boost::format(" [%04X] %02X  %-3s") % addr % int(data) % instr;
    if (info->cond) {
        unsigned char cond = data & 0x0f;
        char zero, carry;
        const char *name;

        map_cond (cond, zero, carry, condflags, name);
        if (name) {
            cout << sep << name;
            sep = ",";
        }

    }
    if (info->argument) {
        u_int16_t arg;
        arg  = RAM.get (++addr, 1);
        arg |= RAM.get (++addr, 1) << 8;
        cout << sep << boost::format("0x%04X") % int (arg);
    }
    cout << endl;
    addr = SP.get();
    fp   = FP.get();

    cout << "   [SP]";
    for (i = 16; i; i--, addr++) {
        cout << boost::format(" %s%02X") % (addr == fp ? "*" : " ") % int (RAM.get (addr, 1));
    }
    cout << endl;
};

static std::string gen_regname (const char *name, int hl)
{
    std::string buf = name;

    if (hl == REG_LOW || hl == REG_HIGH) {
        buf += (hl == REG_LOW ? ".L" : ".H");
    }

    return buf;
}

void CPU16::debug_microcode (const struct MicroInstruction *ip, unsigned char ir) const
{
    char Z, C;
    unsigned char reg;

    F.get (Z, C);

    dump_registers ("   » ", 1);
    cout <<  "BUS:" << boost::format("%04X") % int (bus.read ())
         <<  boost::format("  [%02X]") % int(micro_ptr);
    if (ip->enable) {
        reg = map_reg (ip->enable, ir);
        cout << boost::format(" enable:%-5s") % gen_regname (register_info[reg].name, ip->pen);
    }
    if (ip->load) {
        reg = map_reg (ip->load,   ir);
        cout <<  boost::format(" load:%-5s") % gen_regname (register_info[reg].name, ip->pld);
    }
    if (ip->clk_pc) {
        cout <<  " PC" << (ip->clk_pc == 1 ? "++" : "--");
    }
    if (ip->clk_sp) {
        cout <<  " SP" << (ip->clk_sp == 1 ? "++" : "--");
    }
    if (ip->clk_mar) {
        cout <<  " MAR" << (ip->clk_mar == 1 ? "++" : "--");
    }
    if (ip->oper) {
        int oper = (ip->oper == Alu16::OpInd ? ir : ip->oper);
        cout <<  " operation:" << Alu16::OpNames[oper & 0x07];
        if (ip->flags) cout << ".F";
    }
    if (ip->cond) {
        const char *name;
        char zero, carry;
        char condflags;
        map_cond (ir & 0x0f, zero, carry, condflags, name);
        if (name) cout << " [" << name << "]";
    }
    if (ip->halt) {
        cout <<  " HALT";
    }
    if (ip->end_instr) {
        cout <<  " END";
    }
    cout << endl;
};

struct CPU16::MicroInstruction *CPU16::initMicroInstruction (const struct CPU16::MicroInstruction *_inst)
{
    const int prefix_sz = sizeof(prefix)/sizeof(MicroInstruction);

    const struct MicroInstruction *i;
    struct MicroInstruction *j, *ret;

    for (i = _inst; i->cond || !i->end_instr; i++);

    ret = new MicroInstruction[prefix_sz + (i - _inst) + 1];

    for (i = prefix, j = ret; i < prefix + prefix_sz; *j++ = *i++);
    /* Duplicate micro instructions until unconditional end_instr */
    for (i = _inst; i->cond || !i->end_instr; *j++ = *i++);
    *j++ = *i++;

    return ret;
};

/*
 * Execute one microcode instruction
 */
int CPU16::execute_microcode(const struct MicroInstruction *ip, unsigned char ir, int debug_level)
{
    char Z,    C;
    char zero, carry;
    char condflags;

    F.get (Z, C);

    if (debug_level && !micro_ptr) debug (PC.get ());
    if (debug_level >= 2) debug_microcode (ip, ir);

    if (ip->cond) {
        const char *name;
        map_cond (ir & 0x0f, zero, carry, condflags, name);
    } else {
        zero      = FLAG_NONE;
        carry     = FLAG_NONE;
        condflags = 0;
    }

    if (carry     && C != carry -1            ) return 0;
    if (zero      && Z != zero  -1            ) return 0;
    if (condflags && !F.match_comp (condflags)) return 0;

    if (ip->oper)    ALU.SetOperation (ip->oper, ir, ip->flags);
    if (ip->enable)  registers[map_reg (ip->enable, ir)]->enable (ip->pen);
    if (ip->load)    registers[map_reg (ip->load,   ir)]->load   (ip->pld);
    if (ip->clk_pc)  PC.clock  (ip->clk_pc);
    if (ip->clk_sp)  SP.clock  (ip->clk_sp);
    if (ip->clk_mar) MAR.clock (ip->clk_mar);
    if (ip->halt)    halted = 1;

    return ip->end_instr;
};

int CPU16::clock (int debug_level)
{
    const struct MicroInstruction *ip;
    unsigned char ir = IR.get();

    ip = instructions[(ir >> 4) & 0xf] + micro_ptr;

    if (execute_microcode(ip, ir, debug_level)) {
        micro_ptr = 0;
    } else {
        micro_ptr++;
    }

    return halted;
};

void CPU16::MemDataRegister::enable (int partial) {
    value = RAM.get(MAR.get());

    Register16::enable (partial);
};

void CPU16::MemDataRegister::load (int partial) {
    Register16::load (partial);

    RAM.set (MAR.get(), value);
};

void CPU16::Alu16::enable (int partial) {
    int result;
    char Z, C;

    switch (oper) {
    case OpAdd:
        result = TL.get () + TR.get ();
        break;
    case OpSub:
        result = TL.get () - TR.get ();
        break;
    };

    value = result & 0xffff;

    if (flags) {
        C = (value != result);
        Z = (value == 0);
        F.set (Z, C);
    }

    Register16::enable (partial);
};

