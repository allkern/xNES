#pragma once
#include <iostream>
#include <cstdint>
#include <string>
#include <array>

class cpu {
	// NES Registers:
	uint_least16_t pc = 0x0600; // PC: Program Counter, 16-bits long
	uint_least8_t  sp = 0xfd,	// SP: Stack Pointer, 8-bits long
				   a  = 0x00,   // A : Accumulator, 8-bits long
				   x  = 0x00,	// X : Index X, 8-bits long
				   y  = 0x00,	// Y : Index T, 8-bits long
				   p  = 0x34,   // P : Status Register, 8-bits long, IRQ Disabled at startup
	// Helper Registers:
				   ufetched = 0, // fetched: 
				   opcode = 0,
				   cycles = 0;
	 int_least16_t ifetched = 0;

	// Bus:
	bus* Bus = defaultBus.getThisReference();

	#define unsignedf ufetched
	#define signedf   ifetched

	// Addressing modes:
	addrmode(imp, false, this->a   								   , 0, 0, unsignedf); // Implied
	addrmode(acc, false, this->a   								   , 0, 0, unsignedf); // Accumulator
	addrmode(imm, true , this->pc+1								   , 0, 1, unsignedf); // Immediate
	addrmode(zp0, true , this->pc+1								   , 0, 1, unsignedf); // Zero Page
	addrmode(abs, false, ((read(this->pc+2)<<8) | read(this->pc+1)), 0, 2, unsignedf); // Absolute
	addrmode(rel, false, read(this->pc+1)						   , 0, 1,   signedf); // Relative
	addrmode(ind, true , ((read(this->pc+2)<<8) | read(this->pc+1)), 0, 2, unsignedf); // Indirect
	addrmode(aix, true , ((read(this->pc+2)<<8) | read(this->pc+1)) + this->x, 0, 2, unsignedf); // Absolute Indexed X

	// Instructions:
	#define ld(r) this->##r = this->ufetched
	instruction(lda, imm, ld(a);, 2, 1);
	instruction(ldx, imm, ld(x);, 2, 1);
	instruction(ldy, imm, ld(y);, 2, 1);
	instruction(ora, imm, this->a |= this->ufetched;, 2, 1);

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
		uint_least8_t opcode = read(this->pc);
		if (this->pc == last_initialized) { std::cout << "Program end at PC=$" << fhex(this->pc, 4) << '\n'; program_finished = true; }
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