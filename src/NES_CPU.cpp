
#include "NES_CPU.h"
#include "NES_PPU.h"
#include "Opcodes.h"

#include <iostream>
#include <string>
#include <fstream>

NES_CPU* nes_cpu;

/// <summary>
/// Check if given address is, not strictly, within bounds
/// </summary>
/// <param name="address"> address to check</param>
/// <param name="addressLowerBound"> lower bound, inclusive</param>
/// <param name="addressUpperBound"> upper bound, inclusive</param>
/// <returns> True if in bounds, false if not</returns>
static bool isAddressInRangeInclusive(Word address, Word addressLowerBound, Word addressUpperBound) {
	return address >= addressLowerBound &&  address <= addressUpperBound;
}

NES_CPU::NES_CPU() {
	this->cycleCount = 7; // Power up sequence takes 7 cycles
	this->totalCycleCount = 7;
	this->openBusValue = 0;
	this->isPRG_ROMMirrored = false;
	this->pageBoundaryCrossedOnPeek = false;
	this->NMI = false;
	this->power_up();

	opcodes::loadLegalOpcodes();
	opcodes::loadIllegalOpcodes();
}


void NES_CPU::power_up() {
	A = 0;
	X = 0;
	Y = 0;
	P = 0x24; // Interrupt request (IRQ) disabled
	S = 0xFD;
	memory[0x4017] = 0x00; // Frame irq enabled
	memory[0x4015] = 0x00; // All channels disabled

	for (int address = 0x0000; address < 0xFFFF; address++) {
		memory[address] = 0;
	}

	//for (int address = 0x4000; address <= 0x400F; address++) { // $4000-$400F = $00
	//	memory[address] = 0x00;
	//}

	//for (int address = 0x4010; address <= 0x4013; address++) { // $4010-$4013 = $00 ( Eliminator Boat Duel )
	//	memory[address] = 0x00;
	//}
}

void NES_CPU::reset() {
	S -= 3;
	P |= 0x04;
}

void NES_CPU::run() {
	if (cycleCount <= 0) {
		
		Byte opcode = correctPeek(PC);
		Word address = PC;
		
		// BREAK OPCODE
		if (opcode == 0) return;

		// Run instruction at PC
		opcodes::opcodeFuncPointers[opcode](opcode, address);

		this->pageBoundaryCrossedOnPeek = false;

		cycleCount--;
	}
	else cycleCount--;
}


bool NES_CPU::setBytes(int start, std::vector<int> source) {
	// Pre-conditions otherwise there is a possibility of index out of bounds error
	if (start + source.size() >= 256 * 256) return false;
	if (start < 0) return false;

	for (int i = 0; i < source.size(); i++) {
		memory[start + i] = source[i];
	}

	return true;
}

bool NES_CPU::loadROM(std::string path) {
	// Open the file in binary mode and move cursor to the end to get size
	std::ifstream file(path, std::ios::binary | std::ios::ate);

	if (!file) {
		std::cerr << "Failed to open file: " << path << "\n";
		return false;
	}

	// Get size and allocate buffer
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<char> buffer(size);

	if (file.read(buffer.data(), size)) {

		// Check if prg rom is 16kb or 32 kb
		if (buffer[4] == 1) nes_cpu->isPRG_ROMMirrored = true; // PRG ROM is 16kb
		else nes_cpu->isPRG_ROMMirrored = false; // PRG ROM is 32 kb

		// Load 16 KB after first 16 bytes
		for (int i = 0; i < 0x4000; i++) {
			nes_cpu->memory[0x8000 + i] = buffer[16 + i];
		}

		if (!isPRG_ROMMirrored) {
			// Load second set of 16kb if rom not mirrored
			for (int i = 0x4000; i < 0x10000; i++) {
				nes_cpu->memory[0xC000 + i] = buffer[16 + i];
			}
		}

	}
	else {
		std::cerr << "Error reading file.\n";
		return false;
	}

	return true;
}

Byte NES_CPU::peek(Word address) {
	return memory[address];
}

Byte NES_CPU::correctPeek(Word address) {
	// Mirrorred RAM
	if (isAddressInRangeInclusive(address, 0x0000, 0x1FFF)) {
		return memory[address % 0x0800];
	}

	// PPU registers
	if (isAddressInRangeInclusive(address, 0x2000, 0x3FFF)) {
		// Because ppu registers are mirrored every 8 bytes
		address = address % 8 + 0x2000;

		switch (address) {
		case 0x2000:
			return ppu->CpuPpuLatchRead();
		case 0x2001:
			return ppu->CpuPpuLatchRead();
		case 0x2002:
			return ppu->PPUSTATUSread();
		case 0x2003:
			return ppu->CpuPpuLatchRead();
		case 0x2004:
			return ppu->OAMDATAread();
		case 0x2005:
			return ppu->CpuPpuLatchRead();
		case 0x2006:
			return ppu->CpuPpuLatchRead();
		case 0x2007:
			return ppu->PPUDATAread();
		}
	}

	// OAMDMA address
	if (address == 0x4014) {
		return ppu->CpuPpuLatchRead();
	}

	// For mirrored RAM
	if (isPRG_ROMMirrored && isAddressInRangeInclusive(address, 0xC000, 0XFFFF)) {
		return memory[address % 0x4000 + 0x8000];
	}

	return memory[address];
}

