#pragma once
#include <iostream>
#include <cstdint>
#include <string>
#include <array>

#include "cpu-defines.h"

class cpu {
	// NES Registers:
	uint_least16_t pc = 0x0600;  // PC: Program Counter, 16-bits long
	uint_least8_t  sp = 0xfd,	 // SP: Stack Pointer, 8-bits long
				   a  = 0x00,    // A : Accumulator, 8-bits long
				   x  = 0x00,	 // X : Index X, 8-bits long
				   y  = 0x00,	 // Y : Index T, 8-bits long
				   p  = 0x34,    // P : Status Register, 8-bits long, IRQ Disabled at startup
	// Helper Variables:
				   fetched = 0,  // fetched: Contains the value last fetched through an addressing mode
				   opcode = 0,   // opcode: Contains the last fetched opcode
				   cycles = 0,   // cycles: Contains the amount of cycles needed to be consumed before the next instruction
				   addr_abs = 0; // addr_abs: Contains the last fetched absolute address
					 next = this->pc + 1;
	int_least16_t  addr_rel = 0; // addr_rel: Contains the last fetched relative address

	// Bus:
	bus* Bus = defaultBus.getThisReference();
	inline void sf(uint_least8_t m) { this->p |= m; } // Set flag
	inline void rf(uint_least8_t m) { this->p &= ~m; } // Reset flag
	inline void tf(uint_least8_t m) { this->p ^= m; } // Toggle flag

	// Addressing modes:
	#define next this->pc + 1
	addrmode(imp, false, this->a   								   , 0, 0, fetched,); // Implied
	addrmode(acc, false, this->a   								   , 0, 0, fetched,); // Accumulator
	addrmode(imm, true , next								   , 0, 1, fetched,); // Immediate
	addrmode(zp0, true , next								   , 0, 1, addr_abs,); // Zero Page
	addrmode(abs, false, rw(next):, 0, 2, addr_abs,); // Absolute
	addrmode(rel, false, (int_least16_t)rb(next), 0, 1, addr_rel,); // Relative
	addrmode(ind, true , rw(next), 0, 2, fetched,); // Indirect

	// Instructions:
	#define ld(r) this->##r = this->fetched
	instruction(lda, imm, ld(a);, 2, 1);
	instruction(ldx, imm, ld(x);, 2, 1);
	instruction(ldy, imm, ld(y);, 2, 1);
	instruction(ora, imm, this->a |= this->fetched;, 2, 1);

	// Signals:
	void clk() {
		if (this->cycles > 0) {
			--cycles;
		} else {
			sf(flags::u);
		}
	}

	void rst() {
		this->pc = rw(0xfffc);
		this->a  = this->x = this->y = 0;
		this->sp = 0xfd;
		sf(flags::u);
		fetched  = 0;
		cycles   = 8;
	}

	void irq() {
		// To-do
	}

	void nmi() {
		// To-do
	}

public:
	inline void initializeMemoryFromString(std::string s, int base) {
		int i = base;
		for (auto c: s) {
			write(i, c);
			++i;
		}
		last_initialized = i;
	}

	#define fhex(n, f) std::setfill('0') << std::setw(f) << std::hex << n
	inline void executeOne() {
		this->opcode = read(this->pc);
		switch (opcode) {
			case 0xa9: _instruction(lda, imm); break;
			case 0xa2: _instruction(ldx, imm); break;
			case 0xa0: _instruction(ldy, imm); break;
			case 0x09: _instruction(ora, imm); break;
		}
	}

	inline void logState() {
		std::cout << "pc = 0x" << std::hex << (int)this->pc << "\n"
				  << "sp = 0x" << std::hex << (int)this->sp << "\n"
				  << "a  = 0x" << std::hex << (int)this->a  << "\n"
				  << "x  = 0x" << std::hex << (int)this->x  << "\n"
				  << "y  = 0x" << std::hex << (int)this->y  << "\n"
				  << "p  = 0x" << std::hex << (int)this->p  << "\n";
	}
};
