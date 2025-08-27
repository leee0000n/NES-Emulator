#include "App.h"
#include "NES_CPU.h"
#include "NES_PPU.h"
#include "NES_APU.h"
#include "NES_CPUdebug.h"
#include "NES_PPUdebug.h"
#include "Render.h"

#include <iostream>


void init(std::string s) {
	nes_cpu->loadROM(s);
	ppu->loadCHRROM(s);
}

void App::run() {
	nes_cpu = new NES_CPU();
	ppu = new NES_PPU();
	apu = new NES_APU();

	std::string game = "C:/Users/leon_/source/repos/leee0000n/NES-Emulator/resource/pacman.nes";
	std::string test = "C:/Users/leon_/source/repos/leee0000n/NES-Emulator/resource/testnes/10even_odd_timing.nes";

;
	init(test);
	ppu->loadPalFile("");

	// Set program counter to start of instructions
	Word startaddress = nes_cpu->correctPeek(0xFFFC) + (nes_cpu->correctPeek(0xFFFD) << 8);
	nes_cpu->setPC(startaddress);

	render::initSDL2();

	bool running = false;
	bool reset = false;
	bool started = false;
	int loop = 0;
	// Run program
	while (true) {
		loop++;
		for (int i = 0; i < 3; i++) {
			ppu->runPPUCycle();
		}
		
		nes_cpu->runCPUCycle();
		apu->runAPUCycle();

		if (nes_cpu->finish) break;


		if (loop == 10000000) {
			for (int i = 0; i < 100; i++) std::cout << "\n";
			NES_CPUdebug::printMemoryMirrored(0x60, 0x60);
			loop = 0;
		}

		Byte testCode = nes_cpu->peek(0x6000);
		if (testCode == 0x80) {
			if (!running) std::cout << "Running\n";
			running = true;
			reset = false;
			started = true;
		}
		else if (testCode == 0x81) {
			if (!reset) std::cout << "Reset required\n";
			running = false;
			reset = true;
			started = true;
		}
		else if (testCode < 0x80 && started) {
			std::cout << "Test complete";
			break;
		}
		
	}


	render::closeSDL2();


	// print pages
	NES_CPUdebug::printMemoryMirrored(0x60, 0x60);

	NES_CPUdebug::addCPUTraceToFile("C:/Users/leon_/source/repos/NES-Emulator/resource/cpuTrace.txt");

	NES_PPUdebug::writePPUTrace("C:/Users/leon_/source/repos/NES-Emulator/resource/ppuTrace.txt");
}
