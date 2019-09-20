#define read(addr) Bus->access(addr)
#define write(addr, v) Bus->access(addr, v, false)

#define addrmode(name, memory_operand, fetch, cycles, len, fsign) addrmode_##memory_operand(name, fetch, cycles, len, fsign)
#define addrmode_true(name, fetch, _cycles, len, fsign) \
	inline void __cdecl addrmode_##name(void) { \
		uint_least16_t offset = ##fetch; \
		this->##fsign = read(offset); \
		this->pc += len; \
		this->cycles += _cycles; \
	}
#define addrmode_false(name, fetch, _cycles, len, fsign) \
	inline void __cdecl addrmode_##name(void) { \
		this->##fsign = fetch; \
		this->pc += len; \
		this->cycles += _cycles; \
	}

#define instruction(name, _addrmode_, code, _cycles, len) \
	inline void __cdecl instruction_##name##_##_addrmode_(void) { \
		addrmode_##_addrmode_(); \
		##code \
		this->pc += len; \
		this->cycles += _cycles; \
	}

#define _instruction(name, addrmode) instruction_##name##_##addrmode()