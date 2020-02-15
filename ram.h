#ifndef H_RAM
#define H_RAM

#include "register.h"

/****************************************************************************
 * Ram Register - is connected to the RAM (data)
 ****************************************************************************/
class RamRegister : public Register {
    unsigned char addr;
    unsigned char *data;
    int      size;
private:
    unsigned char truncate (unsigned char _addr) const {
        return _addr % size ;
    }
public:
    RamRegister (InternalBus &bus, int _size, unsigned char *_data) : Register (bus) {
        addr = 0;
        data = _data;
        size = _size;
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

#endif
