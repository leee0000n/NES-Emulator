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
		for (int address = 256 * pageNum; address < 256 * (pageNum + 1); address++) {
			if (address % 16 == 0) std::cout << "\n";

			std::cout << charToHex(nes_cpu->peek(address)) << " ";
		}
		std::cout << "\n";
	}
}

void NES_CPUdebug::printMemoryMirrored(int start, int end) {
	if (start < 0 || start >= 256) {
		std::cerr << "start page not in range 0-255\n";
		return;
	}

	if (end < 0 || end >= 256) {
		std::cerr << "end page not in range 0-255\n";
		return;
	}

	if (end < start) {
		std::cerr << "end page less than start page\n";
		return;
	}

	for (int pageNum = start; pageNum <= end; pageNum++) {
		for (int address = 256 * pageNum; address < 256 * (pageNum + 1); address++) {
			if (address % 16 == 0) std::cout << "\n";
			
			std::cout << charToHex(nes_cpu->correctPeek(address)) << " ";
		}
		std::cout << "\n";
	}
}

