/*
 * This demonstrates CPU16 (see cpu2.h)
 *
 * It counts down from 5 to 0 and
 * outputs each counter value
 */

#include <iostream>
#include <boost/format.hpp>
#include <cstddef>

#include "cpu16a.h"

using namespace std; 

int main (void)
{
    int i, ticks = 0;

    static u_int8_t ram2[65536] = { 0x20, 0x42, 0x00 // [0000] PUSHI 0x0042 # main
                                  , 0xE0             // [0003] JUMP
                                                     //                     #         # int fib (int p) {
                                  , 0x10, 0xFC, 0xFF // [0004] SETSP -4     #         #     int i, j;

                                  , 0x30, 0x04, 0x00 // [0007] PUSHL 4      # push p  #     if (p < 2) {
                                  , 0x40             // [0008] FETCH
                                  , 0x20, 0x02, 0x00 // [000B] PUSHC 2      # push #2 #
                                  , 0xB0             // [000E] CMP
                                  , 0x20, 0x18, 0x00 // [000F] PUSHC 0x0018
                                  , 0xE7             // [0012] JUMP  NC
                                  , 0x30, 0x04, 0x00 // [0013] PUSHL 4      # push p  #         return p;
                                  , 0x40             // [0016] FETCH
                                  , 0x90             // [0017] RET          #         #     }
                                  , 0x30, 0x04, 0x00 // [0018] PUSHL 4      # push p  #     i = fib (p - 1);
                                  , 0x40             // [001B] FETCH
                                  , 0x20, 0x01, 0x00 // [001C] PUSHC 1      # push #1 #
                                  , 0xA3             // [001F] SUB          # p1 - #1 #
                                  , 0x20, 0x04, 0x00 // [0020] PUSHC 0x0004 # fib()   #
                                  , 0x80             // [0023] CALL

                                  , 0x30, 0xFE, 0xFF // [0024] PUSHL -2     # i =     #
                                  , 0x50             // [0027] STORE

                                  , 0x30, 0x04, 0x00 // [0028] PUSHL 4      # push p  #     j = fib (p - 2);
                                  , 0x40             // [002B] FETCH
                                  , 0x20, 0x02, 0x00 // [002C] PUSHC 2      # push #2 #
                                  , 0xA3             // [002F] SUB          # p1 - #1 #
                                  , 0x20, 0x04, 0x00 // [0030] PUSHC 0x0004 # fib()   #
                                  , 0x80             // [0033] CALL
                                  , 0x30, 0xFC, 0xFF // [0034] PUSHL -4     # j =     #
                                  , 0x50             // [0037] STORE
                                  , 0x30, 0xFE, 0xFF // [0038] PUSHL -2     # push i  #     return i + j
                                  , 0x40             // [003B] FETCH
                                  , 0x30, 0xFC, 0xFF // [003C] PUSHL -4     # push j  #
                                  , 0x40             // [003F] FETCH
                                  , 0xA2             // [0040] ADD          #         #
                                  , 0x90             // [0041] RET          #         #

                                  , 0x00, 0x00, 0x80 // [0042] SETFP 0x8000 #
                                                     //                     #         # main () {
                                  , 0x20, 0x06, 0x00 // [0045] PUSHC 6      #         #    fib (6)
                                  , 0x20, 0x04, 0x00 // [0048] PUSHC 0x0004 # fib()   #
                                  , 0x80             // [004B] CALL
                                  , 0xF0             // [004C] HALT         #         # }
                                  };
    Cache8assoc RAM (sizeof (ram2), ram2, 64, 2);

    CPU16 cpu (RAM);

    while (!cpu.clock (1)) { ticks++; };

    for (i = 0; i < 2; i++) {
         int nget, nset;
         RAM.get_stats (i, nget, nset);
         cout <<  boost::format("%s: %8d %8d") % i % nget %nset << endl;
    }
    cout <<  boost::format("TICKS: %d") % ticks << endl;
}
