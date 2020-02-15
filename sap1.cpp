/*
 * This demonstrates CPU1 (see cpu1.h)
 *
 * It counts down from 5 to 0 and
 * outputs each counter value
 */

#include <iostream>
#include <boost/format.hpp>
#include <cstddef>

#include "cpu1.h"

using namespace std; 

int main (void)
{
    unsigned char ram[16] = { 0x51 // [00] LDI 0x01
                            , 0x4e // [01] STA 0x0e # Start 1 in [0E]
                            , 0x55 // [02] LDI 0x05 # Set A to counter value 5
                            , 0xe0 // [03] OUT      # Output A
                            , 0x3e // [04] SUB 0x0e # Subtract 1 ([0E]) from A
                            , 0xe0 // [05] OUT      # Output A
                            , 0x88 // [06] JZ  0x08 # Go to [08] when A is zero
                            , 0x64 // [07] JMP 0x04 # Repeat the loop at [04]
                            , 0xf0 // [08] HLT      # HALT
                            };

    CPU1 cpu (ram);

    while (!cpu.clock (1)) {};
}
