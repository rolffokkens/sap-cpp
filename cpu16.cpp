#include "cpu16.h"

const char *CPU16::Alu16::OpNames[] = { "-", "IND", "ADD", "SUB" };

const struct CPU16::MicroInstruction CPU16::prefix[] = {
        {.enable = R_PC,  .load = R_MAR                                                 }
,       {.enable = R_RAM, .load = R_IR,  .clk_pc = 1,                    .clk_mar = 1 }
};

const struct CPU16::MicroInstruction CPU16::inst_setfp[] = {
        {.enable = R_RAM, .load = R_FP,  .clk_pc = 1, .pld = REG_LOW,    .clk_mar = 1  }
,       {.enable = R_RAM, .load = R_FP,  .clk_pc = 1, .pld = REG_HIGH                 }
,       {.enable = R_FP,  .load = R_SP,                                  .end_instr = 1 }
};

const struct CPU16::MicroInstruction CPU16::inst_setsp[] = {
        {.enable = R_RAM, .load = R_TR,  .clk_pc = 1, .pld = REG_LOW,    .clk_mar = 1   }
,       {.enable = R_RAM, .load = R_TR,  .clk_pc = 1, .pld = REG_HIGH                 }
,       {.enable = R_FP,  .load = R_TL,   .oper = Alu16::OpAdd                          }
,       {.enable = R_ALU, .load = R_SP,                                  .end_instr = 1 }
};

const struct CPU16::MicroInstruction CPU16::inst_pushi[] = {
        {.enable = R_RAM, .load = R_TR,  .clk_pc = 1, .pld = REG_LOW,   .clk_mar = 1    }
,       {.enable = R_RAM, .load = R_TR,  .clk_pc = 1, .clk_sp =-1, .pld = REG_HIGH                 }
,       {.enable = R_SP,  .load = R_MAR                                                 }
,       {.enable = R_TR,  .load = R_RAM, .clk_sp =-1, .pen = REG_HIGH, .clk_mar =-1 }
,       {.enable = R_TR,  .load = R_RAM,              .pen = REG_LOW,  .end_instr = 1 }
};

const struct CPU16::MicroInstruction CPU16::inst_pushg[] = {
        {.enable = R_PC,  .load = R_MAR                                                 }
,       {.enable = R_RAM, .load = R_TR,  .clk_pc = 1, .pld = REG_LOW                  }
,       {.enable = R_PC,  .load = R_MAR                                                 }
,       {.enable = R_RAM, .load = R_TR,  .clk_pc = 1, .pld = REG_HIGH                 }
,       {.enable = R_TR,  .load = R_MAR, .clk_sp =-1                                    }
,       {.enable = R_RAM, .load = R_TR,               .pld = REG_LOW,  .clk_mar = 1    }
,       {.enable = R_RAM, .load = R_TR,               .pld = REG_HIGH,                 }
,       {.enable = R_SP,  .load = R_MAR                                                 }
,       {.enable = R_TR,  .load = R_RAM, .clk_sp =-1, .pen = REG_HIGH, .clk_mar =-1 }
,       {.enable = R_TR,  .load = R_RAM,              .pen = REG_LOW,  .end_instr = 1 }
};

const struct CPU16::MicroInstruction CPU16::inst_pushl[] = {
        {.enable = R_PC,  .load = R_MAR                                                 }
,       {.enable = R_RAM, .load = R_TR,  .clk_pc = 1, .pld = REG_LOW                  }
,       {.enable = R_PC,  .load = R_MAR, .clk_sp =-1                                    }
,       {.enable = R_RAM, .load = R_TR,  .clk_pc = 1, .pld = REG_HIGH                 }
,       {.enable = R_FP,  .load = R_TL,  .oper = Alu16::OpAdd                           }
,       {.enable = R_ALU, .load = R_MAR                                                 }
,       {.enable = R_RAM, .load = R_TR,               .pld = REG_LOW,  .clk_mar = 1    }
,       {.enable = R_RAM, .load = R_TR,               .pld = REG_HIGH,                 }
,       {.enable = R_SP,  .load = R_MAR                                                 }
,       {.enable = R_TR,  .load = R_RAM, .clk_sp =-1, .pen = REG_HIGH, .clk_mar =-1 }
,       {.enable = R_TR,  .load = R_RAM,              .pen = REG_LOW,  .end_instr = 1 }
};

const struct CPU16::MicroInstruction CPU16::inst_popg[] = {
        {.enable = R_RAM, .load = R_TR,  .clk_pc = 1, .pld = REG_LOW,  .clk_mar = 1     }
,       {.enable = R_RAM, .load = R_TR,  .clk_pc = 1, .pld = REG_HIGH                 }
,       {.enable = R_SP,  .load = R_MAR                                                 }
,       {.enable = R_RAM, .load = R_TL,  .clk_sp = 1, .pld = REG_LOW,  .clk_mar = 1    }
,       {.enable = R_RAM, .load = R_TL,  .clk_sp = 1, .pld = REG_HIGH                  }
,       {.enable = R_TR,  .load = R_MAR                                                 }
,       {.enable = R_TL,  .load = R_RAM,              .pen = REG_LOW,  .clk_mar = 1    }
,       {.enable = R_TL,  .load = R_RAM,              .pen = REG_HIGH, .end_instr = 1 }
};

