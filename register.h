#ifndef H_REGISTER
#define H_REGISTER

#include <iostream>
#include <boost/format.hpp>

using namespace std;

/****************************************************************************
 * General Register - is connected to the InternalBus
 ****************************************************************************/
class Register {
private:
    InternalBus &bus;
    char enable_mask, load_mask;
protected:
    unsigned char value;
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

#endif
