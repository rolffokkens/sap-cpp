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
    /******************************************************************************************
    *                                                                                         *
    *  BROKEN!!!! ISA changed                                                                 *
    *                                                                                         *
    ******************************************************************************************/



    unsigned char ram2[65536] = { 0x00, 0x00, 0x80 // [0000] SETFP 0x8000 # Initialize SP, FP
                                , 0x10, 0xFC, 0xFF // [0003] SETSP -4     # Set SP (4 bytes on FP)
                                , 0x20, 0x00, 0x00 // [0006] PUSHI 0
                                , 0x60, 0xFE, 0xFF // [0009] POPL  -2     # i = 0
                                , 0x20, 0x0A, 0x00 // [000C] PUSHI 10
                                , 0x60, 0xFE, 0xFF // [000F] POPL  -4     # j = 10

                                , 0x20, 0x00, 0x00 // [0012] PUSHI 0      # (retval)
                                , 0x20, 0x01, 0x00 // [0015] PUSHI 1      # p1
                                , 0x20, 0x02, 0x00 // [0018] PUSHI 2      # p2
                                , 0x70, 0x21, 0x00 // [001B] CALLI 0x0021

                                , 0xD0, 0x2F, 0x00 // [001E] JUMPI 0x002F

                                , 0x10, 0xFE, 0xFF // [0021] SETSP -2     # Set SP ( 2 bytes on FP)
                                , 0x40, 0x06, 0x00 // [0024] PUSHL 6      # push p1
                                , 0x40, 0x04, 0x00 // [0027] PUSHL 4      # push p2
                                , 0xA2             // [002A] ADD
                                , 0x60, 0x08, 0x00 // [002B] POPL  8      # pop retval
                                , 0x90             // [002E] RET

                                , 0x20, 0x01, 0x00 // [002F] PUSHI 1      # p1
                                , 0x20, 0x02, 0x00 // [0032] PUSHI 2      # p2
                                , 0xA3             // [0025] SUB
                                , 0xF0             // [0028] HALT

                                , 0x10, 0x00, 0x00 // [000C] SETSP 0x0000 # Set SP
                                };

    Ram8 RAM8 (sizeof (ram2), ram2);

    CPU16 cpu (&RAM8);

    while (!cpu.clock (2)) {};
}
