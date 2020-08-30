/*
 * This demonstrates CPU16 (see cpu2.h)
 *
 * It counts down from 5 to 0 and
 * outputs each counter value
 */

#include <iostream>
#include <cstddef>
#include <fstream>
#include <boost/format.hpp>
#include <boost/program_options.hpp>

#include "cpu16d.h"

using namespace std; 

namespace po = boost::program_options;

static po::variables_map vm;

int parse_args (int argc, char *argv[])
{
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "set debug level")
        ("debug", po::value<int>(),    "set debug level")
        ("image", po::value<string>(), "load specified image")
        ("arg",   po::value<int>(),    "set argument")
    ;

    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);    

    if (vm.count("help")) {
        cout << desc << "\n";
        return 1;
    }
    return 0;
}

int main (int argc, char *argv[])
{
    int i, debug = 0, param, ticks = 0;

    static u_int8_t ram2[65536] = { 0x20, 0x67, 0x00 // [0000] LDTORC 0x0067 # _init
                                  , 0xE0             // [0003] JUMP

                                                     //                      #         # int fib (int p) {
                                  , 0x10, 0xFC, 0xFF // [0004] SETSP  FP,-4  #         #     int i, j;

                                  , 0x30, 0x04, 0x00 // [0007] LDTORL 4      # push p  #     if (p < 2) {
                                  , 0x40             // [000A] FETCH
                                  , 0x60             // [000B] PUSH

                                  , 0x20, 0x02, 0x00 // [000C] LDTORC 2      # push #2 #
                                  , 0xB9             // [000F] CMP LT
                                  , 0x60             // [0010] PUSH

                                  , 0x20, 0x00, 0x00 // [0011] LDTORC 0      # push #0 #
                                  , 0xB0             // [0014] CMP
                                  , 0x20, 0x1E, 0x00 // [0015] LDTORC 0x001E #..else..
                                  , 0xE4             // [0018] JUMP Z

                                  , 0x30, 0x04, 0x00 // [0019] LDTORL 4      # push p  #         return p;
                                  , 0x40             // [001C] FETCH
                                  , 0x90             // [001D] RET           #         #     }

                                  , 0x30, 0x04, 0x00 // [001E] LDTORL 4      # push p  #
                                  , 0x40             // [0021] FETCH
                                  , 0x60             // [0022] PUSH

                                  , 0x20, 0x01, 0x00 // [0023] LDTORC 1      # push #1 #
                                  , 0xA3             // [0026] SUB           # p1 - #1 #
                                  , 0x60             // [0027] PUSH

                                  , 0x20, 0x04, 0x00 // [0028] LDTORC 4      # fib     #     i = fib (p - 1);
                                  , 0x80             // [002B] CALL
                                  , 0x11, 0x02, 0x00 // [002C] SETSP  SP,2   #

                                  , 0x60             // [002F] PUSH
                                  , 0x30, 0xFE, 0xFF // [0030] LDTORL -2     # push i  #
                                  , 0x50             // [0033] STORE

                                  , 0x30, 0x04, 0x00 // [0034] LDTORL 4      # push p  #         
                                  , 0x40             // [0037] FETCH
                                  , 0x60             // [0038] PUSH

                                  , 0x20, 0x02, 0x00 // [0039] LDTORC 2      # push #2 #
                                  , 0xA3             // [003C] SUB           # p1 - #2 #
                                  , 0x60             // [003D] PUSH

                                  , 0x20, 0x04, 0x00 // [003E] LDTORC 4      # fib     #     j = fib (p - 2);
                                  , 0x80             // [0041] CALL
                                  , 0x11, 0x02, 0x00 // [0042] SETSP  SP,2   #

                                  , 0x60             // [0045] PUSH
                                  , 0x30, 0xFC, 0xFF // [0046] LDTORL -4     # push j  #
                                  , 0x50             // [0049] STORE

                                  , 0x30, 0xFE, 0xFF // [004A] LDTORL -2     # push i  #
                                  , 0x40             // [004D] FETCH
                                  , 0x60             // [004E] PUSH

                                  , 0x30, 0xFC, 0xFF // [004F] LDTORL -4     # push j  #
                                  , 0x40             // [0052] FETCH

                                  , 0xA2             // [0053] ADD           # i + j   #     return i + j;
                                  , 0x90             // [0054] RET           #         #

                                  , 0x90             // [0055] RET           #         # }

                                  , 0x10, 0x00, 0x00 // [0056] SETSP  FP,0   #         # int main (int param) {
                                  , 0x30, 0x04, 0x00 // [0059] LDTORL 4      # param   # 
                                  , 0x40             // [005C] FETCH
                                  , 0x60             // [005D] PUSH

                                  , 0x20, 0x04, 0x00 // [005E] LDTORC 4      # fib     #    out fib (param);
                                  , 0x80             // [0061] CALL
                                  , 0x11, 0x02, 0x00 // [0062] SETSP  SP,2   #

                                  , 0xD0             // [0065] OUT           #         # out ...
                                  , 0x90             // [0055] RET           #         #     }


                                  , 0x00, 0xFE, 0xFF // [0056] SETFP  0xFFFE #
                                  , 0x20, 0xFE, 0xFF // [0059] LDTORC 0xFFFE # PARAM   #
                                  , 0x40             // [005C] FETCH
                                  , 0x60             // [005D] PUSH

                                  , 0x20, 0x56, 0x00 // [005E] LDTORC 0x0056 # main    #
                                  , 0x80             // [0061] CALL
                                  , 0xF0             // [0052] HALT          #         #
                                  };

    if (parse_args (argc, argv)) exit (1);

    debug = vm.count("debug") ? vm["debug"].as<int>() : 0;

    if (vm.count("image")) {
        ifstream myFile (vm["image"].as<string>(), ios::in | ios::binary);
        if (!myFile.read ((char *)ram2, sizeof (ram2))) {
        }
    }
    param = vm.count("arg") ? vm["arg"].as<int>() : 0;

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
