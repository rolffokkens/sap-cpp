#include "cpu2.h"

const char *CPU2::Alu::OpNames[] = { "-", "ADD", "SUB" };

const struct CPU2::MicroInstruction CPU2::prefix[] = {
        {.enable    = R_PC,    .load      = R_MAR                                  }
,       {.enable    = R_RAM,   .load      = R_IR,   .clock_pc = 1                  }
};

const struct CPU2::MicroInstruction CPU2::inst_ld[] = {
        {.enable    = R_PC,    .load      = R_MAR                                  }
,       {.enable    = R_RAM,   .load      = R_MAR,  .clock_pc = 1                  }
,       {.enable    = R_RAM,   .load      = R_ANY,                  .end_instr = 1 }
};

const struct CPU2::MicroInstruction CPU2::inst_add[] = {
        {.enable    = R_ANY,   .load      = R_TMP,  .oper = Alu::OpAdd             }
,       {.enable    = R_ALU,   .load      = R_A,                    .end_instr = 1 }
};

const struct CPU2::MicroInstruction CPU2::inst_sub[] = {
        {.enable    = R_ANY,   .load      = R_TMP,  .oper = Alu::OpSub             }
,       {.enable    = R_ALU,   .load      = R_A,                    .end_instr = 1 }
};

const struct CPU2::MicroInstruction CPU2::inst_st[] = {
        {.enable    = R_PC,    .load      = R_MAR                                  }
,       {.enable    = R_RAM,   .load      = R_MAR,  .clock_pc = 1                  }
,       {.enable    = R_ANY,   .load      = R_RAM,                  .end_instr = 1 }
};

const struct CPU2::MicroInstruction CPU2::inst_set[] = {
        {.enable    = R_PC,    .load      = R_MAR                                  }
,       {.enable    = R_RAM,   .load      = R_ANY,  .clock_pc = 1, .end_instr = 1 }
};

const struct CPU2::MicroInstruction CPU2::inst_jmp[] = {
        {.enable    = R_PC,    .load      = R_MAR,                                 }
,       {                                           .clock_pc = 1                  }
,       {.enable    = R_RAM,   .load      = R_PC,   .cond = 1                      }
,       {                                                           .end_instr = 1 }
};

const struct CPU2::MicroInstruction CPU2::inst_lda[] = {
        {.enable    = R_ANY,   .load      = R_A,                    .end_instr = 1 }
};

const struct CPU2::MicroInstruction CPU2::inst_sta[] = {
        {.enable    = R_A,     .load      = R_ANY,                  .end_instr = 1 }
};

const struct CPU2::MicroInstruction CPU2::inst_ldi[] = {
        {.enable    = R_A,     .load      = R_MAR,                                 }
,       {.enable    = R_RAM,   .load      = R_ANY,                  .end_instr = 1 }
};

const struct CPU2::MicroInstruction CPU2::inst_sti[] = {
        {.enable    = R_A,     .load      = R_MAR,                                 }
,       {.enable    = R_ANY,   .load      = R_RAM,                  .end_instr = 1 }
};

const struct CPU2::MicroInstruction CPU2::inst_psh[] = {
        {                                           .clock_sp = -1                 }
,       {.enable    = R_SP,    .load      = R_MAR,                                 }
,       {.enable    = R_ANY,   .load      = R_RAM,                  .end_instr = 1 }
};

const struct CPU2::MicroInstruction CPU2::inst_pop[] = {
        {.enable    = R_SP,    .load      = R_MAR,                                 }
,       {.enable    = R_RAM,   .load      = R_ANY,  .clock_sp = 1,  .end_instr = 1 }
};

const struct CPU2::MicroInstruction CPU2::inst_out[] = {
        {.enable    = R_ANY,   .load      = R_OUT,                  .end_instr = 1 }
};

const struct CPU2::MicroInstruction CPU2::inst_hlt[] = {
        {.halt      = 1,                                            .end_instr = 1 }
};

const CPU2::RegisterInfo CPU2::register_info[] = {
    {1, "-"}
,   {0, "PC"}
,   {1, "IR"}
,   {1, "MAR"}
,   {1, "RAM"}
,   {1, "TMP"}
,   {1, "ALU"}
,   {1, "OUT"}
,   {0, "SP"}
,   {0, "A"}
,   {0, "B"}
,   {0, "C"}
,   {0, "D"}
};

const CPU2::InstructionInfo CPU2::instruction_info[] = {
    { "LD",  1, 1, 0 }
,   { "ADD", 1, 0, 0 }
,   { "SUB", 0, 1, 0 }
,   { "ST",  1, 1, 0 }
,   { "SET", 1, 1, 0 }
,   { "JMP", 1, 0, 1 }
,   { "LDA", 0, 1, 0 }
,   { "STA", 0, 1, 0 }
,   { "LDI", 0, 1, 0 }
,   { "STI", 0, 1, 0 }
,   { "PSH", 0, 1, 0 }
,   { "POP", 0, 1, 0 }
,   { "R0D", 0, 0, 0 }
,   { "R0E", 0, 0, 0 }
,   { "OUT", 0, 1, 0 }
,   { "HLT", 0, 0, 0 }
};

