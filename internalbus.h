#ifndef H_INTERNALBUS
#define H_INTERNALBUS

/****************************************************************************
 * Internal CPU bus
 ****************************************************************************/
class InternalBus {
private:
    unsigned char value;
public:
    inline void write (unsigned char _value) {
        value = _value;
    };
    inline unsigned char read (void) const {
        return value;
    };
};

#endif
