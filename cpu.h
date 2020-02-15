#ifndef H_CPU
#define H_CPU

#include "internalbus.h"
#include "register.h"
#include "ram.h"

class CPU {
protected:
    /****************************************************************************
     * Micro code is always executed conditional, conditions specified as
     * requirenents for Z en C flags. FLAG_NONE means: no requirement.
     ****************************************************************************/
    enum FlagRequire {
        FLAG_NONE = 0
    ,   FLAG_0    = 1
    ,   FLAG_1    = 2
    };

    /****************************************************************************
     * FlagsRegister - could be part of ALU
     ****************************************************************************/
    class FlagsRegister {
    private:
        char Z, C;
    public:
        FlagsRegister () {
            Z = 0;
            C = 0;
        };
        inline void set (char _Z, char _C) {
            Z = _Z;
            C = _C;
        };
        inline void get (char &_Z, char &_C) const {
            _Z = Z;
            _C = C;
        };
    };

    /****************************************************************************
     * Counter Register - currently only PC, could be SP in future too
     ****************************************************************************/
    class Counter: public Register {
    public:
        Counter (InternalBus &bus): Register (bus) { };
        void clock (void) {
            value++;
        };
    };

    InternalBus bus;
    Counter PC;
    Register IR;
    OutputRegister OUT;
    FlagsRegister F;
    int halted;
    int micro_ptr;

    /*
     * Constructor. Setup all components.
     */
    CPU (unsigned char *_ram)
        : bus ()
        , PC (bus)
        , IR (bus, 0x0f)
        , OUT (bus)
    {
        micro_ptr = 0;

        PC.reset ();
        IR.reset ();

        halted = 0;
    };

public:
    virtual int clock (int debug_level) = 0;
};

#endif