const struct CPU16::MicroInstruction CPU16::inst_popl[] = {
        {.enable = R_RAM, .load = R_TR,  .clk_pc = 1, .pld = REG_LOW,  .clk_mar = 1     }
,       {.enable = R_RAM, .load = R_TR,  .clk_pc = 1, .pld = REG_HIGH                 }
,       {.enable = R_FP,  .load = R_TL, .oper = Alu16::OpAdd                           }
,       {.enable = R_ALU, .load = R_TR                                                 }
,       {.enable = R_SP,  .load = R_MAR                                                 }
,       {.enable = R_RAM, .load = R_TL,  .clk_sp = 1, .pld = REG_LOW,  .clk_mar = 1    }
,       {.enable = R_RAM, .load = R_TL,  .clk_sp = 1, .pld = REG_HIGH                  }
,       {.enable = R_TR,  .load = R_MAR                                                 }
,       {.enable = R_TL,  .load = R_RAM,              .pen = REG_LOW,  .clk_mar = 1    }
,       {.enable = R_TL,  .load = R_RAM,              .pen = REG_HIGH, .end_instr = 1 }
};

const struct CPU16::MicroInstruction CPU16::inst_calli[] = {
        {.enable    = R_ANY,   .load      = R_TR,                    .end_instr = 1 }
};

const struct CPU16::MicroInstruction CPU16::inst_call[] = {
        {.enable    = R_TR,     .load      = R_ANY,                  .end_instr = 1 }
};

const struct CPU16::MicroInstruction CPU16::inst_ret[] = {
        {.enable    = R_TR,     .load      = R_MAR,                                 }
,       {.enable    = R_RAM,   .load      = R_ANY,                  .end_instr = 1 }
};

const struct CPU16::MicroInstruction CPU16::inst_math[] = {
        {.enable = R_SP,  .load = R_MAR, .oper = Alu16::OpInd                           }
,       {.enable = R_RAM, .load = R_TL,  .clk_sp = 1, .pld = REG_LOW,  .clk_mar = 1     }
,       {.enable = R_RAM, .load = R_TL,  .clk_sp = 1, .pld = REG_HIGH, .clk_mar = 1     }
,       {.enable = R_RAM, .load = R_TR,  .clk_sp = 1, .pld = REG_LOW,  .clk_mar = 1     }
,       {.enable = R_RAM, .load = R_TR,               .pld = REG_HIGH,                  }
,       {.enable = R_ALU, .load = R_RAM,              .pen = REG_HIGH,                  }
,       {.enable = R_ALU, .load = R_RAM, .clk_sp =-1, .pen = REG_LOW,  .end_instr = 1 }
};

const struct CPU16::MicroInstruction CPU16::inst_sti[] = {
        {.enable    = R_TR,     .load      = R_MAR,                                 }
,       {.enable    = R_ANY,   .load      = R_RAM,                  .end_instr = 1 }
};

const struct CPU16::MicroInstruction CPU16::inst_psh[] = {
        {                                           .clk_sp = -1                 }
,       {.enable    = R_SP,    .load      = R_MAR,                                 }
,       {.enable    = R_ANY,   .load      = R_RAM,                  .end_instr = 1 }
};

const struct CPU16::MicroInstruction CPU16::inst_pop[] = {
        {.enable    = R_SP,    .load      = R_MAR,                                 }
,       {.enable    = R_RAM,   .load      = R_ANY,  .clk_sp = 1,  .end_instr = 1 }
};

const struct CPU16::MicroInstruction CPU16::inst_out[] = {
        {.enable    = R_ANY,   .load      = R_OUT,                  .end_instr = 1 }
};

const struct CPU16::MicroInstruction CPU16::inst_hlt[] = {
        {.halt      = 1,                                            .end_instr = 1 }
};

const CPU16::RegisterInfo CPU16::register_info[] = {
    {1, "-"}
,   {0, "PC"}
,   {1, "IR"}
,   {1, "MAR"}
,   {1, "RAM"}
,   {1, "TL"}
,   {1, "TR"}
,   {1, "ALU"}
,   {1, "OUT"}
,   {0, "SP"}
,   {0, "FP"}
};

const CPU16::InstructionInfo CPU16::instruction_info[] = {
    { "SETFP",  1, 0 }
,   { "SETSP",  1, 0 }
,   { "PUSHI",  1, 0 }
,   { "PUSHG",  1, 0 }
,   { "PUSHL",  1, 0 }
,   { "POPG",   1, 0 }
,   { "POPL",   1, 0 }

,   { "CALLI",  1, 0 }
,   { "CALL",   1, 0 }
,   { "RET",    0, 0 }
,   { "MATH",   0, 0 }
,   { "PSH", 0, 0 }
,   { "POP", 0, 0 }
,   { "R0D", 0, 0 }
,   { "R0E", 0, 0 }
,   { "OUT", 0, 0 }
,   { "HLT", 0, 0 }
};

