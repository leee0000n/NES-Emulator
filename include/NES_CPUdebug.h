#pragma once

#include "NES_CPU.h"

class NES_CPUdebug {
public:
	/// <summary>
	/// Print all pages between start and end pag, inclusive
	/// </summary>
	/// <param name="start"> start page number to print</param>
	/// <param name="end"> end page number to print </param>
	static void printMemory(int start, int end);

	static void printRegisters();
	static void readData();
	static void addInstruction(Byte opcode, Word address, Word data = 0);
	static void addProgram(Byte bytes[], int length);
	static void setInstruction(Word address, Byte data);
	static void setData(Word address, Byte hi_byte, Byte lo_byte, bool hi_enabled = false);
	static void opcodeToAsm(char opcode);
};