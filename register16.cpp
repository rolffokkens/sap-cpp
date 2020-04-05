#include <sys/types.h>
#include <iostream>
#include <boost/format.hpp>
#include "register16.h"

using namespace std;

u_int16_t Register16::get (void) const {
    return value;
};

void Register16::enable (int partial) {
    u_int16_t tmp = value;
    if (partial == REG_HIGH) {
        tmp >>= 8;
    }
    bus.write (tmp & enable_mask);
};

void Register16::load (int partial) {
    u_int16_t tmp = bus.read ();
    switch (partial) {
    case REG_LOW:
        tmp = (tmp   & 0xff) | ((value << 8) & 0xff00);
        break;
    case REG_HIGH:
        tmp = (value & 0xff) | ((tmp   << 8) & 0xff00);
        break;
    default:
        break;
    }
    value = tmp & load_mask;
};