void CPU16::initRegisters (void)
{
    registers[R_PC]  = &PC;
    registers[R_SP]  = &SP;
    registers[R_FP]  = &FP;
    registers[R_IR]  = &IR;
    registers[R_MAR] = &MAR;
    registers[R_RAM] = &RAM;
    registers[R_TL]  = &TL;
    registers[R_TR]  = &TR;
    registers[R_ALU] = &ALU;
    registers[R_OUT] = &OUT;
};

void CPU16::initMicroInstructions (void)
{
    instructions[I_SETFP]  = initMicroInstruction (inst_setfp);
    instructions[I_SETSP]  = initMicroInstruction (inst_setsp);
    instructions[I_PUSHI]  = initMicroInstruction (inst_pushi);
    instructions[I_PUSHG]  = initMicroInstruction (inst_pushg);
    instructions[I_PUSHL]  = initMicroInstruction (inst_pushl);
    instructions[I_POPG]   = initMicroInstruction (inst_popg);
    instructions[I_POPL]   = initMicroInstruction (inst_popl);
    instructions[I_CALLI]  = initMicroInstruction (inst_calli);
    instructions[I_CALL]   = initMicroInstruction (inst_call);
    instructions[I_RET]    = initMicroInstruction (inst_ret);
    instructions[I_MATH]   = initMicroInstruction (inst_math);

    instructions[I_PSH] = initMicroInstruction (inst_psh);
    instructions[I_POP] = initMicroInstruction (inst_pop);
    instructions[I_R0D] = initMicroInstruction (inst_hlt);
    instructions[I_R0E] = initMicroInstruction (inst_hlt);
    instructions[I_OUT] = initMicroInstruction (inst_out);
    instructions[I_HLT] = initMicroInstruction (inst_hlt);
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

void CPU16::map_cond (unsigned char data, char &zero, char &carry, const char *&name) const
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
    char i = data & 0x07;

    zero  = cond_maps[i].zero;
    carry = cond_maps[i].carry;
    name  = cond_maps[i].name;
};

void CPU16::debug (u_int16_t addr) const
{
    unsigned char data;
    int i;
    const InstructionInfo *info;
    const char *sep = " ";

    data = RAM.get (addr);
    info = instruction_info + (data >> 4);

    dump_registers (" » ", 0);

    cout << boost::format(" [%04X] %02X  %-3s") % addr % int(data) % info->name;
    if (info->cond) {
        unsigned char cond = data & 0x07;
        char zero, carry;
        const char *name;

        map_cond (cond, zero, carry, name);
        if (name) {
            cout << sep << name;
            sep = ",";
        }

    }
    if (info->argument) {
        u_int16_t arg;
        arg  = RAM.get (++addr);
        arg |= RAM.get (++addr) << 8;
        cout << sep << boost::format("0x%04X") % int (arg);
    }
    cout << endl;
    addr = SP.get();
    cout << "   " << boost::format("[%04x]") % int (addr);
    for (i = 16; i; i--, addr++) cout << boost::format(" %02X") % int (RAM.get (addr));
    cout << endl;
};

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
        cout <<  boost::format(" enable:%-3s") % register_info[reg].name;
    }
    if (ip->load) {
        reg = map_reg (ip->load,   ir);
        cout <<  boost::format(" load:%-3s") % register_info[reg].name;
    }
    if (ip->pen == REG_LOW || ip->pen == REG_HIGH) {
        cout <<  " pen:" << (ip->pen == REG_LOW ? "L" : "H");
    }
    if (ip->pld == REG_LOW || ip->pld == REG_HIGH) {
        cout <<  " pld:" << (ip->pld == REG_LOW ? "L" : "H");
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
        cout <<  " operation:" << Alu16::OpNames[ip->oper];
    }
    if (ip->cond) {
        const char *name;
        char zero, carry;
        map_cond (ir & 0x07, zero, carry, name);
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

    for (i = _inst; !i->end_instr; i++);

    ret = new MicroInstruction[prefix_sz + (i - _inst) + 1];

    for (i = prefix, j = ret; i < prefix + prefix_sz; *j++ = *i++);
    for (i = _inst; !i->end_instr; *j++ = *i++);
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

    F.get (Z, C);

    if (debug_level && !micro_ptr) debug (PC.get ());
    if (debug_level >= 2) debug_microcode (ip, ir);

    if (ip->cond) {
        const char *name;
        map_cond (ir & 0x07, zero, carry, name);
    } else {
        zero  = FLAG_NONE;
        carry = FLAG_NONE;
    }

    if (carry && C != carry -1) return 0;
    if (zero  && Z != zero  -1) return 0;

    if (ip->oper)    ALU.SetOperation (ip->oper, ir);
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

    C = (value != result);
    Z = (value == 0);
    F.set (Z, C);

    Register16::enable (partial);
};

