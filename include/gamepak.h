#include <cstdint>

class mapper {
	uint_least16_t prgBanks = 0, chrBanks = 0;
	
public:
	mapper();
	~mapper();
}