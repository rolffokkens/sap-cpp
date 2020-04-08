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
    unsigned char ram2[65536] = { 0xD0, 0x36, 0x00 // [0000] JUMPI 0x0036

                                                   //                     #         # int fib (int p) {
                                , 0x10, 0xFC, 0xFF // [0003] SETSP -4     #         #     int i, j;

                                , 0x40, 0x04, 0x00 // [0006] PUSHL 4      # push p  #     if (p < 2) {
                                , 0x20, 0x02, 0x00 // [0009] PUSHI 2      # push #2 #
                                , 0xB0             // [000C] CMP
                                , 0xD7, 0x14, 0x00 // [000D] JUMPI NC,0x0014

                                , 0x40, 0x04, 0x00 // [0010] PUSHL 4      # push p  #         return p;
                                , 0x90             // [0013] RET          #         #
                                                   //                     #         #     }

                                , 0x40, 0x04, 0x00 // [0014] PUSHL 4      # push p  #     i = fib (p - 1);
                                , 0x20, 0x01, 0x00 // [0017] PUSHI 1      # push #1 #
                                , 0xA3             // [001A] SUB          # p1 - #1 #
                                , 0x70, 0x03, 0x00 // [001B] CALLI 0x0003 # fib()   #
                                , 0x60, 0xFE, 0xFF // [001E] POPL  -2     # i =     #

                                , 0x40, 0x04, 0x00 // [0021] PUSHL 4      # push p  #     j = fib (p - 2);
                                , 0x20, 0x02, 0x00 // [0024] PUSHI 2      # push #2 #
                                , 0xA3             // [0027] SUB          # p1 - #1 #
                                , 0x70, 0x03, 0x00 // [0028] CALLI 0x0003 # fib()   #
                                , 0x60, 0xFC, 0xFF // [002B] POPL  -4     # j =     #

                                , 0x40, 0xFE, 0xFF // [002E] PUSHL -2     # push i  #     return i + j
                                , 0x40, 0xFC, 0xFF // [0031] PUSHL -4     # push j  #
                                , 0xA2             // [0034] ADD          #         #
                                , 0x90             // [0035] RET          #         #

                                , 0x00, 0x00, 0x80 // [0036] SETFP 0x8000 # Initialize SP, FP

                                                   //                     #         # main () {
                                , 0x20, 0x06, 0x00 // [0038] PUSHI 6      #         #    fib (6)
                                , 0x70, 0x03, 0x00 // [003B] CALLI 0x0003 # fib()   # 
                                , 0xF0             // [003E] HALT         #         # }
                                };

    CPU16 cpu (ram2);

    while (!cpu.clock (1)) {};
}
