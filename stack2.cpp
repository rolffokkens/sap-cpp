/*
 * This demonstrates CPU16 (see cpu2.h)
 *
 * It counts down from 5 to 0 and
 * outputs each counter value
 */

#include <iostream>
#include <boost/format.hpp>
#include <cstddef>

#include "cpu16.h"

using namespace std; 

int main (void)
{
    static u_int8_t ram2[65536] = { 0x20, 0x3A, 0x00 // [0000] PUSHI 0x003A # main
                                , 0xE0             // [0003] JUMP
                                                   //                     #         # int fib (int p) {
                                , 0x10, 0xFC, 0xFF // [0004] SETSP -4     #         #     int i, j;

                                , 0x40, 0x04, 0x00 // [0007] PUSHL 4      # push p  #     if (p < 2) {
                                , 0x20, 0x02, 0x00 // [000A] PUSHI 2      # push #2 #
                                , 0xB0             // [000D] CMP
                                , 0x20, 0x16, 0x00 // [000E] PUSHI 0x0016
                                , 0xE7             // [0011] JUMP  NC
                                , 0x40, 0x04, 0x00 // [0012] PUSHL 4      # push p  #         return p;
                                , 0x90             // [0015] RET          #         #     }
                                , 0x40, 0x04, 0x00 // [0016] PUSHL 4      # push p  #     i = fib (p - 1);
                                , 0x20, 0x01, 0x00 // [0019] PUSHI 1      # push #1 #
                                , 0xA3             // [001C] SUB          # p1 - #1 #
                                , 0x20, 0x04, 0x00 // [001D] PUSHI 0x0004 # fib()   #
                                , 0x80             // [0020] CALL
                                , 0x60, 0xFE, 0xFF // [0021] POPL  -2     # i =     #
                                , 0x40, 0x04, 0x00 // [0024] PUSHL 4      # push p  #     j = fib (p - 2);
                                , 0x20, 0x02, 0x00 // [0027] PUSHI 2      # push #2 #
                                , 0xA3             // [002A] SUB          # p1 - #1 #
                                , 0x20, 0x04, 0x00 // [002B] PUSHI 0x0004 # fib()   #
                                , 0x80             // [002E] CALL
                                , 0x60, 0xFC, 0xFF // [002F] POPL  -4     # j =     #
                                , 0x40, 0xFE, 0xFF // [0032] PUSHL -2     # push i  #     return i + j
                                , 0x40, 0xFC, 0xFF // [0035] PUSHL -4     # push j  #
                                , 0xA2             // [0038] ADD          #         #
                                , 0x90             // [0039] RET          #         #

                                , 0x00, 0x00, 0x80 // [003A] SETFP 0x8000 #
                                                   //                     #         # main () {
                                , 0x20, 0x06, 0x00 // [003C] PUSHI 6      #         #    fib (6)
                                , 0x20, 0x04, 0x00 // [0040] PUSHI 0x0004 # fib()   #
                                , 0x80             // [0043] CALL
                                , 0xF0             // [0044] HALT         #         # }
                                };
    Ram8 RAM8 (sizeof (ram2), ram2);

    CPU16 cpu (RAM8);

    while (!cpu.clock (1)) {};
}
