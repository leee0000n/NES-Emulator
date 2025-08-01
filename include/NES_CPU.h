// ReSharper disable All
#pragma once

#include <array>
#include <vector>

typedef unsigned char Byte;
typedef unsigned short Word;

constexpr bool HI_BYTE_ENABLED = true;
constexpr bool HI_BYTE_DISABLED = false;

constexpr int CARRY = 1;
constexpr int ZERO = 2;
constexpr int INTERRUPT_DISABLE = 4;
constexpr int DECIMAL = 8;
constexpr int BREAK = 16;
constexpr int OVRFLOW = 64;
constexpr int NEGATIVE = 128;

/*
7  bit  0
---- ----
NV1B DIZC
|||| ||||
|||| |||+- Carry
|||| ||+-- Zero
|||| |+--- Interrupt Disable
|||| +---- Decimal
|||+------ (No CPU effect; see: the B flag)
||+------- (No CPU effect; always pushed as 1)
|+-------- Overflow
+--------- Negative
*/

/// <summary>
/// CPU for the nes
/// </summary>
class NES_CPU {
private:
	// Number of cycles current instruction takes
	int cycleCount;

	Byte A; // Accumulator
	Byte X; // X index register
	Byte Y; // Y index register
	Byte S; // Stack pointer, $0100 - $01ff

	// from bit 7 to bit 0 these are the negative(N), overflow(V), reserved, break (B), decimal(D), interrupt disable(I), zero(Z) and carry(C) flag
	Byte P; // Status flags

	Word PC; // Program counter 

	std::array<Byte, 65536> memory;
public:
	/// <summary>
	/// Power up cpu by setting necessary values
	/// </summary>
	void power_up();

	/// <summary>
	/// Reset cpu by setting necessary values
	/// </summary>
	void reset();

	/// <summary>
	/// Main loop for instruction doing  TODO: RENAME
	/// </summary>
	void run();

	/// <summary>
	/// Set multiple, contigous bytes in memory to a specific sequence of bytes
	/// </summary>
	/// <param name="start"> Starting memory addres</param>
	/// <param name="source"> Sequence of bytes to add</param>
	/// <returns></returns>
	bool setBytes(int start, std::vector<int> source);

	/// <summary>
	/// Print value stored in all registers
	/// </summary>
	//void printRegisters();

	/// <summary>
	/// Peek at data stored at specific address
	/// </summary>
	/// <param name="address"> address of data</param>
	/// <returns> Data at specified address</returns>
	Byte peek(Word address);

	/// <summary>
	/// Get the operand of the opcode
	/// </summary>
	/// <param name="address"> Address of opcode </param>
	/// <returns> Operand of the opcode</returns>
	Byte immediatePeek(Word address);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="address"></param>
	/// <returns></returns>
	Byte zeroPagePeek(Word address);

	Byte zeroPageXPeek(Word address);

	Byte zeroPageYPeek(Word address);

	Byte absolutePeek(Word address);

	Byte absoluteXPeek(Word address);

	Byte absoluteYPeek(Word address);

	Byte indirectXPeek(Word address);

	Byte indirectYPeek(Word address);


	void zeroPageSet(Word address, Byte data);

	void zeroPageXSet(Word address, Byte data);

	void zeroPageYSet(Word address, Byte data);

	void absoluteSet(Word address, Byte data);

	void absoluteXSet(Word address, Byte data);

	void absoluteYSet(Word address, Byte data);

	void indirectXSet(Word address, Byte data);

	void indirectYSet(Word address, Byte data);



	/// <summary>
	/// Set data at address
	/// </summary>
	/// <param name="address"> Address of memory to set data</param>
	/// <param name="data"> 8 bit data to set</param>
	void set(Word address, Byte data);

	void setCycleCount(int cycleCount);


	Byte getA() const;
	Byte getX() const;
	Byte getY() const;
	Byte getS() const;
	Byte getP() const;

	Word getPC() const;

	void setA(Byte A);
	void setX(Byte X);
	void setY(Byte Y);
	void setS(Byte S);
	void setP(Byte P);

	void setPC(Word PC);
	void incrementPCBy(Word increment);

	void pushStack(Byte data);

	Byte pullStack();

	void setCarry();
	void clearCarry();
	bool isCarrySet() const;

	void setZero();
	void clearZero();
	bool isZeroSet() const;

	void setInterruptDisable();
	void clearInterruptDisable();
	bool isInterruptDisableSet() const;

	void setDecimal();
	void clearDecimal();
	bool isDecimalSet() const;

	void setBreak();
	void clearBreak();
	bool isBreakSet() const;

	void setOverflow();
	void clearOverflow();
	bool isOverflowSet() const;

	void setNegative();
	void clearNegative();
	bool isNegativeSet() const;
};

extern NES_CPU* nes_cpu;