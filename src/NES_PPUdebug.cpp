#include "NES_PPUdebug.h"
#include "NES_PPU.h"

#include <fstream>
#include <iostream>

std::string NES_PPUdebug::ppuTrace = "";

using namespace std;

void NES_PPUdebug::addPPUTraceHeader() {
	ppuTrace += "Frame | Cycle | Scanline | Dot | Frame Type\n";
	ppuTrace += "-------------------------------------------\n";
}

void NES_PPUdebug::logPPUDotSkip() {
	ppuTrace += to_string(ppu->getFrameCount()) + " , " + to_string(ppu->getPPUCycle()) + " , " + to_string(ppu->getPPUScanline()) + " , " + to_string(ppu->getPPUDot()) + " , " + (ppu->getIsFrameOdd() ? "odd frame" : "even frame") + " || NEXT DOT SKIPPED\n";
}

void NES_PPUdebug::logNoPPUDotSkip() {
	ppuTrace += to_string(ppu->getFrameCount()) + " , " + to_string(ppu->getPPUCycle()) + " , " + to_string(ppu->getPPUScanline()) + " , " + to_string(ppu->getPPUDot()) + " , " + (ppu->getIsFrameOdd() ? "odd frame" : "even frame") + " || NO DOT SKIPPED\n";
}

void NES_PPUdebug::logPPURenderingToggled(bool isRenderingEnabled) {
	ppuTrace += to_string(ppu->getFrameCount()) + " , " + to_string(ppu->getPPUCycle()) + " , " + to_string(ppu->getPPUScanline()) + " , " + to_string(ppu->getPPUDot()) + " , " + (ppu->getIsFrameOdd() ? "odd frame" : "even frame") + " || " + (isRenderingEnabled ? "Rendering ON" : "Rendering OFF") + "\n";
}

void NES_PPUdebug::logRegisterUpdate(std::string reg, int prev, int now) {
	ppuTrace += to_string(ppu->getFrameCount()) + " , " + to_string(ppu->getPPUCycle()) + " , " + to_string(ppu->getPPUScanline()) + " , " + to_string(ppu->getPPUDot()) + " , " + (ppu->getIsFrameOdd() ? "odd frame" : "even frame") + " || ";
	ppuTrace += " " + reg + " prev: " + to_string(prev) + " now: " + to_string(now) + "\n";
}

void NES_PPUdebug::logVBLANKSet() {
	ppuTrace += to_string(ppu->getFrameCount()) + " , " + to_string(ppu->getPPUCycle()) + " , " + to_string(ppu->getPPUScanline()) + " , " + to_string(ppu->getPPUDot()) + " , " + (ppu->getIsFrameOdd() ? "odd frame" : "even frame") + " || VBLANK SET\n";
}

void NES_PPUdebug::logVBLANKClear() {
	ppuTrace += to_string(ppu->getFrameCount()) + " , " + to_string(ppu->getPPUCycle()) + " , " + to_string(ppu->getPPUScanline()) + " , " + to_string(ppu->getPPUDot()) + " , " + (ppu->getIsFrameOdd() ? "odd frame" : "even frame") + " || VBLANK CLEARED\n";
}

void NES_PPUdebug::logAllRegisters() {
	ppuTrace += to_string(ppu->getFrameCount()) + " , " + to_string(ppu->getPPUCycle()) + " , " + to_string(ppu->getPPUScanline()) + " , " + to_string(ppu->getPPUDot()) + " , " + (ppu->getIsFrameOdd() ? "odd frame" : "even frame") + " || ";
	ppuTrace += to_string(ppu->getPPUCTRL()) + ", ";
	ppuTrace += to_string(ppu->getPPUMASK()) + ", ";
	ppuTrace += to_string(ppu->getPPUSTATUS()) + ", ";
	ppuTrace += to_string(ppu->getPPUSCROLL()) + ", ";
	ppuTrace += to_string(ppu->getPPUADDR()) + "\n";
}

void NES_PPUdebug::writePPUTrace(std::string path) {
	std::ofstream file(path);
	if (file.is_open()) {
		file << ppuTrace;
		file.close();
		std::cout << "PPU trace written successfully\n";
	}
	else {
		std::cerr << "Failed to open file: " << path << "\n";
	}
}