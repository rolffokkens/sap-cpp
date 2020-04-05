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
    unsigned char ram2[65536] = { 0x00, 0x00, 0x80 // [0000] SETFP 0x8000 # Initialize SP, FP
                                , 0x10, 0xFC, 0xFF // [0003] SETSP 0xFFFC # Set SP
                                , 0x20, 0x45, 0x23 // [0006] PUSHI 0x2345
                                , 0x20, 0x56, 0x34 // [0006] PUSHI 0x3456
                                , 0xA2             // [0009] ADD

                                , 0x10, 0x00, 0x00 // [000C] SETSP 0x0000 # Set SP
                                };

    CPU16 cpu (ram2);

    while (!cpu.clock (2)) {};
}
