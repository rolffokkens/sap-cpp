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
    unsigned char ram2[256] = { 0x51, 0x01 // [00] LDI B,0x01  # Set B to dec value 1
                              , 0x50, 0x05 // [02] LDI A,0x05  # Set A to counter value 5
                              , 0xe0       // [04] OUT A       # Output A
                              , 0x31       // [05] SUB B       # Subtract 1 (B) from A
                              , 0xe0       // [06] OUT A       # Output A
                              , 0x65, 0x05 // [07] JMP NZ,0x05 # Repeat loop if non zero at [05]
                              , 0xf0       // [09] HLT         # HALT
                              };

    CPU2 cpu (ram2);

    while (!cpu.clock (1)) {};
}
