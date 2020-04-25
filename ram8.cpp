#include "ram8.h"
#include "register16.h"

u_int8_t Ram8::get (int addr, int silent)
{
    if (!silent) {
        // cout <<  boost::format("RAM GET %04X %02X") % addr % int (data[truncate(addr)]) << endl;
        nget++;
    }

    return data[truncate(addr)];
};

void Ram8::set (int addr, u_int8_t _data, int silent)
{
    if (!silent) {
        // cout <<  boost::format("RAM SET %04X %02X") % addr % int (data[truncate(addr)]) << endl;
        nset++;
    }

    data[truncate(addr)] = _data;
};

void Cache8::flush (int idx, int silent)
{
    if (cstat[idx] != C_DIRTY) return;
    Ram8::set (caddr[idx], cdata[idx], silent);
    cstat[idx] = C_DATA;
}

u_int8_t Cache8::get (int addr, int silent)
{
    int idx = addr % csize;

    if (silent) {
        u_int8_t ret;

        if (cstat[idx] != C_CLEAN && caddr[idx] == addr) {
            ret = cdata[idx];
        } else {
            ret = Ram8::get (addr, 1);
        }
        return ret;
    }

    if (cstat[idx] == C_CLEAN || caddr[idx] != addr) {
        flush (idx, silent);
        cdata[idx] = Ram8::get (addr, silent);
        caddr[idx] = addr;
        cstat[idx] = C_DATA;
    }

    // cout <<  boost::format("CHE GET %04X %02X") % addr % int (cdata[idx]) << endl;
    nget++;

    return cdata[idx];
};

void Cache8::set (int addr, u_int8_t data, int silent)
{
    int idx = addr % csize;

    if (!silent) {
        // cout <<  boost::format("CHE SET %04X %02X") % addr % int (data) << endl;
        nset++;
    }

    if (cstat[idx] == C_CLEAN || caddr[idx] != addr) {
        flush (idx, silent);
        caddr[idx] = addr;
    }
    cdata[idx] = data;
    cstat[idx] = C_DIRTY;
};

void Cache8assoc::flush (int idx, int silent)
{
    if (cstat[idx] != C_DIRTY) return;
    Ram8::set (caddr[idx], cdata[idx], silent);
    cstat[idx] = C_DATA;
}

int Cache8assoc::find_idx (int addr)
{
    int hash = addr;
    int last_idx = hash % (csize >> assocbits);
    int idx = last_idx << assocbits;
    int i;

    for (i = idx; i < idx + assoc; i++) {
        if (cstat[i] != C_CLEAN && caddr[i] == addr) return i;
    }
    clast[last_idx] = (clast[last_idx] + 1) & assocmask;
    return idx + clast[last_idx];
};

u_int8_t Cache8assoc::get (int addr, int silent)
{
    int idx = find_idx (addr);

    if (silent) {
        u_int8_t ret;

        if (cstat[idx] != C_CLEAN && caddr[idx] == addr) {
            ret = cdata[idx];
        } else {
            ret = Ram8::get (addr, 1);
        }
        return ret;
    }

    if (cstat[idx] == C_CLEAN || caddr[idx] != addr) {
        flush (idx, silent);
        cdata[idx] = Ram8::get (addr, silent);
        caddr[idx] = addr;
        cstat[idx] = C_DATA;
    }

    // cout <<  boost::format("CHE GET %04X %02X") % addr % int (cdata[idx]) << endl;
    nget++;

    return cdata[idx];
};

void Cache8assoc::set (int addr, u_int8_t data, int silent) 
{
    int idx = find_idx (addr);

    if (!silent) {
        // cout <<  boost::format("CHE SET %04X %02X") % addr % int (data) << endl;
        nset++;
    }

    if (cstat[idx] == C_CLEAN || caddr[idx] != addr) {
        flush (idx, silent);
        caddr[idx] = addr;
    }
    cdata[idx] = data;
    cstat[idx] = C_DIRTY;
};
