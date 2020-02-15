#include "cpu1.h"

const char *CPU1::Alu::OpNames[] = { "-", "ADD", "SUB" };

const struct CPU1::MicroInstruction CPU1::prefix[] = {
        {.enable    = R_PC,    .load      = R_MAR                           }
    ,   {.enable    = R_RAM,   .load      = R_IR,      .clock_pc  = 1       }
};

const struct CPU1::MicroInstruction CPU1::inst_nop[] = {
        {.end_instr = 1      }
};

const struct CPU1::MicroInstruction CPU1::inst_lda[] = {
        {.enable    = R_IR,    .load      = R_MAR                           }
    ,   {.enable    = R_RAM,   .load      = R_A,       .end_instr = 1       }
};

const struct CPU1::MicroInstruction CPU1::inst_add[] = {
        {.enable    = R_IR,    .load      = R_MAR                           }
    ,   {.enable    = R_RAM,   .load      = R_B,     .oper = Alu::OpAdd     }
    ,   {.enable    = R_ADDER, .load      = R_A,     .end_instr = 1         }
};

const struct CPU1::MicroInstruction CPU1::inst_sub[] = {
        {.enable    = R_IR,    .load      = R_MAR                           }
    ,   {.enable    = R_RAM,   .load      = R_B,     .oper = Alu::OpSub     }
    ,   {.enable    = R_ADDER, .load      = R_A,     .end_instr = 1         }
};

const struct CPU1::MicroInstruction CPU1::inst_sta[] = {
        {.enable    = R_IR,    .load      = R_MAR                           }
    ,   {.enable    = R_A,     .load      = R_RAM,   .end_instr = 1         }
};

const struct CPU1::MicroInstruction CPU1::inst_ldi[] = {
        {.enable    = R_IR,    .load      = R_A,     .end_instr = 1         }
};

const struct CPU1::MicroInstruction CPU1::inst_jmp[] = {
        {.enable    = R_IR,    .load      = R_PC,    .end_instr = 1         }
};

const struct CPU1::MicroInstruction CPU1::inst_jc[] = {
        {.enable    = R_IR,    .load      = R_PC,    .carry = FLAG_1        }
    ,   {                                            .end_instr = 1         }
};

const struct CPU1::MicroInstruction CPU1::inst_jz[] = {
        {.enable    = R_IR,    .load      = R_PC,    .zero = FLAG_1         }
    ,   {                                            .end_instr = 1         }
};

const struct CPU1::MicroInstruction CPU1::inst_out[] = {
        {.enable    = R_A,     .load      = R_OUT,   .end_instr = 1         }
};

const struct CPU1::MicroInstruction CPU1::inst_hlt[] = {
        {.halt      = 1,                             .end_instr = 1         }
};

const CPU1::RegisterInfo CPU1::register_info[] = {
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

const CPU1::InstructionInfo CPU1::instruction_info[] = {
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
};

void CPU1::initMicroInstructions (void)
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

void CPU1::dump_registers (const char *prefix, int internal) const
{
    int i;

    cout << prefix;
    for (i = 1; i < R_NUM; i++) {
        if (internal != register_info[i].internal) continue;
        cout << boost::format("%s:%02X ") % register_info[i].name % int(registers[i]->get());
    }
};

void CPU1::debug (int addr) const
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

void CPU1::debug_microcode (const struct MicroInstruction *ip) const
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

struct CPU1::MicroInstruction *CPU1::initMicroInstruction (const struct CPU1::MicroInstruction *_inst)
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
int CPU1::execute_microcode(const struct MicroInstruction *ip, int debug_level)
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

int CPU1::clock (int debug_level)
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
