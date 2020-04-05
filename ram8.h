#ifndef H_RAM8
#define H_RAM8

#include "register16.h"

/****************************************************************************
 * Ram Register - is connected to the RAM (data)
 ****************************************************************************/
class RamRegister8 : public Register16 {
    u_int16_t     addr;
    unsigned char *data;
    int           size;
private:
    u_int16_t truncate (u_int16_t _addr) const {
        return _addr % size ;
    }
public:
    RamRegister8 (InternalBus16 &bus, int _size, unsigned char *_data) : Register16 (bus) {
        addr = 0;
        data = _data;
        size = _size;
    };

    void set_addr (u_int16_t _addr) {
        addr = truncate (_addr);
    }

    void enable (int partial);
    void load   (int partial);

    u_int16_t get (u_int16_t _addr) const {
        return (unsigned char)(data[truncate (_addr)]);
    };
};

/****************************************************************************
 * Memory Address Register - passes address info to RAM
 ****************************************************************************/
class MemAddrRegister16 : public Counter16 {
private:
    RamRegister8 &RAM;
public:
    MemAddrRegister16 (InternalBus16 &bus, RamRegister8 &_RAM) : Counter16 (bus), RAM(_RAM) { };

    void load (int partial) {
        Counter16::load (partial);
        RAM.set_addr (value);
    };

    void clock (int diff) {
        Counter16::clock (diff);
        RAM.set_addr (value);
    };
};

#endif
