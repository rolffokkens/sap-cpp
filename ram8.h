#ifndef H_RAM8
#define H_RAM8

#include "register16.h"

/*
 * Ram
 */
class Ram8 {
    u_int8_t *data;
    int      size;
private:
    u_int16_t truncate (u_int16_t _addr) const {
        return _addr % size ;
    }
public:
    Ram8 (int _size, u_int8_t *_data) {
        data = _data;
        size = _size;
    }
    u_int8_t get (int addr) const {
        return data[truncate(addr)];
    }
    void set (int addr, u_int8_t _data) {
        data[truncate(addr)] = _data;
    }
};

#endif
