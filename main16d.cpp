/*
 * This demonstrates CPU16 (see cpu2.h)
 *
 * It counts down from 5 to 0 and
 * outputs each counter value
 */

#include <iostream>
#include <boost/format.hpp>
#include <cstddef>
#include <fstream>

#include "cpu16d.h"

using namespace std; 

int main (int argc, char *argv[])
{
    int i, debug = 0, param, ticks = 0;

    static u_int8_t ram2[65536] = { 0x20, 0x01, 0x00 // [0000] LDTRC 0x0001
                                  , 0x60             // [0003] PUSH
                                  , 0x20, 0x02, 0x00 // [0004] LDTRC 0x0002
                                  , 0xBC             // [0007] CMP   LT
                                  , 0xF0             // [0008] HALT

                                  , 0x20, 0x4A, 0x00 // [0000] LDTRC 0x004A # main
                                  , 0xE0             // [0003] JUMP

                                                     //                     #         # int fib (int p) {
                                  , 0x10, 0xFC, 0xFF // [0004] SETSP -4     #         #     int i, j;

                                  , 0x30, 0x04, 0x00 // [0007] LDTRL 4      # push p  #     if (p < 2) {
                                  , 0x40             // [000A] FETCH
                                  , 0x60             // [000B] PUSH

                                  , 0x20, 0x02, 0x00 // [000C] LDTRC 2      # push #2 #
                                  , 0xB0             // [000F] CMP
                                  , 0x20, 0x19, 0x00 // [0010] LDTRC 0x0019
                                  , 0xE7             // [0013] JUMP  NC
                                  , 0x30, 0x04, 0x00 // [0014] LDTRL 4      # push p  #         return p;
                                  , 0x40             // [0017] FETCH
                                  , 0x90             // [0018] RET          #         #     }
                                  , 0x30, 0x04, 0x00 // [0019] LDTRL 4      # push p  #     i = fib (p - 1);
                                  , 0x40             // [001C] FETCH
                                  , 0x60             // [001D] PUSH
                                  , 0x20, 0x01, 0x00 // [001E] LDTRC 1      # push #1 #
                                  , 0xA3             // [0021] SUB          # p1 - #1 #
                                  , 0x60             // [0022] PUSH
                                  , 0x20, 0x04, 0x00 // [0023] LDTRC 0x0004 # fib()   #
                                  , 0x80             // [0026] CALL
                                  , 0x60             // [0027] PUSH
                                  , 0x30, 0xFE, 0xFF // [0028] LDTRL -2     # i =     #
                                  , 0x50             // [002B] STORE
                                  , 0x30, 0x04, 0x00 // [002C] LDTRL 4      # push p  #     j = fib (p - 2);
                                  , 0x40             // [002F] FETCH
                                  , 0x60             // [0030] PUSH
                                  , 0x20, 0x02, 0x00 // [0031] LDTRC 2      # push #2 #
                                  , 0xA3             // [0034] SUB          # p1 - #1 #
                                  , 0x60             // [0035] PUSH
                                  , 0x20, 0x04, 0x00 // [0036] LDTRC 0x0004 # fib()   #
                                  , 0x80             // [0039] CALL
                                  , 0x60             // [003A] PUSH
                                  , 0x30, 0xFC, 0xFF // [003B] LDTRL -4     # j =     #
                                  , 0x50             // [003E] STORE

                                  , 0x30, 0xFE, 0xFF // [003F] LDTRL -2     # push i  #     return i + j
                                  , 0x40             // [0042] FETCH
                                  , 0x60             // [0043] PUSH
                                  , 0x30, 0xFC, 0xFF // [0044] LDTRL -4     # push j  #
                                  , 0x40             // [0047] FETCH
                                  , 0xA2             // [0048] ADD          #         #
                                  , 0x90             // [0049] RET          #         #

                                  , 0x00, 0x00, 0x80 // [004A] SETFP 0x8000 #
                                                     //                     #         # main () {
                                  , 0x20, 0x06, 0x00 // [0046] LDTRC 6      #         #    fib (6)
                                  , 0x60             // [0049] PUSH
                                  , 0x20, 0x04, 0x00 // [004A] LDTRC 0x0004 # fib()   #
                                  , 0x80             // [004D] CALL
                                  , 0xF0             // [004E] HALT         #         # }
                                  };

    debug = (argc >= 2 ? atoi (argv[1]) : 0);

    if (argc >= 3) {
        ifstream myFile (argv[2], ios::in | ios::binary);
        if (!myFile.read ((char *)ram2, sizeof (ram2))) {
        }
    }
    param = (argc >= 4 ? atoi (argv[3]) : 0);

    ram2[0xfffe] =  param       & 0xff;
    ram2[0xffff] = (param >> 8) & 0xff;

    Cache8assoc RAM (sizeof (ram2), ram2, 256, 2);

    CPU16 cpu (RAM);

    while (!cpu.clock (debug)) { ticks++;};

    if (debug) {
        for (i = 0; i < 2; i++) {
            int nget, nset;
            RAM.get_stats (i, nget, nset);
            cout <<  boost::format("%s: %8d %8d") % i % nget % nset << endl;
        }
        cout <<  boost::format("TICKS: %d") % ticks << endl;
    }
}
