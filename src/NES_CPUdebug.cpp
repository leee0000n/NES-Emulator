#include "NES_CPUdebug.h"
#include "NES_CPU.h"

#include <string>
#include <iostream>

/// Converts a character to a hex string representation
/// @param chr Character to convert to hex string representation
/// @return Hex string representing character
static std::string charToHex(Byte chr) {
	const int nibbleLo = chr % 16;
	const int nibbleHi = chr >> 4;

	std::string rtn;

	if (nibbleHi > 9) {
		switch (nibbleHi)
		{
		case 10:
			rtn += 'A';
			break;
		case 11:
			rtn += 'B';
			break;
		case 12:
			rtn += 'C';
			break;
		case 13:
			rtn += 'D';
			break;
		case 14:
			rtn += 'E';
			break;
		case 15:
			rtn += 'F';
			break;
		default:
			break;
		}
	}
	else {
		rtn += nibbleHi + 48;
	}

	if (nibbleLo > 9) {
		switch (nibbleLo)
		{
		case 10:
			rtn += 'A';
			break;
		case 11:
			rtn += 'B';
			break;
		case 12:
			rtn += 'C';
			break;
		case 13:
			rtn += 'D';
			break;
		case 14:
			rtn += 'E';
			break;
		case 15:
			rtn += 'F';
			break;
		default:
			break;
		}
	}
	else {
		rtn += nibbleLo + 48;
	}

	return rtn;
}

void NES_CPUdebug::printMemory(int start, int end) {
	for (int pageNum = start; pageNum <= end; pageNum++) {
		for (int i = 256 * pageNum; i < 256 * (pageNum + 1); i++) {
			if (i % 16 == 0) std::cout << "\n";

			// Minus 1 from i since nes_cpu->immediatePeek() increments input by 1
			std::cout << charToHex(nes_cpu->immediatePeek(i - 1)) << " ";
		}
		std::cout << "\n";
	}
}
