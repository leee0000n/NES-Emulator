#pragma once

#include "Constants.h"

#include <string>

class NES_PPUdebug {
public:
	static std::string ppuTrace;

	static void addPPUTraceHeader();

	static void logPPUDotSkip();
	static void logPPURenderingToggled(bool isRenderingEnabled);

	static void writePPUTrace(std::string path);
};