Byte NES_CPU::immediatePeek(Word address) {
	return correctPeek(address + 1);
}

Byte NES_CPU::zeroPagePeek(Word address) {
	Word zeropageaddress = correctPeek(address + 1);
	return correctPeek(zeropageaddress);
}

Byte NES_CPU::zeroPageXPeek(Word address) {
	Word zeropageXaddress = correctPeek(address + 1) + X;
	return correctPeek(zeropageXaddress % 256);
}

Byte NES_CPU::zeroPageYPeek(Word address) {
	Word zeropageYaddress = correctPeek(address + 1) + Y;
	return correctPeek(zeropageYaddress % 256);
}

Byte NES_CPU::absolutePeek(Word address) {
	Word absoluteaddress = correctPeek(address + 1) + (correctPeek(address + 2) << 8);
	return correctPeek(absoluteaddress);
}

Byte NES_CPU::absoluteXPeek(Word address) {
	Word absoluteaddress = correctPeek(address + 1) + (correctPeek(address + 2) << 8);
	Word absoluteXaddress = absoluteaddress + X;

	// Used for adding cycles when page boudnary crossed
	if ((absoluteaddress & 0xFF00) != (absoluteXaddress & 0xFF00)) pageBoundaryCrossedOnPeek = true;

	return correctPeek(absoluteXaddress);
}

Byte NES_CPU::absoluteYPeek(Word address) {
	Word absoluteaddress = correctPeek(address + 1) + (correctPeek(address + 2) << 8);
	Word absoluteYaddress = absoluteaddress + Y;

	// Used for adding cycles when page boudnary crossed
	if ((absoluteaddress & 0xFF00) != (absoluteYaddress & 0xFF00)) pageBoundaryCrossedOnPeek = true;

	return correctPeek(absoluteYaddress);
}

Byte NES_CPU::indirectPeek(Word address) {
	Word indirectXPeek = correctPeek(address + 1);
	Word lookupaddress = correctPeek(indirectXPeek) + (correctPeek(indirectXPeek + 1) << 8);
	return correctPeek(lookupaddress);
}

Byte NES_CPU::indirectXPeek(Word address) {
	Word indirectXPeek = correctPeek(address + 1) + X;
	Word lookupaddress = correctPeek(indirectXPeek % 256) + (correctPeek((indirectXPeek + 1) % 256) << 8);
	return correctPeek(lookupaddress);
}

Byte NES_CPU::indirectYPeek(Word address) {
	Word indirectPeek = correctPeek(address + 1);
	indirectPeek = correctPeek(indirectPeek) + (correctPeek((indirectPeek + 1) % 256) << 8);
	Word indirectYPeek = indirectPeek + Y;

	// Used for adding cycles when page boudnary crossed
	if ((indirectYPeek & 0xFF00) != (indirectPeek & 0xFF00)) pageBoundaryCrossedOnPeek = true;

	return correctPeek(indirectYPeek);
}

void NES_CPU::set(Word address, Byte data) {
	memory[address] = data;
}

void NES_CPU::correctSet(Word address, Byte data) {
	// Ram and its mirrored sections
	if (isAddressInRangeInclusive(address, 0x0000, 0x1FFF)) {
		memory[address % 0x0800] = data;
		return;
	}

	// PPU registers
	// Mirrored every 8 bytes
	if (isAddressInRangeInclusive(address, 0x2000, 0x3FFF)) {
		address = address % 8 + 0x2000;

		switch (address) {
		case 0x2000:
			ppu->PPUCTRLwrite(data);
		case 0x2001:
			ppu->PPUMASKwrite(data);
		case 0x2002:
			ppu->CpuPpuLatchwrite(data);
		case 0x2003:
			ppu->OAMADDRwrite(data);
		case 0x2004:
			ppu->OAMDATAwrite(data);
		case 0x2005:
			ppu->PPUSCROLLwrite(data);
		case 0x2006:
			ppu->PPUADDRwrite(data);
		case 0x2007:
			ppu->PPUDATAwrite(data);
		}

		return;
	}

	// OAMDMA register
	if (address == 0x4014) {
		ppu->OAMDMAwrite(data);
		return;
	}

	// Mirrored PRG ROM
	if (isPRG_ROMMirrored && isAddressInRangeInclusive(address, 0xC000, 0XFFFF)) {
		memory[address % 0x4000 + 0x8000] = data;
		return;
	}

	memory[address] = data;
}

void NES_CPU::zeroPageSet(Word address, Byte data) {
	address = correctPeek(address + 1);
	correctSet(address, data);
}

