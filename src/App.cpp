#include "App.h"
#include "NES_CPU.h"
#include "NES_CPUdebug.h"


// Tolower code
//A0,00,B1,80,F0,11,C9,41,90,06,C9,5B,B0,02,09,20,91,82,C8,D0,ED,91,82,18

void App::run() {
	nes_cpu = new NES_CPU();

	//// Set src and target address
	//nes_cpu->setBytes(0x0080, {0x00, 0x04}); // Store source address
	//nes_cpu->setBytes(0x0082, {0x00, 0x05}); // Store target address
	//nes_cpu->setBytes(0x0100, {
	//	                  0xA0, 0x00, 0xB1, 0x80, 0xF0, 0x11, 0xC9, 0x41, 0x90, 0x06, 0xC9, 0x5B, 0xB0, 0x02, 0x09,
	//	                  0x20, 0x91, 0x82, 0xC8, 0xD0, 0xED, 0x38, 0x60, 0x91, 0x82, 0x18, 0x60
	//                  });

	//// String to convert to lower
	//char tolower[] = "DfJNJjdf";
	//const std::vector<int> d(tolower, tolower + sizeof tolower);


	//// Add string to convert to lower to target address
	//nes_cpu->setBytes(0x0400, d);

	// Set program counter to start of instructions
	nes_cpu->setPC(0xC000);

	nes_cpu->loadROM("C:/Users/leon_/source/repos/NES-Emulator/resource/nestest.nes");

	// Run program
	nes_cpu->run();


	// print pages
	NES_CPUdebug::printMemoryMirrored(0x0000 / 256, 0x0000 / 256);
}
