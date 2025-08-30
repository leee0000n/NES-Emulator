#pragma once

#include <string>

class NES_PPUdebug {
public:
	static std::string ppuTrace;

	static void addPPUTraceHeader();

	static void logPPUDotSkip();
	static void logNoPPUDotSkip();
	static void logPPURenderingToggled(bool isRenderingEnabled);
	static void logRegisterUpdate(std::string name, int prevVal, int newVal);

	static void logVBLANKSet();
	static void logVBLANKClear();

	static void logAllRegisters();

	static void writePPUTrace(std::string path);
};