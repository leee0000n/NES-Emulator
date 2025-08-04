#include "NES_CPUdebug.h"
#include "NES_CPU.h"

#include <string>
#include <iostream>
#include <fstream>

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

std::string NES_CPUdebug::cpuTrace = "";

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

void NES_CPUdebug::printRegisters() {
	std::cout << "A:" << charToHex(nes_cpu->getA()) << " ";
	std::cout << "X:" << charToHex(nes_cpu->getX()) << " ";
	std::cout << "Y:" << charToHex(nes_cpu->getY()) << " ";
	std::cout << "P:" << charToHex(nes_cpu->getP()) << " ";
	std::cout << "SP:" << charToHex(nes_cpu->getS()) << " ";
	// IGNORE PPU FOR NOW
	std::cout << "CYC:" << nes_cpu->getTotalCycleCount() << "\n";
}

void NES_CPUdebug::logCPUState() {
	std::string cpuLog = "";
	//cpuLog += "opcode: " + charToHex(nes_cpu->correctPeek(nes_cpu->getPC())) + " ";
	cpuLog += "A:" + charToHex(nes_cpu->getA()) + " ";
	cpuLog += "X:" + charToHex(nes_cpu->getX()) + " ";
	cpuLog += "Y:" + charToHex(nes_cpu->getY()) + " ";
	cpuLog += "P:" + charToHex(nes_cpu->getP()) + " ";
	cpuLog += "SP:" + charToHex(nes_cpu->getS()) + "\n";
	// TODO: add ppu scanlines and cycles logging

	cpuTrace += cpuLog;
}

void NES_CPUdebug::addCPUTraceToFile(std::string path) {
	// Open a file for writing
	std::ofstream outFile(path);

	if (outFile.is_open()) {
		outFile << cpuTrace;  // Write the string
		outFile.close();  // Close the file
		std::cout << "File written successfully.\n";
	}
	else {
		std::cerr << "Failed to open file for writing.\n";
	}
}
