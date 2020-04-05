#ifndef H_INTERNALBUS16
#define H_INTERNALBUS16

#include <sys/types.h>

/****************************************************************************
 * Internal CPU bus
 ****************************************************************************/
class InternalBus16 {
private:
    u_int16_t value;
public:
    inline void write (u_int16_t _value) {
        value = _value;
    };
    inline u_int16_t read (void) const {
        return value;
    };
};

#endif
