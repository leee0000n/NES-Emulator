#include "App.h"
#include "NES_CPU.h"
#include "NES_PPU.h"
#include "NES_CPUdebug.h"
#include "Render.h"

void App::run() {
	nes_cpu = new NES_CPU();
	ppu = new NES_PPU();

	std::string rom = "C:/Users/leon_/source/repos/NES-Emulator/resource/nestest.nes"
;
	nes_cpu->loadROM(rom);
	ppu->loadCHRROM(rom);

	// Set program counter to start of instructions
	Word startaddress = nes_cpu->correctPeek(0xFFFC) + (nes_cpu->correctPeek(0xFFFD) << 8);
	nes_cpu->setPC(0xc000);

	render::initSDL2();

	// Run program
	//while (true) {
	//	if (ppu->getPPUCycle() % 3 == 0) {
	//		nes_cpu->run();
	//	}
	//	//ppu->run();
	//}

	nes_cpu->run();

	render::closeSDL2();


	// print pages
	NES_CPUdebug::printMemoryMirrored(0xBF, 0xBF);

	NES_CPUdebug::addCPUTraceToFile("C:/Users/leon_/source/repos/NES-Emulator/resource/cpuTrace.txt");
}
