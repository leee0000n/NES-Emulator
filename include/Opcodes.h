#pragma once

#include "NES_CPU.h"

namespace opcodes {
	// Possibly change code so that there is a function for each addressing mode
	extern void (*opcodeFuncPointers[256])(Byte, Word);
	void initFuncArray();
}