void NES_CPU::zeroPageXSet(Word address, Byte data) {
	address = correctPeek(address + 1) + X;
	correctSet(address % 256, data);
}

void NES_CPU::zeroPageYSet(Word address, Byte data) {
	address = correctPeek(address + 1) + Y;
	correctSet(address % 256, data);
}

void NES_CPU::absoluteSet(Word address, Byte data) {
	address = correctPeek(address + 1) + (correctPeek(address + 2) << 8);
	correctSet(address, data);
}

void NES_CPU::absoluteXSet(Word address, Byte data) {
	address = X + correctPeek(address + 1) + (correctPeek(address + 2) << 8);
	correctSet(address, data);
}

void NES_CPU::absoluteYSet(Word address, Byte data) {
	address = Y + correctPeek(address + 1) + (correctPeek(address + 2) << 8);
	correctSet(address, data);
}

void NES_CPU::indirectXSet(Word address, Byte data) {
	address = correctPeek(address + 1) + X;
	address = correctPeek(address % 256) + (correctPeek((address + 1) % 256) << 8);
	correctSet(address, data);
}

void NES_CPU::indirectYSet(Word address, Byte data) {
	address = correctPeek(address + 1);
	address = Y + correctPeek(address) + (correctPeek((address + 1) % 256) << 8);
	correctSet(address, data);
}

Byte NES_CPU::getA() const {
	return A;
}

Byte NES_CPU::getX() const {
	return X;
}

Byte NES_CPU::getY() const {
	return Y;
}

Byte NES_CPU::getS() const {
	return S;
}

Byte NES_CPU::getP() const {
	return P;
}

Word NES_CPU::getPC() const {
	return PC;
}

unsigned long NES_CPU::getTotalCycleCount() const {
	return totalCycleCount;
}

void NES_CPU::setA(Byte A) {
	this->A = A;
}

void NES_CPU::setX(Byte X) {
	this->X = X;
}

void NES_CPU::setY(Byte Y) {
	this->Y = Y;
}

void NES_CPU::setS(Byte S) {
	this->S = S;
}

void NES_CPU::setP(Byte P) {
	this->P = P;
}

void NES_CPU::setPC(Word PC) {
	this->PC = PC;
}

void NES_CPU::incrementPCBy(Word increment) {
	this->PC += increment;
}

void NES_CPU::pushStack1Byte(Byte data) {
	memory[0x0100 + S] = data;
	S--;
}

void NES_CPU::pushStack2Byte(Word data) {
	memory[0x0100 + S - 1] = data % 256;
	memory[0x0100 + S] = data >> 8;
	S -= 2;
}


Byte NES_CPU::pullStack1Byte() {
	S++;
	return memory[0x0100 + S];
}

Word NES_CPU::pullStack2Byte() {
	S += 2;
	return memory[0x0100 + S - 1] + (memory[0x0100 + S] << 8);
}

void NES_CPU::setCycleCount(int c) {
	cycleCount = c;
}

int NES_CPU::getCycleCount() const {
	return cycleCount;
}


void NES_CPU::setCarry() {this->P |= CARRY;}
void NES_CPU::clearCarry() { this->P &= ~CARRY;}
bool NES_CPU::isCarrySet() const { return this->P & CARRY;} // True if bit is set, false if not

void NES_CPU::setZero() {this->P |= ZERO;}
void NES_CPU::clearZero() {this->P &= ~ZERO; }
bool NES_CPU::isZeroSet() const { return this->P & ZERO;}

void NES_CPU::setInterruptDisable() { this->P |= INTERRUPT_DISABLE; }
void NES_CPU::clearInterruptDisable() { this->P &= ~INTERRUPT_DISABLE;}
bool NES_CPU::isInterruptDisableSet() const { return this->P & INTERRUPT_DISABLE; }

void NES_CPU::setDecimal() { this->P |= DECIMAL; }
void NES_CPU::clearDecimal() { this->P &= ~DECIMAL;}
bool NES_CPU::isDecimalSet() const { return this->P & DECIMAL; }

void NES_CPU::setBreak() { this->P |= BREAK; }
void NES_CPU::clearBreak() { this->P &= ~BREAK; }
bool NES_CPU::isBreakSet() const { return this->P & BREAK; }

void NES_CPU::setOverflow() { this->P |= OVRFLOW; }
void NES_CPU::clearOverflow() { this->P &= ~OVRFLOW; }
bool NES_CPU::isOverflowSet() const {
	return this->P & OVRFLOW;
}

void NES_CPU::setNegative() { this->P |= NEGATIVE; }
void NES_CPU::clearNegative() { this->P &= ~NEGATIVE; }
bool NES_CPU::isNegativeSet() const { return this->P & NEGATIVE; }

bool NES_CPU::wasPageBoundaryCrossedOnPeek() const {
	return pageBoundaryCrossedOnPeek;
}

void NES_CPU::setNMI() {
	NMI = true;
}

