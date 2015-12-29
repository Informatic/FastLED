#ifndef __INC_FASTPIN_XTENSA_H
#define __INC_FASTPIN_XTENSA_H

FASTLED_NAMESPACE_BEGIN

#if defined(FASTLED_FORCE_SOFTWARE_PINS)
#warning "Software pin support forced, pin access will be sloightly slower." // TODO: ??
#define NO_HARDWARE_PIN_SUPPORT
#undef HAS_HARDWARE_PIN_SUPPORT
#else

/// Class definition for a Pin where we know the port registers at compile time for said pin.  This allows us to make
/// a lot of optimizations, as the inlined hi/lo methods will devolve to a single io register write/bitset.
template<uint8_t PIN, uint16_t _MASK> class _XTENSAPIN {
public:
	typedef volatile uint32_t * port_ptr_t;
	typedef uint16_t port_t;

	inline static void setOutput() { pinMode(PIN, OUTPUT); } // TODO: MUX config { GPIO_REG_WRITE(GPIO_ENABLE_W1TS_ADDRESS, _MASK); }
	inline static void setInput() { pinMode(PIN, INPUT); } // TODO: MUX config { GPIO_REG_WRITE(GPIO_ENABLE_W1TC_ADDRESS, _MASK); }

	inline static void hi() __attribute__ ((always_inline)) { GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, _MASK); }
	inline static void lo() __attribute__ ((always_inline)) { GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, _MASK); }
	inline static void set(register uint8_t val) __attribute__ ((always_inline)) { GPIO_REG_WRITE(GPIO_OUT_ADDRESS, _MASK); }

	inline static void strobe() __attribute__ ((always_inline)) { toggle(); toggle(); }

	inline static void toggle() __attribute__ ((always_inline)) { if(GPIO_REG_READ(GPIO_OUT_ADDRESS) & _MASK) { lo(); } else { hi(); } }

	inline static void hi(register port_ptr_t port) __attribute__ ((always_inline)) { hi(); }
	inline static void lo(register port_ptr_t port) __attribute__ ((always_inline)) { lo(); }
	inline static void fastset(register port_ptr_t port, register uint8_t val) __attribute__ ((always_inline)) { set(val); }

	inline static port_t hival() __attribute__ ((always_inline)) { return GPIO_REG_READ(GPIO_OUT_ADDRESS) | _MASK; }
	inline static port_t loval() __attribute__ ((always_inline)) { return GPIO_REG_READ(GPIO_OUT_ADDRESS) & ~_MASK; }
	inline static port_ptr_t port() __attribute__ ((always_inline)) { return ETS_UNCACHED_ADDR(PERIPHS_GPIO_BASEADDR + GPIO_OUT_ADDRESS); }
	inline static port_t mask() __attribute__ ((always_inline)) { return _MASK; }
};



/// AVR definitions for pins.  Getting around  the fact that I can't pass GPIO register addresses in as template arguments by instead creating
/// a custom type for each GPIO register with a single, static, aggressively inlined function that returns that specific GPIO register.  A similar
/// trick is used a bit further below for the ARM GPIO registers (of which there are far more than on AVR!)
#define _DEFPIN_XTENSA(_PIN) template<> class FastPin<_PIN> : public _XTENSAPIN<_PIN, 1 << _PIN> {};

#define MAX_PIN 15

/// NOTE: Pins 6, 7, 8 and 11 may be unavailable, because they are used by SPI
_DEFINE_XTENSA(0); _DEFINE_XTENSA(1); _DEFINE_XTENSA(2); _DEFINE_XTENSA(3);
_DEFINE_XTENSA(4); _DEFINE_XTENSA(5); _DEFINE_XTENSA(6); _DEFINE_XTENSA(7);
_DEFINE_XTENSA(8); _DEFINE_XTENSA(9); _DEFINE_XTENSA(10); _DEFINE_XTENSA(11);
_DEFINE_XTENSA(12); _DEFINE_XTENSA(13); _DEFINE_XTENSA(14); _DEFINE_XTENSA(15);

#endif // FASTLED_FORCE_SOFTWARE_PINS

FASTLED_NAMESPACE_END

#endif // __INC_FASTPIN_XTENSA_H
