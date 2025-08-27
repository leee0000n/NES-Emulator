#pragma once

#include "NES_CPU.h"

class NES_CPUdebug {
private:
	static std::string cpuTrace;
public:
	/// <summary>
	/// Print all pages between start and end pag, inclusive. Does not
	/// take into account mirrored parts of memory
	/// </summary>
	/// <param name="start"> start page number to print</param>
	/// <param name="end"> end page number to print </param>
	static void printMemory(int start, int end);

	/// <summary>
	/// Print all pages within range. Take into account mirrored parts of
	/// memory
	/// </summary>
	/// <param name="start"> start page</param>
	/// <param name="end"> end page</param>
	static void printMemoryMirrored(int start, int end);

	/// <summary>
	/// Logs the state of the CPU register's in a private string attriute
	/// </summary>
	static void logCPUState();

	/// <summary>
	/// Add all logged CPU states stored in private string attribute to specifed file
	/// </summary>	
	/// <param name="path"> path to store CPU trace</param>
	static void addCPUTraceToFile(std::string path);


	static void readData();
	static void addInstruction(Byte opcode, Word address, Word data = 0);
	static void addProgram(Byte bytes[], int length);
	static void setInstruction(Word address, Byte data);
	static void setData(Word address, Byte hi_byte, Byte lo_byte, bool hi_enabled = false);
	static void opcodeToAsm(char opcode);
};