void CPU2::initRegisters (void)
{
    registers[R_PC]  = &PC;
    registers[R_SP]  = &SP;
    registers[R_IR]  = &IR;
    registers[R_MAR] = &MAR;
    registers[R_RAM] = &RAM;
    registers[R_TMP] = &TMP;
    registers[R_ALU] = &ALU;
    registers[R_OUT] = &OUT;
    registers[R_A]   = &A;
    registers[R_B]   = &B;
    registers[R_C]   = &C;
    registers[R_D]   = &D;
};

void CPU2::initMicroInstructions (void)
{
    instructions[I_LD]  = initMicroInstruction (inst_lda);
    instructions[I_ADD] = initMicroInstruction (inst_add);
    instructions[I_SUB] = initMicroInstruction (inst_sub);
    instructions[I_ST]  = initMicroInstruction (inst_sta);
    instructions[I_SET] = initMicroInstruction (inst_set);
    instructions[I_JMP] = initMicroInstruction (inst_jmp);
    instructions[I_LDA] = initMicroInstruction (inst_lda);
    instructions[I_STA] = initMicroInstruction (inst_sta);
    instructions[I_LDI] = initMicroInstruction (inst_ldi);
    instructions[I_STI] = initMicroInstruction (inst_sti);
    instructions[I_PSH] = initMicroInstruction (inst_psh);
    instructions[I_POP] = initMicroInstruction (inst_pop);
    instructions[I_R0D] = initMicroInstruction (inst_hlt);
    instructions[I_R0E] = initMicroInstruction (inst_hlt);
    instructions[I_OUT] = initMicroInstruction (inst_out);
    instructions[I_HLT] = initMicroInstruction (inst_hlt);
};

void CPU2::dump_registers (const char *prefix, int internal) const
{
    int i;
    char Z, C;

    F.get (Z, C);

    cout << prefix;
    for (i = 1; i < R_NUM; i++) {
        if (!internal && register_info[i].internal) continue;
        cout << boost::format("%s:%02X ") % register_info[i].name % int(registers[i]->get());
    }
    cout <<  "c:" << int(C) <<  " z:" << int(Z) << " ";
};

void CPU2::map_cond (unsigned char data, char &zero, char &carry, const char *&name) const
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

void CPU2::debug (int addr) const
{
    unsigned char data;
    int i;
    const InstructionInfo *info;
    const char *sep = " ";

    data = RAM.get (addr);
    info = instruction_info + (data >> 4);

    dump_registers (" » ", 0);

    cout << boost::format(" [%02X] %02X  %-3s") % addr % int(data) % info->name;
    if (info->regid) {
        cout << sep << register_info[map_reg(R_ANY, data)].name;
        sep = ",";
    }
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
    if (info->argument) cout << sep << boost::format("0x%02X") % int (RAM.get (addr + 1));
    cout << endl;
};

void CPU2::debug_microcode (const struct MicroInstruction *ip, unsigned char ir) const
{
    char Z, C;
    unsigned char reg;

    F.get (Z, C);

    dump_registers ("   » ", 1);
    cout <<  "BUS:" << boost::format("%02X") % int (bus.read ())
         <<  boost::format("  [%02X]") % int(micro_ptr);
    if (ip->enable) {
        reg = map_reg (ip->enable, ir);
        cout <<  boost::format(" enable:%-3s") % register_info[reg].name;
    }
    if (ip->load) {
        reg = map_reg (ip->load,   ir);
        cout <<  boost::format(" load:%-3s") % register_info[reg].name;
    }
    if (ip->clock_pc) {
        cout <<  " clockPC" << (ip->clock_pc == 1 ? "+" : "-");
    }
    if (ip->clock_sp) {
        cout <<  " clockSP" << (ip->clock_sp == 1 ? "+" : "-");
    }
    if (ip->oper) {
        cout <<  " operation:" << Alu::OpNames[ip->oper];
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

struct CPU2::MicroInstruction *CPU2::initMicroInstruction (const struct CPU2::MicroInstruction *_inst)
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
int CPU2::execute_microcode(const struct MicroInstruction *ip, unsigned char ir, int debug_level)
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

    if (ip->oper)      ALU.SetOperation (ip->oper);
    if (ip->enable)    registers[map_reg (ip->enable, ir)]->enable ();
    if (ip->load)      registers[map_reg (ip->load,   ir)]->load ();
    if (ip->clock_pc)  PC.clock (ip->clock_pc);
    if (ip->clock_sp)  SP.clock (ip->clock_sp);
    if (ip->halt)      halted = 1;

    return ip->end_instr;
};

int CPU2::clock (int debug_level)
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
