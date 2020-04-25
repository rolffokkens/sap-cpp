#ifndef H_RAM8
#define H_RAM8

#include "register16.h"

/*
 * Ram
 */
class Ram8 {
private:
    int      nget, nset;

    u_int8_t *data;
    int      size;
    u_int16_t truncate (u_int16_t _addr) const {
        return _addr % size ;
    }
public:
    Ram8 (int _size, u_int8_t *_data) : nget (0), nset (0) {
        data = _data;
        size = _size;
    }
    virtual u_int8_t get (int addr, int silent = 0);
    virtual void set (int addr, u_int8_t _data, int silent = 0);
    virtual void get_stats (int depth, int &_nget, int &_nset) {
        _nget = nget;
        _nset = nset;
    }
};

class Cache8 : public Ram8 {
private:
    int      nget, nset;

    u_int8_t  *cdata;
    u_int16_t *caddr;
    u_int8_t  *cstat;
    int       csize;

    enum {
        C_CLEAN = 0
    ,   C_DATA
    ,   C_DIRTY
    };

    void flush (int idx, int silent);
public:
    Cache8 (int ramsize, u_int8_t *ramdata, int cachesize) : Ram8 (ramsize, ramdata), nget (0), nset (0) {
        csize = cachesize;
        cdata = new u_int8_t[csize];
        caddr = new u_int16_t[csize];
        cstat = new u_int8_t[csize] ();
    }
    virtual u_int8_t get (int addr, int silent = 0);
    virtual void set (int addr, u_int8_t data, int silent = 0);
    virtual void get_stats (int depth, int &_nget, int &_nset) {
        if (depth) {
            Ram8::get_stats (depth - 1, _nget, _nset);
        } else {
            _nget = nget;
            _nset = nset;
        }
    }
};

class Cache8assoc : public Ram8 {
private:
    int      nget, nset;

    u_int8_t  *clast;
    u_int8_t  *cdata;
    u_int16_t *caddr;
    u_int8_t  *cstat;
    int       csize, assoc, assocbits, assocmask;

    enum {
        C_CLEAN = 0
    ,   C_DATA
    ,   C_DIRTY
    };

    int find_idx (int addr);

    void flush (int idx, int silent);
public:
    Cache8assoc (int ramsize, u_int8_t *ramdata, int cachesize, int _assocbits)
        : Ram8 (ramsize, ramdata), nget (0), nset (0) {
        assocbits = _assocbits;
        assoc     = 1 << _assocbits;
        assocmask = assoc - 1;
        csize     = cachesize;
        clast     = new u_int8_t[csize >> _assocbits] ();
        cdata     = new u_int8_t[csize];
        caddr     = new u_int16_t[csize];
        cstat     = new u_int8_t[csize] ();
    }
    virtual u_int8_t get (int addr, int silent = 0);
    virtual void set (int addr, u_int8_t data, int silent = 0);
    virtual void get_stats (int depth, int &_nget, int &_nset) {
        if (depth) {
            Ram8::get_stats (depth - 1, _nget, _nset);
        } else {
            _nget = nget;
            _nset = nset;
        }
    }
};


#endif
