#pragma once
#include <cstdint>
#include <array>

#define onRange(l, addr, h) ((addr <= h) && (addr >= l))

class bus {
	std::array <uint_least8_t, 0xffff> m = { 0 };

public:
	bus* getThisReference() { return this; }
	uint_least8_t access(uint_least16_t addr, uint_least8_t v = 0, bool rw = true) {
		if (onRange(0x0000, addr, 0xffff)) {
			if (rw) {
				return (uint_least8_t)this->m[addr];
			} else {
				this->m[addr] = v;
				return (uint_least8_t)true;
			}
		}
	}
}; bus defaultBus;

#undef onRange