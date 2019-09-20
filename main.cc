#include "graphicsWrapper.h"
#include "cpu.h"

int main() {
	std::cout << "xNES Emulator v0.01\n\n"; 
	xNES_CPU.initializeMemoryFromString("\xa9\xa0\x09\x0a\xa2\xca\xa0\xfe", 0x0600);
	xNES_CPU.logState();
	for (;;) {
		system("pause");
		std::cout << '\n';
		xNES_CPU.executeOne();
		xNES_CPU.logState();
	}
	end:
	return 0;
}