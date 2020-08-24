#ifndef H_REGISTER16
#define H_REGISTER16

#include <sys/types.h>
#include <iostream>
#include <boost/format.hpp>
#include "internalbus16.h"

using namespace std;

/****************************************************************************
 * Micro code is always executed conditional, conditions specified as
 * requirenents for Z en C flags. FLAG_NONE means: no requirement.
 ****************************************************************************/
enum Partial {
    REG_FULL = 0
,   REG_LOW  = 1
,   REG_HIGH = 2
};

/****************************************************************************
 * General Register16 - is connected to the InternalBus16
 ****************************************************************************/
class Register16 {
private:
    const char *name;
    InternalBus16 &bus;
    u_int16_t enable_mask, load_mask;
protected:
    u_int16_t value;
public:
    Register16 (const char *_name, InternalBus16 &_bus, u_int16_t _enable_mask = 0xffff, u_int16_t _load_mask = 0xffff) 
        : bus (_bus)
        , name (_name) {
        enable_mask = _enable_mask;
        load_mask   = _load_mask;
    };
    void reset (u_int16_t _value = 0) {
        value = _value;
    };
    virtual u_int16_t get (void) const;
    virtual void enable (int partial);
    virtual void load (int partial);
};

/****************************************************************************
 * Counter Register - currently only PC, could be SP in future too
 ****************************************************************************/
class Counter16: public Register16 {
public:
    Counter16 (const char *_name, InternalBus16 &bus): Register16 (_name, bus) { };
    void clock (void) {
        value++;
    };
    virtual void clock (int diff) {
        value += diff;
    };
};

/****************************************************************************
 * Output Register - no LED's but some output nontheless
 ****************************************************************************/
class OutputRegister16: public Register16 {
public:
    OutputRegister16 (const char *_name, InternalBus16 &bus): Register16 (_name, bus) { };

    void load (int partial) {
        Register16::load (partial);

        cout << "OUTPUT:" << int(value) << endl;
    }
};

#endif
