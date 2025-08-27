#include "NES_PPUdebug.h"
#include "NES_PPU.h"

#include <fstream>
#include <iostream>

std::string NES_PPUdebug::ppuTrace = "";

using namespace std;

void NES_PPUdebug::addPPUTraceHeader() {
	ppuTrace += "Frame,Cycle,Scanline,Dot\n";
}

void NES_PPUdebug::logPPUDotSkip() {
	ppuTrace += to_string(ppu->getFrameCount) + " , " + to_string(ppu->getPPUCycle()) + " , " + to_string(ppu->getScanlineNum()) + " , " + to_string(ppu->getPPUDot()) + " , " + (ppu->getIsFrameOdd ? "odd frame" : "even frame") + "\n";
}

void NES_PPUdebug::logPPURenderingToggled(bool isRenderingEnabled) {
	ppuTrace += to_string(ppu->getFrameCount) + " , " + to_string(ppu->getPPUCycle()) + " , " + to_string(ppu->getScanlineNum()) + " , " + to_string(ppu->getPPUDot()) + " , " + (ppu->getIsFrameOdd ? "odd frame" : "even frame") "||" + "NEXT DOT SKIPPED\n";
}

void NES_PPUdebug::writePPUTrace(std::string path) {
	std::ofstream file(path);
	if (file.is_open()) {
		file << ppuTrace;
		file.close();
	}
	else {
		std::cerr << "Failed to open file: " << path << "\n";
	}
}