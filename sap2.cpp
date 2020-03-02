/*
 * This demonstrates CPU2 (see cpu2.h)
 *
 * It counts down from 5 to 0 and
 * outputs each counter value
 */

#include <iostream>
#include <boost/format.hpp>
#include <cstddef>

#include "cpu2.h"

using namespace std; 

int main (void)
{
    unsigned char ram2[256] = { 0x42, 0x01 // [00] SET B,0x01  # Set B to dec value 1
                              , 0x41, 0x05 // [02] SET A,0x05  # Set A to counter value 5
                              , 0xe1       // [04] OUT A       # Output A
                              , 0x22       // [05] SUB B       # Subtract 1 (B) from A
                              , 0xe1       // [06] OUT A       # Output A
                              , 0x55, 0x05 // [07] JMP NZ,0x05 # Repeat loop if non zero at [05]

                              , 0x40, 0x00 // [09] SET SP,0x00
                              , 0xa2       // [0A] PSH B
                              , 0xb3       // [0B] POP C
                              , 0x41, 0xff // [0C] SET A,0xff
                              , 0x84       // [0D] LDI D

                              , 0xf0       // [09] HLT         # HALT
                              };

    CPU2 cpu (ram2);

    while (!cpu.clock (1)) {};
}
