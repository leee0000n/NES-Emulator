#include "App.h"
#include "NES_CPU.h"
#include "NES_PPU.h"
#include "NES_CPUdebug.h"
#include "Render.h"

#include <iostream>

void App::run() {
	nes_cpu = new NES_CPU();
	ppu = new NES_PPU();

	std::string rom = "C:/Users/leon_/source/repos/NES-Emulator/resource/test_ppu_read_buffer.nes"
;
	nes_cpu->loadROM(rom);
	ppu->loadCHRROM(rom);
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
	while (true && loop != 50000000) {
		loop++;
		for (int i = 0; i < 3; i++) {
			ppu->run();
		}

		if (loop == 913195) {
			loop = loop;
		}
		
		nes_cpu->run();

		if (nes_cpu->finish) break;


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
}
