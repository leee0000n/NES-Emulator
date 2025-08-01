// ReSharper disable All
#include "NES_CPU.h"
#include "Decode.h"

#include <iostream>
#include <string>

NES_CPU* nes_cpu;

// $0000 - $00ff zero page
// $0100 - $01ff stack 
// Need $fffa – $ffff.These addresses contain the addresses of the reset routine and the IRQ and NMI ISR(interrupt service routine).

void NES_CPU::power_up() {
	A = 0;
	X = 0;
	Y = 0;
	P = 0x34; // Interrupt request (IRQ) disabled
	S = 0xFD;
	memory[0x4017] = 0x00; // Frame irq enabled
	memory[0x4015] = 0x00; // All channels disabled

	for (int address = 0x4000; address <= 0x400F; address++) { // $4000-$400F = $00
		memory[address] = 0x00;
	}

	for (int address = 0x4010; address <= 0x4013; address++) { // $4010-$4013 = $00 ( Eliminator Boat Duel )
		memory[address] = 0x00;
	}

	decode::initFuncArray();
}

void NES_CPU::reset() {
	S -= 3;
	P |= 0x04;
}

void NES_CPU::run() {
	power_up();

	while (true) {
		if (cycleCount <= 0) {
			
			Byte opcode = memory[PC];
			Word address = PC;
			
			// BREAK OPCODE
			if (opcode == 0) break;

			// Run instruction at PC
			decode::opcodeFuncPointers[opcode](opcode, address);

			cycleCount--;
		}
		else cycleCount--;
	}
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

//void NES_CPU::printRegisters() {
//	std::cout << "A: " << A << "\n";
//	std::cout << "P: " << P << "\n";
//	std::cout << "X: " << X << "\n";
//	std::cout << "Y: " << Y << "\n";
//	std::cout << "S: " << S << "\n";
//	std::cout << "PC: " << PC << "\n";
//}


Byte NES_CPU::peek(Word address) {
	return memory[address];
}

Byte NES_CPU::immediatePeek(Word address) {
	return memory[address + 1];
}

Byte NES_CPU::zeroPagePeek(Word address) {
	Word zeropageaddress = memory[address + 1];
	return memory[zeropageaddress];
}

Byte NES_CPU::zeroPageXPeek(Word address) {
	Word zeropageXaddress = memory[address + 1] + X;
	return memory[zeropageXaddress];
}

Byte NES_CPU::zeroPageYPeek(Word address) {
	Word zeropageYaddress = memory[address + 1] + Y;
	return memory[zeropageYaddress];
}

Byte NES_CPU::absolutePeek(Word address) {
	Word absoluteaddress = memory[address + 1] + (memory[address + 2] << 8);
	return memory[absoluteaddress];
}

Byte NES_CPU::absoluteXPeek(Word address) {
	Word absoluteXaddress = X + memory[address + 1] + (memory[address + 2] << 8);
	return memory[absoluteXaddress];
}

Byte NES_CPU::absoluteYPeek(Word address) {
	Word absoluteYaddress = Y + memory[address + 1] + (memory[address + 2] << 8);
	return memory[absoluteYaddress];
}

Byte NES_CPU::indirectXPeek(Word address) {
	Word indirectXPeek = memory[address + 1] + X;
	Word lookupaddress = memory[indirectXPeek] + (memory[indirectXPeek + 1] << 8);
	return memory[lookupaddress];
}

Byte NES_CPU::indirectYPeek(Word address) {
	Word indirectYPeek = memory[address + 1];
	Word lookupaddress = Y + memory[indirectYPeek] + (memory[indirectYPeek + 1] << 8);
	return memory[lookupaddress];
}

void NES_CPU::zeroPageSet(Word address, Byte data) {
	Word zeropageaddress = memory[address + 1];
	memory[zeropageaddress] = data;
}

void NES_CPU::zeroPageXSet(Word address, Byte data) {
	Word zeropageXaddress = memory[address + 1] + X;
	memory[zeropageXaddress] = data;
}

void NES_CPU::zeroPageYSet(Word address, Byte data) {
	Word zeropageYaddress = memory[address + 1] + Y;
	memory[zeropageYaddress] = data;
}

void NES_CPU::absoluteSet(Word address, Byte data) {
	Word absoluteaddress = memory[address + 1] + (memory[address + 2] << 8);
	memory[absoluteaddress] = data;
}

void NES_CPU::absoluteXSet(Word address, Byte data) {
	Word absoluteXaddress = X + memory[address + 1] + (memory[address + 2] << 8);
	memory[absoluteXaddress] = data;
}

void NES_CPU::absoluteYSet(Word address, Byte data) {
	Word absoluteYaddress = Y + memory[address + 1] + (memory[address + 2] << 8);
	memory[absoluteYaddress] = data;
}

void NES_CPU::indirectXSet(Word address, Byte data) {
	Word indirectXSet = memory[address + 1] + X;
	Word lookupaddress = memory[indirectXSet] + (memory[indirectXSet + 1] << 8);
	memory[lookupaddress] = data;
}

void NES_CPU::indirectYSet(Word address, Byte data) {
	Word indirectXSet = memory[address + 1];
	Word lookupaddress = Y + memory[indirectXSet] + (memory[indirectXSet + 1] << 8);
	memory[lookupaddress] = data;
}

void NES_CPU::set(Word address, Byte data) {
	memory[address] = data;
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

void NES_CPU::pushStack(Byte data) {
	memory[0x0100 + S] = data;
	S--;
}

Byte NES_CPU::pullStack() {
	S++;
	return memory[0x0100 + S];
	
}

void NES_CPU::setCycleCount(int cycleCount) {
	this->cycleCount = cycleCount;
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

void NES_CPU::setOverflow() { this->P |= OVERFLOW; }
void NES_CPU::clearOverflow() { this->P &= ~OVERFLOW; }
bool NES_CPU::isOverflowSet() const { return this->P & OVERFLOW; }

void NES_CPU::setNegative() { this->P |= NEGATIVE; }
void NES_CPU::clearNegative() { this->P &= ~NEGATIVE; }
bool NES_CPU::isNegativeSet() const { return this->P & NEGATIVE; }