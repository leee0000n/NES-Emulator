#include "App.h"
#include "NES_CPU.h"
#include "NES_PPU.h"
#include "NES_CPUdebug.h"
#include "Render.h"

void App::run() {
	nes_cpu = new NES_CPU();
	ppu = new NES_PPU();

	// Set program counter to start of instructions
	nes_cpu->setPC(0xC000);

	nes_cpu->loadROM("C:/Users/leon_/source/repos/NES-Emulator/resource/nestest.nes");

	// Run program
	nes_cpu->run();

	render::initSDL2();
	render::closeSDL2();

	// print pages
	NES_CPUdebug::printMemoryMirrored(0x0000, 0x0000);

	NES_CPUdebug::addCPUTraceToFile("C:/Users/leon_/source/repos/NES-Emulator/resource/cpuTrace.txt");
}
