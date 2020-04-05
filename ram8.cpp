#include "ram8.h"

void RamRegister8::enable (int partial) {
    value = data[addr];

    Register16::enable (partial);
};

void RamRegister8::load (int partial) {
    Register16::load (partial);

    data[addr] = value;
};
