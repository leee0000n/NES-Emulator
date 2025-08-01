// ReSharper disable CppInconsistentNaming
#include "Decode.h"

// Function pointer array
void (*decode::opcodeFuncPointers[256])(Byte, Word);

/// <summary>
/// Add memory value and carry to accumulator
/// </summary>
/// <param name="opcode"> Opcode byte</param>
/// <param name="address"> Opcode address</param>
void ADC(Byte opcode, Word address) {
	Byte data = 0;

	// Set data to correct data depending on address mode
	switch (opcode) {
	// Immediate, 2 bytes, 2 cycles
	case 0x69:
		data = nes_cpu->immediatePeek(address);
		nes_cpu->setCycleCount(2);
		nes_cpu->incrementPCBy(2);
		break;
	// Zero page, 2 bytes, 3 cycles
	case 0x65:
		data = nes_cpu->zeroPagePeek(address);
		nes_cpu->setCycleCount(3);
		nes_cpu->incrementPCBy(2);
		break;
	// Zero page,X , 2 bytes, 4 cycles
	case 0x75:
		data = nes_cpu->zeroPageXPeek(address);
		nes_cpu->setCycleCount(4);
		nes_cpu->incrementPCBy(2);
		break;
	// Absolute, 3 bytes, 4 cycles
	case 0x6d:
		data = nes_cpu->absolutePeek(address);
		nes_cpu->setCycleCount(4);
		nes_cpu->incrementPCBy(3);
		break;
	// Absolute,X , 3 bytes, 4 cycles (5 if page crossed)
	case 0x7d:
		data = nes_cpu->absoluteXPeek(address);
		nes_cpu->setCycleCount(2);
		nes_cpu->incrementPCBy(3);
		break;
	// Absolute,Y 3 bytes, 4 cycles (5 if page crossed)
	case 0x79:
		data = nes_cpu->absoluteYPeek(address);
		nes_cpu->setCycleCount(2);
		nes_cpu->incrementPCBy(3);
		break;
	// Indirect,X , 2 bytes, 6 cycles
	case 0x61:
		data = nes_cpu->indirectXPeek(address);
		nes_cpu->setCycleCount(6);
		nes_cpu->incrementPCBy(2);
		break;
	// Indirect, Y 2 bytes, 5 cycles (6 if page crossed)
	case 0x71:
		data = nes_cpu->indirectYPeek(address);
		nes_cpu->setCycleCount(5);
		nes_cpu->incrementPCBy(2);
		break;
	default:
		break;
	}

	// Result of add
	int result = nes_cpu->getA() + data + (nes_cpu->getP() & 1);

	// Set zero flag to 0
	nes_cpu->clearZero();

	// If there is an overflow, set carry bit
	if (result > 0xFF) { nes_cpu->setCarry(); }
	// If result is zero, set zero bit
	else if (result == 0) { nes_cpu->setZero(); }
	// If result's sign is different from A's and memory's , signed overflow occured
	if ((result ^ nes_cpu->getA()) & (result ^ data) & 0x80) { nes_cpu->setOverflow(); }
	// Directly copy MSB into N flag
	nes_cpu->setP((nes_cpu->getP() & 0x7f) | (result & NEGATIVE));

	nes_cpu->setA(result);
}

/// <summary>
/// AND memory with accumulator value
/// </summary>
/// <param name="opcode"> Opcode byte</param>
/// <param name="address"> Opcode address</param>
void AND(Byte opcode, Word address) {
	Byte data = 0;

	// Set data to correct data depending on address mode
	switch (opcode) {
	// Immediate, 2 bytes, 2 cycles
	case 0x29:
		data = nes_cpu->immediatePeek(address);
		nes_cpu->setCycleCount(2);
		nes_cpu->incrementPCBy(2);
		break;
	// Zero page, 2 bytes, 3 cycles
	case 0x25:
		data = nes_cpu->zeroPagePeek(address);
		nes_cpu->setCycleCount(3);
		nes_cpu->incrementPCBy(2);
		break;
	// Zero page,X , 2 bytes, 4 cycles
	case 0x35:
		data = nes_cpu->zeroPageXPeek(address);
		nes_cpu->setCycleCount(4);
		nes_cpu->incrementPCBy(2);
		break;
	// Absolute, 3 bytes, 4 cycles
	case 0x2d:
		data = nes_cpu->absolutePeek(address);
		nes_cpu->setCycleCount(4);
		nes_cpu->incrementPCBy(3);
		break;
	// Absolute,X , 3 bytes, 4 cycles (5 if page crossed)
	case 0x3d:
		data = nes_cpu->absoluteXPeek(address);
		nes_cpu->setCycleCount(4);
		nes_cpu->incrementPCBy(3);
		break;
	// Absolute,Y 3 bytes, 4 cycles (5 if page crossed)
	case 0x39:
		data = nes_cpu->absoluteYPeek(address);
		nes_cpu->setCycleCount(4);
		nes_cpu->incrementPCBy(3);
		break;
	// Indirect,X , 2 bytes, 6 cycles
	case 0x21:
		data = nes_cpu->indirectXPeek(address);
		nes_cpu->setCycleCount(6);
		nes_cpu->incrementPCBy(2);
		break;
	// Indirect, Y 2 bytes, 5 cycles (6 if page crossed)
	case 0x31:
		data = nes_cpu->indirectYPeek(address);
		nes_cpu->setCycleCount(5);
		nes_cpu->incrementPCBy(2);
		break;
	default:
		break;
	}

	int result = nes_cpu->getA() & data;

	// Set zero flag to 0
	nes_cpu->clearZero();

	// If result is zero, set zero bit
	if (result == 0) { nes_cpu->setZero(); }
	// Directly copy MSB into N flag
	nes_cpu->setP((nes_cpu->getP() & 0x7f) | (result & NEGATIVE));

	nes_cpu->setA(data);
}

/// <summary>
/// Arithmetic shift left accumulator value or a value in memory
/// </summary>
/// <param name="opcode"> Opcode byte</param>
/// <param name="address"> Opcode address</param>
void ASL(Byte opcode, Word address) {
	Byte data = 0;

	// Set data to correct data depending on address mode
	switch (opcode) {
	// Accumulator 1 byte 2 cycles
	case 0x0a:
		// Carry flag if ASL with MSB set to 1
		if (nes_cpu->getA() & 0x80) nes_cpu->setCarry();
		nes_cpu->setA(nes_cpu->getA() << 1);
		nes_cpu->setCycleCount(1);
		nes_cpu->incrementPCBy(1);
		return; // Return as function completed
	// Zero page, 2 bytes, 3 cycles
	case 0x06:
		data = nes_cpu->zeroPagePeek(address);
		nes_cpu->setCycleCount(2);
		nes_cpu->incrementPCBy(2);
		break;
	// Zero page,X , 2 bytes, 4 cycles
	case 0x16:
		data = nes_cpu->zeroPageXPeek(address);
		nes_cpu->setCycleCount(2);
		nes_cpu->incrementPCBy(2);
		break;
	// Absolute, 3 bytes, 4 cycles
	case 0x0e:
		data = nes_cpu->absolutePeek(address);
		nes_cpu->setCycleCount(3);
		nes_cpu->incrementPCBy(3);
		break;
	// Absolute,X , 3 bytes, 4 cycles (5 if page crossed)
	case 0x1e:
		data = nes_cpu->absoluteXPeek(address);
		nes_cpu->setCycleCount(3);
		nes_cpu->incrementPCBy(3);
		break;
	default:
		break;
	}

	// Shift bit 7 into carry flag
	nes_cpu->setP((nes_cpu->getP() & 0xfe) | (data & NEGATIVE));

	int result = data << 1;

	// Set zero flag to 0
	nes_cpu->clearZero();

	// If there is an overflow, set carry bit
	if (result > 0xFF) { nes_cpu->setCarry(); }
	// If result is zero, set zero bit
	else if (result == 0) { nes_cpu->setZero(); }
	// Directly copy MSB into N flag
	nes_cpu->setP((nes_cpu->getP() & 0x7f) | (result & NEGATIVE));

	// Set memory to new value
	switch (opcode) {
	// Zero page, 2 bytes, 3 cycles
	case 0x06:
		nes_cpu->zeroPageSet(address, result);
		break;
	// Zero page,X , 2 bytes, 4 cycles
	case 0x16:
		nes_cpu->zeroPageXSet(address, result);
		break;
	// Absolute, 3 bytes, 4 cycles
	case 0x0e:
		nes_cpu->absoluteSet(address, result);
		break;
	// Absolute,X , 3 bytes, 4 cycles (5 if page crossed)
	case 0x1e:
		nes_cpu->absoluteXSet(address, result);
		break;
	default:
		break;
	}
}

/// <summary>
/// Branch if carry is clear
/// </summary>
/// <param name="opcode"> Opcode byte</param>
/// <param name="address"> Opcode address</param>
void BCC(Byte opcode, Word address) {
	Byte data = nes_cpu->immediatePeek(address);
	nes_cpu->setCycleCount(2);
	nes_cpu->incrementPCBy(2);

	// If carry is clear
	if (!nes_cpu->isCarrySet()) {
		nes_cpu->incrementPCBy(static_cast<signed char>(data));
		nes_cpu->setCycleCount(3);
	}
}

/// <summary>
/// Branch if carry is set
/// </summary>
/// <param name="opcode"> Opcode byte</param>
/// <param name="address"> Opcode address</param>
void BCS(Byte opcode, Word address) {
	Byte data = nes_cpu->immediatePeek(address);
	nes_cpu->setCycleCount(2);
	nes_cpu->incrementPCBy(2);

	// If carry is set
	if (nes_cpu->isCarrySet()) {
		nes_cpu->incrementPCBy(static_cast<signed char>(data));
		nes_cpu->setCycleCount(3);
	}
}

/// <summary>
/// Branch if zero is set
/// </summary>
/// <param name="opcode"> Opcode byte</param>
/// <param name="address"> Opcode address</param>
void BEQ(Byte opcode, Word address) {
	Byte data = nes_cpu->immediatePeek(address);
	nes_cpu->setCycleCount(2);
	nes_cpu->incrementPCBy(2);

	// If zero is set
	if (nes_cpu->isZeroSet()) {
		nes_cpu->incrementPCBy(static_cast<signed char>(data));
		nes_cpu->setCycleCount(3);
	}
}

/// <summary>
/// Compare memory with accumulator value
/// </summary>
/// <param name="opcode"> Opcode byte</param>
/// <param name="address"> Opcode address</param>
void BIT(Byte opcode, Word address) {
	Byte data = 0;

	switch (opcode) {
	case 0x24:
		data = nes_cpu->zeroPagePeek(address);
		nes_cpu->setCycleCount(2);
		nes_cpu->incrementPCBy(2);
		break;
	case 0x2c:
		data = nes_cpu->absolutePeek(address);
		nes_cpu->setCycleCount(3);
		nes_cpu->incrementPCBy(3);
		break;
	default:
		break;
	}

	int result = nes_cpu->getA() & data;

	// Set zero flag to 0
	nes_cpu->clearZero();

	// If result is zero, set zero bit
	if (result == ZERO) { nes_cpu->setP((nes_cpu->getP() & 0xfd) | 2); }
	// Directly copy bit 6 into V flag
	nes_cpu->setP((nes_cpu->getP() & 0xbf) | (result & OVRFLOW));

	// Directly copy MSB into N flag
	nes_cpu->setP((nes_cpu->getP() & 0x7f) | (result & NEGATIVE));
}


/// <summary>
/// Branch if negative is set
/// </summary>
/// <param name="opcode"> Opcode byte</param>
/// <param name="address"> Opcode address</param>
void BMI(Byte opcode, Word address) {
	Byte data = nes_cpu->immediatePeek(address);
	nes_cpu->setCycleCount(2);
	nes_cpu->incrementPCBy(2);

	// If negative is set
	if (nes_cpu->isNegativeSet()) {
		nes_cpu->incrementPCBy(static_cast<signed char>(data));
		nes_cpu->setCycleCount(3);
	}
}

/// <summary>
/// Branch if zero is clear
/// </summary>
/// <param name="opcode"> Opcode byte</param>
/// <param name="address"> Opcode address</param>
void BNE(Byte opcode, Word address) {
	Byte data = nes_cpu->immediatePeek(address);
	nes_cpu->setCycleCount(2);
	nes_cpu->incrementPCBy(2);

	// If zero is clear
	if (!(nes_cpu->isZeroSet())) {
		nes_cpu->incrementPCBy(static_cast<signed char>(data));
		nes_cpu->setCycleCount(3);
	}
}

/// <summary>
/// Branch if negative is clear
/// </summary>
/// <param name="opcode"> Opcode byte</param>
/// <param name="address"> Opcode address</param>
void BPL(Byte opcode, Word address) {
	Byte data = nes_cpu->immediatePeek(address);
	nes_cpu->setCycleCount(2);
	nes_cpu->incrementPCBy(2);

	// If negative is clear
	if (!(nes_cpu->isNegativeSet())) {
		nes_cpu->incrementPCBy(static_cast<signed char>(data));
		nes_cpu->setCycleCount(3);
	}
}

void BRK(Byte opcode, Word address) {
	// TODO
}

/// <summary>
/// Branch if overflow is clear
/// </summary>
/// <param name="opcode"> Opcode byte</param>
/// <param name="address"> Opcode address</param>
void BVC(Byte opcode, Word address) {
	Byte data = nes_cpu->immediatePeek(address);
	nes_cpu->setCycleCount(2);
	nes_cpu->incrementPCBy(2);

	if (!nes_cpu->isOverflowSet()) {
		nes_cpu->incrementPCBy(static_cast<signed char>(data));
		nes_cpu->setCycleCount(3);
	}
}

/// <summary>
/// Branch if overflow set
/// </summary>
/// <param name="opcode"> Opcode byte</param>
/// <param name="address"> Opcode address</param>
void BVS(Byte opcode, Word address) {
	Byte data = nes_cpu->immediatePeek(address);
	nes_cpu->setCycleCount(2);
	nes_cpu->incrementPCBy(2);

	if (nes_cpu->isOverflowSet()) {
		nes_cpu->incrementPCBy(static_cast<signed char>(data));
		nes_cpu->setCycleCount(3);
	}
}

/// <summary>
/// Clear carry
/// </summary>
/// <param name="opcode"> Opcode byte</param>
/// <param name="address"> Opcode address</param>
void CLC(Byte opcode, Word address) {
	nes_cpu->clearCarry();
	nes_cpu->incrementPCBy(1);
	nes_cpu->setCycleCount(2);
}

/// <summary>
/// Clear decimal
/// </summary>
/// <param name="opcode"> Opcode byte</param>
/// <param name="address"> Opcode address</param>
void CLD(Byte opcode, Word address) {
	nes_cpu->clearDecimal();
	nes_cpu->setCycleCount(2);
	nes_cpu->incrementPCBy(1);
}

/// <summary>
/// Clear interrupt disable
/// </summary>
/// <param name="opcode"> Opcode byte</param>
/// <param name="address"> Opcode address</param>
void CLI(Byte opcode, Word address) {
	nes_cpu->clearInterruptDisable();
	nes_cpu->setCycleCount(2);
	nes_cpu->incrementPCBy(1);
}

/// <summary>
/// Clear overflow
/// </summary>
/// <param name="opcode"> Opcode byte</param>
/// <param name="address"> Opcode address</param>
void CLV(Byte opcode, Word address) {
	nes_cpu->clearOverflow();
	nes_cpu->setCycleCount(2);
	nes_cpu->incrementPCBy(1);
}

/// <summary>
/// Compare value in the accumulator with value in memory and set flags accordingly
/// </summary>
/// <param name="opcode"></param>
/// <param name="address"></param>
void CMP(Byte opcode, Word address) {
	Byte data = 0;

	// Set data to correct data depending on address mode
	switch (opcode) {
	// Immediate, 2 bytes, 2 cycles
	case 0xc9:
		data = nes_cpu->immediatePeek(address);
		nes_cpu->setCycleCount(2);
		nes_cpu->incrementPCBy(2);
		break;
	// Zero page, 2 bytes, 3 cycles
	case 0xc5:
		data = nes_cpu->zeroPagePeek(address);
		nes_cpu->setCycleCount(3);
		nes_cpu->incrementPCBy(2);
		break;
	// Zero page,X , 2 bytes, 4 cycles
	case 0xd5:
		data = nes_cpu->zeroPageXPeek(address);
		nes_cpu->setCycleCount(4);
		nes_cpu->incrementPCBy(2);
		break;
	// Absolute, 3 bytes, 4 cycles
	case 0xcd:
		data = nes_cpu->absolutePeek(address);
		nes_cpu->setCycleCount(4);
		nes_cpu->incrementPCBy(3);
		break;
	// Absolute,X , 3 bytes, 4 cycles (5 if page crossed)
	case 0xdd:
		data = nes_cpu->absoluteXPeek(address);
		nes_cpu->setCycleCount(4);
		nes_cpu->incrementPCBy(3);
		break;
	// Absolute,Y 3 bytes, 4 cycles (5 if page crossed)
	case 0xd9:
		data = nes_cpu->absoluteYPeek(address);
		nes_cpu->setCycleCount(4);
		nes_cpu->incrementPCBy(3);
		break;
	// Indirect,X , 2 bytes, 6 cycles
	case 0xc1:
		data = nes_cpu->indirectXPeek(address);
		nes_cpu->setCycleCount(6);
		nes_cpu->incrementPCBy(2);
		break;
	// Indirect, Y 2 bytes, 5 cycles (6 if page crossed)
	case 0xd1:
		data = nes_cpu->indirectYPeek(address);
		nes_cpu->setCycleCount(5);
		nes_cpu->incrementPCBy(2);
		break;
	default:
		break;
	}

	// Set zero flag to 0
	nes_cpu->clearZero();

	// Set carry flag if no borrow occurs
	if (nes_cpu->getA() >= data) nes_cpu->setCarry();
		// Set zero flag if result is 0
	else if (nes_cpu->getA() == data) nes_cpu->setZero();
	else nes_cpu->clearCarry();
	int result = nes_cpu->getA() - data;
	// Directly copy MSB into N flag
	nes_cpu->setP((nes_cpu->getP() & 0x7f) | (result & NEGATIVE));
}

/// <summary>
/// Compare value in X register with value in memory and set flags accordingly
/// </summary>
/// <param name="opcode"></param>
/// <param name="address"></param>
void CPX(Byte opcode, Word address) {
	Byte data = 0;

	// Set data to correct data depending on address mode
	switch (opcode) {
	// Immediate, 2 bytes, 2 cycles
	case 0xe0:
		data = nes_cpu->immediatePeek(address);
		nes_cpu->setCycleCount(2);
		nes_cpu->incrementPCBy(2);
		break;
	// Zero page, 2 bytes, 3 cycles
	case 0xe4:
		data = nes_cpu->zeroPagePeek(address);
		nes_cpu->setCycleCount(3);
		nes_cpu->incrementPCBy(2);
		break;
	// Absolute, 3 bytes, 4 cycles
	case 0xec:
		data = nes_cpu->absolutePeek(address);
		nes_cpu->setCycleCount(4);
		nes_cpu->incrementPCBy(3);
		break;
	default:
		break;
	}

	// Set zero flag to 0
	nes_cpu->clearZero();

	// Set carry flag if no borrow occurs
	if (nes_cpu->getX() >= data) nes_cpu->setCarry();
		// Set zero flag if result is 0
	else if (nes_cpu->getX() == data) nes_cpu->setZero();
	int result = nes_cpu->getX() - data;
	// Directly copy MSB into N flag
	nes_cpu->setP((nes_cpu->getP() & 0x7f) | (result & NEGATIVE));
}

/// <summary>
/// Compare value in Y register with value in memory and set flags accordingly
/// </summary>
/// <param name="opcode"></param>
/// <param name="address"></param>
void CPY(Byte opcode, Word address) {
	Byte data = 0;

	// Set data to correct data depending on address mode
	switch (opcode) {
	// Immediate, 2 bytes, 2 cycles
	case 0xc0:
		data = nes_cpu->immediatePeek(address);
		nes_cpu->setCycleCount(2);
		nes_cpu->incrementPCBy(2);
		break;
	// Zero page, 2 bytes, 3 cycles
	case 0xc4:
		data = nes_cpu->zeroPagePeek(address);
		nes_cpu->setCycleCount(3);
		nes_cpu->incrementPCBy(2);
		break;
	// Absolute, 3 bytes, 4 cycles
	case 0xcc:
		data = nes_cpu->absolutePeek(address);
		nes_cpu->setCycleCount(4);
		nes_cpu->incrementPCBy(3);
		break;
	default:
		break;
	}

	// Set zero flag to 0
	nes_cpu->clearZero();

	// Set carry flag if no borrow occurs
	if (nes_cpu->getY() >= data) nes_cpu->setCarry();
		// Set zero flag if result is 0
	else if (nes_cpu->getY() == data) nes_cpu->setZero();
	int result = nes_cpu->getY() - data;
	// Directly copy MSB into N flag
	nes_cpu->setP((nes_cpu->getP() & 0x7f) | (result & NEGATIVE));
}

void DEC(Byte opcode, Word address) {
	Byte data = 0;

	// Set data to correct data depending on address mode
	switch (opcode) {
	// Zero page, 2 bytes, 3 cycles
	case 0x65:
		data = nes_cpu->zeroPagePeek(address);
		nes_cpu->setCycleCount(2);
		nes_cpu->incrementPCBy(2);
		break;
	// Zero page,X , 2 bytes, 4 cycles
	case 0x75:
		data = nes_cpu->zeroPageXPeek(address);
		nes_cpu->setCycleCount(2);
		nes_cpu->incrementPCBy(2);
		break;
	// Absolute, 3 bytes, 4 cycles
	case 0x6d:
		data = nes_cpu->absolutePeek(address);
		nes_cpu->setCycleCount(3);
		nes_cpu->incrementPCBy(2);
		break;
	// Absolute,X , 3 bytes, 4 cycles (5 if page crossed)
	case 0x7d:
		data = nes_cpu->absoluteXPeek(address);
		nes_cpu->setCycleCount(3);
		nes_cpu->incrementPCBy(3);
		break;
	default:
		break;
	}

	int result = data - 1;

	// Set zero flag to 0
	nes_cpu->clearZero();

	// If result is zero, set zero bit
	if (result == 0) { nes_cpu->setZero(); }
	// Directly copy MSB into N flag
	nes_cpu->setP((nes_cpu->getP() & 0x7f) | (result & NEGATIVE));

	// Set memory to result
	switch (opcode) {
	case 0x65:
		nes_cpu->zeroPageSet(address, result);
		break;
	case 0x75:
		nes_cpu->zeroPageXSet(address, result);
		break;
	case 0x6d:
		nes_cpu->absoluteSet(address, result);
		break;
	case 0x7d:
		nes_cpu->absoluteXSet(address, result);
		break;
	default:
		break;
	}
}

void DEX(Byte opcode, Word address) {
	int result = nes_cpu->getX() - 1;
	nes_cpu->setCycleCount(2);
	nes_cpu->incrementPCBy(1);

	// Set zero flag to 0
	nes_cpu->clearZero();

	// If result is zero, set zero bit
	if (result == 0) { nes_cpu->setZero(); }
	// Directly copy MSB into N flag
	nes_cpu->setP((nes_cpu->getP() & 0x7f) | (result & NEGATIVE));

	nes_cpu->setX(result);
}

void DEY(Byte opcode, Word address) {
	int result = nes_cpu->getY() - 1;
	nes_cpu->setCycleCount(2);
	nes_cpu->incrementPCBy(1);

	// Set zero flag to 0
	nes_cpu->clearZero();

	// If result is zero, set zero bit
	if (result == 0) { nes_cpu->setZero(); }
	// Directly copy MSB into N flag
	nes_cpu->setP((nes_cpu->getP() & 0x7f) | (result & NEGATIVE));

	nes_cpu->setY(result);
}

void EOR(Byte opcode, Word address) {
	Byte data = 0;

	// Set data to correct data depending on address mode
	switch (opcode) {
	// Immediate, 2 bytes, 2 cycles
	case 0x49:
		data = nes_cpu->immediatePeek(address);
		nes_cpu->setCycleCount(2);
		nes_cpu->incrementPCBy(2);
		break;
	// Zero page, 2 bytes, 3 cycles
	case 0x45:
		data = nes_cpu->zeroPagePeek(address);
		nes_cpu->setCycleCount(3);
		nes_cpu->incrementPCBy(2);
		break;
	// Zero page,X , 2 bytes, 4 cycles
	case 0x55:
		data = nes_cpu->zeroPageXPeek(address);
		nes_cpu->setCycleCount(4);
		nes_cpu->incrementPCBy(2);
		break;
	// Absolute, 3 bytes, 4 cycles
	case 0x4d:
		data = nes_cpu->absolutePeek(address);
		nes_cpu->setCycleCount(4);
		nes_cpu->incrementPCBy(3);
		break;
	// Absolute,X , 3 bytes, 4 cycles (5 if page crossed)
	case 0x5d:
		data = nes_cpu->absoluteXPeek(address);
		nes_cpu->setCycleCount(4);
		nes_cpu->incrementPCBy(3);
		break;
	// Absolute,Y 3 bytes, 4 cycles (5 if page crossed)
	case 0x59:
		data = nes_cpu->absoluteYPeek(address);
		nes_cpu->setCycleCount(4);
		nes_cpu->incrementPCBy(3);
		break;
	// Indirect,X , 2 bytes, 6 cycles
	case 0x41:
		data = nes_cpu->indirectXPeek(address);
		nes_cpu->setCycleCount(6);
		nes_cpu->incrementPCBy(2);
		break;
	// Indirect, Y 2 bytes, 5 cycles (6 if page crossed)
	case 0x51:
		data = nes_cpu->indirectYPeek(address);
		nes_cpu->setCycleCount(5);
		nes_cpu->incrementPCBy(2);
		break;
	default:
		break;
	}

	int result = nes_cpu->getA() ^ data;

	// Set zero flag to 0
	nes_cpu->clearZero();

	// If result is zero, set zero bit
	if (result == 0) { nes_cpu->setZero(); }
	// Directly copy MSB into N flag
	nes_cpu->setP((nes_cpu->getP() & 0x7f) | (result & NEGATIVE));

	nes_cpu->setA(data);
}

void INC(Byte opcode, Word address) {
	Byte data = 0;

	// Set data to correct data depending on address mode
	switch (opcode) {
	// Zero page, 2 bytes, 3 cycles
	case 0x65:
		data = nes_cpu->zeroPagePeek(address);
		nes_cpu->setCycleCount(2);
		nes_cpu->incrementPCBy(2);
		break;
	// Zero page,X , 2 bytes, 4 cycles
	case 0x75:
		data = nes_cpu->zeroPageXPeek(address);
		nes_cpu->setCycleCount(2);
		nes_cpu->incrementPCBy(2);
		break;
	// Absolute, 3 bytes, 4 cycles
	case 0x6d:
		data = nes_cpu->absolutePeek(address);
		nes_cpu->setCycleCount(3);
		nes_cpu->incrementPCBy(3);
		break;
	// Absolute,X , 3 bytes, 4 cycles (5 if page crossed)
	case 0x7d:
		data = nes_cpu->absoluteXPeek(address);
		nes_cpu->setCycleCount(3);
		nes_cpu->incrementPCBy(3);
		break;
	default:
		break;
	}

	int result = data + 1;

	// Set zero flag to 0
	nes_cpu->clearZero();

	// If result is zero, set zero bit
	if (result == 0) { nes_cpu->setZero(); }
	// Directly copy MSB into N flag
	nes_cpu->setP((nes_cpu->getP() & 0x7f) | (result & NEGATIVE));

	// Set memory to result
	switch (opcode) {
	case 0x65:
		nes_cpu->zeroPageSet(address, result);
		break;
	case 0x75:
		nes_cpu->zeroPageXSet(address, result);
		break;
	case 0x6d:
		nes_cpu->absoluteSet(address, result);
		break;
	case 0x7d:
		nes_cpu->absoluteXSet(address, result);
		break;
	default:
		break;
	}
}

void INX(Byte opcode, Word address) {
	int result = nes_cpu->getX() + 1;
	nes_cpu->setCycleCount(2);
	nes_cpu->incrementPCBy(1);

	// Set zero flag to 0
	nes_cpu->clearZero();

	// If result is zero, set zero bit
	if (result == 0) { nes_cpu->setZero(); }
	// Directly copy MSB into N flag
	nes_cpu->setP((nes_cpu->getP() & 0x7f) | (result & NEGATIVE));

	nes_cpu->setX(result);
}

void INY(Byte opcode, Word address) {
	int result = nes_cpu->getY() + 1;
	nes_cpu->setCycleCount(2);
	nes_cpu->incrementPCBy(1);

	// Set zero flag to 0
	nes_cpu->clearZero();

	// If result is zero, set zero bit
	if (result == 0) { nes_cpu->setZero(); }
	// Directly copy MSB into N flag
	nes_cpu->setP((nes_cpu->getP() & 0x7f) | (result & NEGATIVE));

	nes_cpu->setY(result);
}

// TODO implement hardware bug present with this instruction
// Read https://www.nesdev.org/wiki/Instruction_reference
void JMP(Byte opcode, Word address) {
	Word data = 0;

	switch (opcode) {
	case 0x4c:
		data = nes_cpu->peek(address + 1) + (nes_cpu->peek(address + 2) << 8);
		nes_cpu->setCycleCount(3);
		nes_cpu->incrementPCBy(3);
		break;
	case 0x6c:
		data = nes_cpu->peek(address + 1) + (nes_cpu->peek(address + 2) << 8);
		data = nes_cpu->peek(data) + (nes_cpu->peek(data + 1) << 8);
		nes_cpu->incrementPCBy(3);
		nes_cpu->setCycleCount(5);
		break;
	default:
		break;
	}

	nes_cpu->setPC(data);
}

void JSR(Byte opcode, Word address) {
	Word data = nes_cpu->peek(address + 1) + (nes_cpu->peek(address + 2) << 8);
	nes_cpu->incrementPCBy(1);

	nes_cpu->pushStack(nes_cpu->getPC() + 2);
	nes_cpu->setPC(data);

	nes_cpu->setCycleCount(6);

	nes_cpu->setPC(data);
}

void LDA(Byte opcode, Word address) {
	Byte data = 0;

	// Set data to correct data depending on address mode
	switch (opcode) {
	// Immediate, 2 bytes, 2 cycles
	case 0xa9:
		data = nes_cpu->immediatePeek(address);
		nes_cpu->setCycleCount(2);
		nes_cpu->incrementPCBy(2);
		break;
	// Zero page, 2 bytes, 3 cycles
	case 0xa5:
		data = nes_cpu->zeroPagePeek(address);
		nes_cpu->setCycleCount(3);
		nes_cpu->incrementPCBy(2);
		break;
	// Zero page,X , 2 bytes, 4 cycles
	case 0xb5:
		data = nes_cpu->zeroPageXPeek(address);
		nes_cpu->setCycleCount(4);
		nes_cpu->incrementPCBy(2);
		break;
	// Absolute, 3 bytes, 4 cycles
	case 0xad:
		data = nes_cpu->absolutePeek(address);
		nes_cpu->setCycleCount(4);
		nes_cpu->incrementPCBy(3);
		break;
	// Absolute,X , 3 bytes, 4 cycles (5 if page crossed)
	case 0xbd:
		data = nes_cpu->absoluteXPeek(address);
		nes_cpu->setCycleCount(4);
		nes_cpu->incrementPCBy(3);
		break;
	// Absolute,Y 3 bytes, 4 cycles (5 if page crossed)
	case 0xb9:
		data = nes_cpu->absoluteYPeek(address);
		nes_cpu->setCycleCount(4);
		nes_cpu->incrementPCBy(3);
		break;
	// Indirect,X , 2 bytes, 6 cycles
	case 0xa1:
		data = nes_cpu->indirectXPeek(address);
		nes_cpu->setCycleCount(6);
		nes_cpu->incrementPCBy(2);
		break;
	// Indirect, Y 2 bytes, 5 cycles (6 if page crossed)
	case 0xb1:
		data = nes_cpu->indirectYPeek(address);
		nes_cpu->setCycleCount(5);
		nes_cpu->incrementPCBy(2);
		break;
	default:
		break;
	}

	// Set zero flag to 0
	nes_cpu->clearZero();

	// If result is zero, set zero bit
	if (data == 0) { nes_cpu->setZero(); }
	// Directly copy MSB into N flag
	nes_cpu->setP((nes_cpu->getP() & 0x7f) | (data & NEGATIVE));

	nes_cpu->setA(data);
}

void LDX(Byte opcode, Word address) {
	Byte data = 0;

	// Set data to correct data depending on address mode
	switch (opcode) {
	// Immediate, 2 bytes, 2 cycles
	case 0xa2:
		data = nes_cpu->immediatePeek(address);
		nes_cpu->setCycleCount(2);
		nes_cpu->incrementPCBy(2);
		break;
	// Zero page, 2 bytes, 3 cycles
	case 0xa6:
		data = nes_cpu->zeroPagePeek(address);
		nes_cpu->setCycleCount(2);
		nes_cpu->incrementPCBy(2);
		break;
	// Zero page,Y , 2 bytes, 4 cycles
	case 0xb6:
		data = nes_cpu->zeroPageYPeek(address);
		nes_cpu->setCycleCount(2);
		nes_cpu->incrementPCBy(2);
		break;
	// Absolute, 3 bytes, 4 cycles
	case 0xaE:
		data = nes_cpu->absolutePeek(address);
		nes_cpu->setCycleCount(3);
		nes_cpu->incrementPCBy(3);
		break;
	// Absolute,Y 3 bytes, 4 cycles (5 if page crossed)
	case 0xbE:
		data = nes_cpu->absoluteYPeek(address);
		nes_cpu->setCycleCount(3);
		nes_cpu->incrementPCBy(3);
		break;
	default:
		break;
	}

	// Set zero flag to 0
	nes_cpu->clearZero();

	// If result is zero, set zero bit
	if (data == 0) { nes_cpu->setZero(); }
	// Directly copy MSB into N flag
	nes_cpu->setP((nes_cpu->getP() & 0x7f) | (data & NEGATIVE));

	nes_cpu->setX(data);
}

void LDY(Byte opcode, Word address) {
	Byte data = 0;

	// Set data to correct data depending on address mode
	switch (opcode) {
	// Immediate, 2 bytes, 2 cycles
	case 0xa0:
		data = nes_cpu->immediatePeek(address);
		nes_cpu->setCycleCount(3);
		nes_cpu->incrementPCBy(2);
		break;
	// Zero page, 2 bytes, 3 cycles
	case 0xa4:
		data = nes_cpu->zeroPagePeek(address);
		nes_cpu->setCycleCount(3);
		nes_cpu->incrementPCBy(2);
		break;
	// Zero page,X , 2 bytes, 4 cycles
	case 0xb4:
		data = nes_cpu->zeroPageXPeek(address);
		nes_cpu->setCycleCount(4);
		nes_cpu->incrementPCBy(2);
		break;
	// Absolute, 3 bytes, 4 cycles
	case 0xac:
		data = nes_cpu->absolutePeek(address);
		nes_cpu->setCycleCount(4);
		nes_cpu->incrementPCBy(3);
		break;
	// Absolute,X , 3 bytes, 4 cycles (5 if page crossed)
	case 0xbc:
		data = nes_cpu->absoluteXPeek(address);
		nes_cpu->setCycleCount(4);
		nes_cpu->incrementPCBy(3);
		break;
	default:
		break;
	}

	// Set zero flag to 0
	nes_cpu->clearZero();

	// If result is zero, set zero bit
	if (data == 0) { nes_cpu->setZero(); }
	// Directly copy MSB into N flag
	nes_cpu->setP((nes_cpu->getP() & 0x7f) | (data & NEGATIVE));

	nes_cpu->setY(data);
}

void LSR(Byte opcode, Word address) {
	Byte data = 0;

	// Set data to correct data depending on address mode
	switch (opcode) {
	// Accumulator 1 byte 2 cycles
	case 0x0a:
		// Carry flag if ASL with MSB set to 1
		if (nes_cpu->getA() & 0x80) nes_cpu->setCarry();
		nes_cpu->setA(nes_cpu->getA() << 1);
		nes_cpu->setCycleCount(2);
		nes_cpu->incrementPCBy(1);
		return; // Return as function completed
	// Zero page, 2 bytes, 3 cycles
	case 0x06:
		data = nes_cpu->zeroPagePeek(address);
		nes_cpu->setCycleCount(5);
		nes_cpu->incrementPCBy(2);
		break;
	// Zero page,X , 2 bytes, 4 cycles
	case 0x16:
		data = nes_cpu->zeroPageXPeek(address);
		nes_cpu->setCycleCount(6);
		nes_cpu->incrementPCBy(2);
		break;
	// Absolute, 3 bytes, 4 cycles
	case 0x0e:
		data = nes_cpu->absolutePeek(address);
		nes_cpu->setCycleCount(6);
		nes_cpu->incrementPCBy(3);
		break;
	// Absolute,X , 3 bytes, 4 cycles (5 if page crossed)
	case 0x1e:
		data = nes_cpu->absoluteXPeek(address);
		nes_cpu->setCycleCount(7);
		nes_cpu->incrementPCBy(3);
		break;
	default:
		break;
	}

	int result = data >> 1;

	// Set zero flag to 0
	nes_cpu->clearZero();

	// Shift bit 0 into carry flag
	nes_cpu->setP((nes_cpu->getP() & 0xfe) | (data & ZERO));
	// If result is zero, set zero bit
	if (result == 0) { nes_cpu->setZero(); }
	// Directly copy MSB into N flag
	nes_cpu->setP((nes_cpu->getP() & 0x7f) | (result & NEGATIVE));

	// Set memory to new value
	switch (opcode) {
	// Zero page, 2 bytes, 3 cycles
	case 0x06:
		nes_cpu->zeroPageSet(address, result);
		break;
	// Zero page,X , 2 bytes, 4 cycles
	case 0x16:
		nes_cpu->zeroPageXSet(address, result);
		break;
	// Absolute, 3 bytes, 4 cycles
	case 0x0e:
		nes_cpu->absoluteSet(address, result);
		break;
	// Absolute,X , 3 bytes, 4 cycles (5 if page crossed)
	case 0x1e:
		nes_cpu->absoluteXSet(address, result);
		break;
	default:
		break;
	}
}

void NOP(Byte opcode, Word address) {
	nes_cpu->setCycleCount(2);
	nes_cpu->incrementPCBy(1);
}

void ORA(Byte opcode, Word address) {
	Byte data = 0;

	// Set data to correct data depending on address mode
	switch (opcode) {
	// Immediate, 2 bytes, 2 cycles
	case 0x09:
		data = nes_cpu->immediatePeek(address);
		nes_cpu->setCycleCount(2);
		nes_cpu->incrementPCBy(2);
		break;
	// Zero page, 2 bytes, 3 cycles
	case 0x05:
		data = nes_cpu->zeroPagePeek(address);
		nes_cpu->setCycleCount(3);
		nes_cpu->incrementPCBy(2);
		break;
	// Zero page,X , 2 bytes, 4 cycles
	case 0x15:
		data = nes_cpu->zeroPageXPeek(address);
		nes_cpu->setCycleCount(4);
		nes_cpu->incrementPCBy(2);
		break;
	// Absolute, 3 bytes, 4 cycles
	case 0x0d:
		data = nes_cpu->absolutePeek(address);
		nes_cpu->setCycleCount(4);
		nes_cpu->incrementPCBy(3);
		break;
	// Absolute,X , 3 bytes, 4 cycles (5 if page crossed)
	case 0x1d:
		data = nes_cpu->absoluteXPeek(address);
		nes_cpu->setCycleCount(4);
		nes_cpu->incrementPCBy(3);
		break;
	// Absolute,Y 3 bytes, 4 cycles (5 if page crossed)
	case 0x19:
		data = nes_cpu->absoluteYPeek(address);
		nes_cpu->setCycleCount(4);
		nes_cpu->incrementPCBy(3);
		break;
	// Indirect,X , 2 bytes, 6 cycles
	case 0x01:
		data = nes_cpu->indirectXPeek(address);
		nes_cpu->setCycleCount(6);
		nes_cpu->incrementPCBy(2);
		break;
	// Indirect, Y 2 bytes, 5 cycles (6 if page crossed)
	case 0x11:
		data = nes_cpu->indirectYPeek(address);
		nes_cpu->setCycleCount(5);
		nes_cpu->incrementPCBy(2);
		break;
	default:
		break;
	}

	Byte result = nes_cpu->getA() | data;

	// Set zero flag to 0
	nes_cpu->clearZero();

	// If result is zero, set zero bit
	if (result == 0) { nes_cpu->setZero(); }
	// Directly copy MSB into N flag
	nes_cpu->setP((nes_cpu->getP() & 0x7f) | (result & NEGATIVE));

	nes_cpu->setA(result);
}

void PHA(Byte opcode, Word address) {
	nes_cpu->pushStack(nes_cpu->getA());
	nes_cpu->setCycleCount(3);
	nes_cpu->incrementPCBy(1);
}

void PHP(Byte opcode, Word address) {
	nes_cpu->pushStack(nes_cpu->getP());
	nes_cpu->setCycleCount(3);
	nes_cpu->incrementPCBy(1);
}

void PLA(Byte opcode, Word address) {
	nes_cpu->setA(nes_cpu->pullStack());
	nes_cpu->setCycleCount(4);
	nes_cpu->incrementPCBy(1);

	// If result is zero, set zero bit
	if (nes_cpu->getA() == 0) { nes_cpu->setZero(); }
	// Directly copy MSB into N flag
	nes_cpu->setP((nes_cpu->getP() & 0x7f) | (nes_cpu->getA() & NEGATIVE));
}

void PLP(Byte opcode, Word address) {
	nes_cpu->setP(nes_cpu->pullStack());
	nes_cpu->setCycleCount(4);
	nes_cpu->incrementPCBy(1);
}

void ROL(Byte opcode, Word address) {
	Byte data = 0;

	// Set data to correct data depending on address mode
	switch (opcode) {
	// Accumulator
	case 0x2a:
		// Carry flag if ASL with MSB set to 1
		if (nes_cpu->getA() & 0x80) nes_cpu->setCarry();
		nes_cpu->setCycleCount(2);
		nes_cpu->setA(nes_cpu->getA() << 1);
		nes_cpu->incrementPCBy(1);
		return; // Return as function completed
	// Zero page
	case 0x26:
		data = nes_cpu->zeroPagePeek(address);
		nes_cpu->setCycleCount(5);
		nes_cpu->incrementPCBy(2);
		break;
	// Zero page,X
	case 0x36:
		data = nes_cpu->zeroPageXPeek(address);
		nes_cpu->setCycleCount(6);
		nes_cpu->incrementPCBy(2);
		break;
	// Absolute
	case 0x2e:
		data = nes_cpu->absolutePeek(address);
		nes_cpu->setCycleCount(6);
		nes_cpu->incrementPCBy(3);
		break;
	// Absolute,X
	case 0x3e:
		data = nes_cpu->absoluteXPeek(address);
		nes_cpu->setCycleCount(7);
		nes_cpu->incrementPCBy(3);
		break;
	default:
		break;
	}


	int temp = nes_cpu->getP();

	// Set zero flag to 0
	nes_cpu->clearZero();

	// Shift bit 7 into carry flag
	nes_cpu->setP((nes_cpu->getP() & 0xfe) | (data & NEGATIVE));

	int result = data << 1;

	// Shift old carry into bit 0
	result |= temp;

	// If there is an overflow, set carry bit
	if (result > 0xFF) { nes_cpu->setCarry(); }
	// If result is zero, set zero bit
	else if (result == 0) { nes_cpu->setZero(); }
	// Directly copy MSB into N flag
	nes_cpu->setP((nes_cpu->getP() & 0x7f) | (result & NEGATIVE));

	// Set memory to new value
	switch (opcode) {
	// Zero page, 2 bytes, 3 cycles
	case 0x26:
		nes_cpu->zeroPageSet(address, result);
		break;
	// Zero page,X , 2 bytes, 4 cycles
	case 0x36:
		nes_cpu->zeroPageXSet(address, result);
		break;
	// Absolute, 3 bytes, 4 cycles
	case 0x2e:
		nes_cpu->absoluteSet(address, result);
		break;
	// Absolute,X , 3 bytes, 4 cycles (5 if page crossed)
	case 0x3e:
		nes_cpu->absoluteXSet(address, result);
		break;
	default:
		break;
	}
}

void ROR(Byte opcode, Word address) {
	Byte data = 0;

	// Set data to correct data depending on address mode
	switch (opcode) {
	// Accumulator 1 byte 2 cycles
	case 0x6a:
		// Carry flag if ASL with MSB set to 1
		if (nes_cpu->getA() & 0x80) nes_cpu->setCarry();
		nes_cpu->setCycleCount(2);
		nes_cpu->setA(nes_cpu->getA() << 1);
		nes_cpu->incrementPCBy(1);
		return; // Return as function completed
	// Zero page, 2 bytes, 3 cycles
	case 0x66:
		data = nes_cpu->zeroPagePeek(address);
		nes_cpu->setCycleCount(5);
		nes_cpu->incrementPCBy(2);
		break;
	// Zero page,X , 2 bytes, 4 cycles
	case 0x76:
		data = nes_cpu->zeroPageXPeek(address);
		nes_cpu->setCycleCount(6);
		nes_cpu->incrementPCBy(2);
		break;
	// Absolute, 3 bytes, 4 cycles
	case 0x6e:
		data = nes_cpu->absolutePeek(address);
		nes_cpu->setCycleCount(6);
		nes_cpu->incrementPCBy(3);
		break;
	// Absolute,X , 3 bytes, 4 cycles (5 if page crossed)
	case 0x7e:
		data = nes_cpu->absoluteXPeek(address);
		nes_cpu->setCycleCount(7);
		nes_cpu->incrementPCBy(3);
		break;
	default:
		break;
	}


	int temp = nes_cpu->getP();

	// Set zero flag to 0
	nes_cpu->clearZero();

	// Shift bit 7 into carry flag
	nes_cpu->setP((nes_cpu->getP() & 0xfe) | (data & NEGATIVE));

	int result = data << 1;

	// Shift old carry into bit 0
	result |= temp;

	// If there is an overflow, set carry bit
	if (result > 0xFF) { nes_cpu->setCarry(); }
	// If result is zero, set zero bit
	else if (result == 0) { nes_cpu->setZero(); }
	// Directly copy MSB into N flag
	nes_cpu->setP((nes_cpu->getP() & 0x7f) | (result & NEGATIVE));

	// Set memory to new value
	switch (opcode) {
	// Zero page, 2 bytes, 3 cycles
	case 0x66:
		nes_cpu->zeroPageSet(address, result);
		break;
	// Zero page,X , 2 bytes, 4 cycles
	case 0x76:
		nes_cpu->zeroPageXSet(address, result);
		break;
	// Absolute, 3 bytes, 4 cycles
	case 0x6e:
		nes_cpu->absoluteSet(address, result);
		break;
	// Absolute,X , 3 bytes, 4 cycles (5 if page crossed)
	case 0x7e:
		nes_cpu->absoluteXSet(address, result);
		break;
	default:
		break;
	}
}

void RTI(Byte opcode, Word address) {
	// Pull status flags from stack
	nes_cpu->setP(nes_cpu->pullStack());

	// Pull PC from stack
	nes_cpu->setPC(nes_cpu->pullStack());
	nes_cpu->setCycleCount(6);
}

void RTS(Byte opcode, Word address) {
	// Pull PC from stuck and increment by 1
	nes_cpu->setPC(nes_cpu->pullStack() + 1);
	nes_cpu->setCycleCount(6);
}

void SBC(Byte opcode, Word address) {
	Byte data = 0;

	// Set data to correct data depending on address mode
	switch (opcode) {
	// Immediate, 2 bytes, 2 cycles
	case 0xe9:
		data = nes_cpu->immediatePeek(address);
		nes_cpu->setCycleCount(2);
		nes_cpu->incrementPCBy(2);
		break;
	// Zero page, 2 bytes, 3 cycles
	case 0xe5:
		data = nes_cpu->zeroPagePeek(address);
		nes_cpu->setCycleCount(3);
		nes_cpu->incrementPCBy(2);
		break;
	// Zero page,X , 2 bytes, 4 cycles
	case 0xf5:
		data = nes_cpu->zeroPageXPeek(address);
		nes_cpu->setCycleCount(4);
		nes_cpu->incrementPCBy(2);
		break;
	// Absolute, 3 bytes, 4 cycles
	case 0xed:
		data = nes_cpu->absolutePeek(address);
		nes_cpu->setCycleCount(4);
		nes_cpu->incrementPCBy(3);
		break;
	// Absolute,X , 3 bytes, 4 cycles (5 if page crossed)
	case 0xfd:
		data = nes_cpu->absoluteXPeek(address);
		nes_cpu->setCycleCount(4);
		nes_cpu->incrementPCBy(3);
		break;
	// Absolute,Y 3 bytes, 4 cycles (5 if page crossed)
	case 0xf9:
		data = nes_cpu->absoluteYPeek(address);
		nes_cpu->setCycleCount(4);
		nes_cpu->incrementPCBy(3);
		break;
	// Indirect,X , 2 bytes, 6 cycles
	case 0xe1:
		data = nes_cpu->indirectXPeek(address);
		nes_cpu->setCycleCount(6);
		nes_cpu->incrementPCBy(2);
		break;
	// Indirect, Y 2 bytes, 5 cycles (6 if page crossed)
	case 0xf1:
		data = nes_cpu->indirectYPeek(address);
		nes_cpu->setCycleCount(5);
		nes_cpu->incrementPCBy(2);
		break;
	default:
		break;
	}

	// Result of add
	int result = nes_cpu->getA() + ~data + (nes_cpu->getP() & 1);

	// Set zero flag to 0
	nes_cpu->clearZero();

	// If there is an overflow, set carry bit
	if (result >= 0) { nes_cpu->setCarry(); }
	// If result is zero, set zero bit
	if (result == 0) { nes_cpu->setZero(); }
	// If result's sign is different from A's and memory's , signed overflow occured
	if ((result ^ nes_cpu->getA()) & (result ^ ~data) & 0x80) { nes_cpu->setOverflow(); }
	// Directly copy MSB into N flag
	nes_cpu->setP((nes_cpu->getP() & 0x7f) | (result & NEGATIVE));

	nes_cpu->setA(result);
}

void SEC(Byte opcode, Word address) {
	nes_cpu->setCarry();
	nes_cpu->setCycleCount(2);
	nes_cpu->incrementPCBy(1);
}

void SED(Byte opcode, Word address) {
	nes_cpu->setDecimal();
	nes_cpu->setCycleCount(2);
	nes_cpu->incrementPCBy(1);
}

void SEI(Byte opcode, Word address) {
	nes_cpu->setInterruptDisable();
	nes_cpu->setCycleCount(2);
	nes_cpu->incrementPCBy(1);
}

void STA(Byte opcode, Word address) {
	// Store in memory
	switch (opcode) {
	case 0x85:
		// Zero page
		nes_cpu->zeroPageSet(address, nes_cpu->getA());
		nes_cpu->setCycleCount(3);
		nes_cpu->incrementPCBy(2);
		break;
	// Zero page,X
	case 0x95:
		nes_cpu->zeroPageXSet(address, nes_cpu->getA());
		nes_cpu->setCycleCount(4);
		nes_cpu->incrementPCBy(2);
		break;
	// Absolute
	case 0x8d:
		nes_cpu->absoluteSet(address, nes_cpu->getA());
		nes_cpu->setCycleCount(4);
		nes_cpu->incrementPCBy(3);
		break;
	// Absolute,X
	case 0x9d:
		nes_cpu->absoluteXSet(address, nes_cpu->getA());
		nes_cpu->setCycleCount(5);
		nes_cpu->incrementPCBy(3);
		break;
	// Absolute,Y
	case 0x99:
		nes_cpu->absoluteYSet(address, nes_cpu->getA());
		nes_cpu->setCycleCount(5);
		nes_cpu->incrementPCBy(3);
		break;
	// Indirect,X
	case 0x81:
		nes_cpu->indirectXSet(address, nes_cpu->getA());
		nes_cpu->setCycleCount(6);
		nes_cpu->incrementPCBy(2);
		break;
	// Indirect, Y
	case 0x91:
		nes_cpu->indirectYSet(address, nes_cpu->getA());
		nes_cpu->setCycleCount(6);
		nes_cpu->incrementPCBy(2);
		break;
	default:
		break;
	}
}

void STX(Byte opcode, Word address) {
	// Store in memory
	switch (opcode) {
	// Zero page, 2 bytes, 3 cycles
	case 0x85:
		nes_cpu->zeroPageSet(address, nes_cpu->getX());
		nes_cpu->setCycleCount(3);
		nes_cpu->incrementPCBy(2);
		break;
	// Zero page,Y , 2 bytes, 4 cycles
	case 0x95:
		nes_cpu->zeroPageYSet(address, nes_cpu->getX());
		nes_cpu->setCycleCount(4);
		nes_cpu->incrementPCBy(2);
		break;
	// Absolute, 3 bytes, 4 cycles
	case 0x8d:
		nes_cpu->absoluteSet(address, nes_cpu->getX());
		nes_cpu->setCycleCount(4);
		nes_cpu->incrementPCBy(3);
		break;

	default:
		break;
	}
}

void STY(Byte opcode, Word address) {
	// Store in memory
	switch (opcode) {
	// Zero page, 2 bytes, 3 cycles
	case 0x85:
		nes_cpu->zeroPageSet(address, nes_cpu->getY());
		nes_cpu->setCycleCount(3);
		nes_cpu->incrementPCBy(2);
		break;
	// Zero page,X , 2 bytes, 4 cycles
	case 0x95:
		nes_cpu->zeroPageXSet(address, nes_cpu->getY());
		nes_cpu->setCycleCount(4);
		nes_cpu->incrementPCBy(2);
		break;
	// Absolute, 3 bytes, 4 cycles
	case 0x8d:
		nes_cpu->absoluteSet(address, nes_cpu->getY());
		nes_cpu->setCycleCount(4);
		nes_cpu->incrementPCBy(3);
		break;

	default:
		break;
	}
}

void TAX(Byte opcode, Word address) {
	int result = nes_cpu->getA();

	nes_cpu->setX(result);

	// If result is zero, set zero bit
	if (result == 0) { nes_cpu->setZero(); }
	// Directly copy MSB into N flag
	nes_cpu->setP((nes_cpu->getP() & 0x7f) | (result & NEGATIVE));
	nes_cpu->setCycleCount(2);
}

void TAY(Byte opcode, Word address) {
	int result = nes_cpu->getA();

	nes_cpu->setY(result);

	// If result is zero, set zero bit
	if (result == 0) { nes_cpu->setZero(); }
	// Directly copy MSB into N flag
	nes_cpu->setP((nes_cpu->getP() & 0x7f) | (result & NEGATIVE));
	nes_cpu->setCycleCount(2);
	nes_cpu->incrementPCBy(1);
}

void TSX(Byte opcode, Word address) {
	int result = nes_cpu->getS();

	nes_cpu->setX(result);

	// If result is zero, set zero bit
	if (result == 0) { nes_cpu->setZero(); }
	// Directly copy MSB into N flag
	nes_cpu->setP((nes_cpu->getP() & 0x7f) | (result & NEGATIVE));
	nes_cpu->setCycleCount(2);
	nes_cpu->incrementPCBy(1);
}

void TXA(Byte opcode, Word address) {
	int result = nes_cpu->getX();

	nes_cpu->setA(result);

	// If result is zero, set zero bit
	if (result == 0) { nes_cpu->setZero(); }
	// Directly copy MSB into N flag
	nes_cpu->setP((nes_cpu->getP() & 0x7f) | (result & NEGATIVE));
	nes_cpu->setCycleCount(2);
	nes_cpu->incrementPCBy(1);
}

void TXS(Byte opcode, Word address) {
	int result = nes_cpu->getX();

	nes_cpu->setS(result);

	// If result is zero, set zero bit
	if (result == 0) { nes_cpu->setZero(); }
	// Directly copy MSB into N flag
	nes_cpu->setP((nes_cpu->getP() & 0x7f) | (result & NEGATIVE));
	nes_cpu->setCycleCount(2);
	nes_cpu->incrementPCBy(1);
}

void TYA(Byte opcode, Word address) {
	int result = nes_cpu->getY();

	nes_cpu->setA(result);

	// If result is zero, set zero bit
	if (result == 0) { nes_cpu->setZero(); }
	// Directly copy MSB into N flag
	nes_cpu->setP((nes_cpu->getP() & 0x7f) | (result & NEGATIVE));
	nes_cpu->setCycleCount(2);
	nes_cpu->incrementPCBy(1);
}

/// <summary>
/// Initialise the array of function pointers
/// </summary>
void decode::initFuncArray() {
	// ADC
	opcodeFuncPointers[0x69] = ADC;
	opcodeFuncPointers[0x65] = ADC;
	opcodeFuncPointers[0x75] = ADC;
	opcodeFuncPointers[0x6D] = ADC;
	opcodeFuncPointers[0x7D] = ADC;
	opcodeFuncPointers[0x79] = ADC;
	opcodeFuncPointers[0x61] = ADC;
	opcodeFuncPointers[0x71] = ADC;

	// AND
	opcodeFuncPointers[0x29] = AND;
	opcodeFuncPointers[0x25] = AND;
	opcodeFuncPointers[0x35] = AND;
	opcodeFuncPointers[0x2D] = AND;
	opcodeFuncPointers[0x3D] = AND;
	opcodeFuncPointers[0x39] = AND;
	opcodeFuncPointers[0x21] = AND;
	opcodeFuncPointers[0x31] = AND;

	// ASL
	opcodeFuncPointers[0x0A] = ASL;
	opcodeFuncPointers[0x06] = ASL;
	opcodeFuncPointers[0x16] = ASL;
	opcodeFuncPointers[0x0E] = ASL;
	opcodeFuncPointers[0x1E] = ASL;

	// BCC
	opcodeFuncPointers[0x90] = BCC;

	// BCS
	opcodeFuncPointers[0xB0] = BCS;

	// BEQ
	opcodeFuncPointers[0xF0] = BEQ;

	// BIT
	opcodeFuncPointers[0x24] = BIT;
	opcodeFuncPointers[0x2C] = BIT;

	// BMI
	opcodeFuncPointers[0x30] = BMI;

	// BNE
	opcodeFuncPointers[0xD0] = BNE;

	// BPL
	opcodeFuncPointers[0x10] = BPL;

	// BRK
	opcodeFuncPointers[0x00] = BRK;

	// BVC
	opcodeFuncPointers[0x50] = BVC;

	// BVS
	opcodeFuncPointers[0x70] = BVS;

	// CLC
	opcodeFuncPointers[0x18] = CLC;

	// CLD
	opcodeFuncPointers[0xD8] = CLD;

	// CLI
	opcodeFuncPointers[0x58] = CLI;

	// CLV
	opcodeFuncPointers[0xB8] = CLV;

	// CMP
	opcodeFuncPointers[0xC9] = CMP;
	opcodeFuncPointers[0xC5] = CMP;
	opcodeFuncPointers[0xD5] = CMP;
	opcodeFuncPointers[0xCD] = CMP;
	opcodeFuncPointers[0xDD] = CMP;
	opcodeFuncPointers[0xD9] = CMP;
	opcodeFuncPointers[0xC1] = CMP;
	opcodeFuncPointers[0xD1] = CMP;

	// CPX
	opcodeFuncPointers[0xE0] = CPX;
	opcodeFuncPointers[0xE4] = CPX;
	opcodeFuncPointers[0xEC] = CPX;

	// CPY
	opcodeFuncPointers[0xC0] = CPY;
	opcodeFuncPointers[0xC4] = CPY;
	opcodeFuncPointers[0xCC] = CPY;

	// DEC
	opcodeFuncPointers[0xC6] = DEC;
	opcodeFuncPointers[0xD6] = DEC;
	opcodeFuncPointers[0xCE] = DEC;
	opcodeFuncPointers[0xDE] = DEC;

	// DEX
	opcodeFuncPointers[0xCA] = DEX;

	// DEY
	opcodeFuncPointers[0x88] = DEY;

	// EOR
	opcodeFuncPointers[0x49] = EOR;
	opcodeFuncPointers[0x45] = EOR;
	opcodeFuncPointers[0x55] = EOR;
	opcodeFuncPointers[0x4D] = EOR;
	opcodeFuncPointers[0x5D] = EOR;
	opcodeFuncPointers[0x59] = EOR;
	opcodeFuncPointers[0x41] = EOR;
	opcodeFuncPointers[0x51] = EOR;

	// INC
	opcodeFuncPointers[0xE6] = INC;
	opcodeFuncPointers[0xF6] = INC;
	opcodeFuncPointers[0xEE] = INC;
	opcodeFuncPointers[0xFF] = INC;

	// INX
	opcodeFuncPointers[0xE8] = INX;

	// INY
	opcodeFuncPointers[0xC8] = INY;

	// JMP
	opcodeFuncPointers[0x4C] = JMP;
	opcodeFuncPointers[0x6C] = JMP;

	// JSR
	opcodeFuncPointers[0x20] = JSR;

	// LDA
	opcodeFuncPointers[0xA9] = LDA;
	opcodeFuncPointers[0xA5] = LDA;
	opcodeFuncPointers[0xB5] = LDA;
	opcodeFuncPointers[0xAD] = LDA;
	opcodeFuncPointers[0xBD] = LDA;
	opcodeFuncPointers[0xB9] = LDA;
	opcodeFuncPointers[0xA1] = LDA;
	opcodeFuncPointers[0xB1] = LDA;

	// LDX
	opcodeFuncPointers[0xA2] = LDX;
	opcodeFuncPointers[0xA6] = LDX;
	opcodeFuncPointers[0xB6] = LDX;
	opcodeFuncPointers[0xAE] = LDX;
	opcodeFuncPointers[0xBE] = LDX;

	// LDY
	opcodeFuncPointers[0xA0] = LDY;
	opcodeFuncPointers[0xA4] = LDY;
	opcodeFuncPointers[0xB4] = LDY;
	opcodeFuncPointers[0xAC] = LDY;
	opcodeFuncPointers[0xBC] = LDY;

	// LSR
	opcodeFuncPointers[0x4A] = LSR;
	opcodeFuncPointers[0x46] = LSR;
	opcodeFuncPointers[0x56] = LSR;
	opcodeFuncPointers[0x4E] = LSR;
	opcodeFuncPointers[0x5E] = LSR;

	// NOP
	opcodeFuncPointers[0xEA] = NOP;

	// ORA
	opcodeFuncPointers[0x09] = ORA;
	opcodeFuncPointers[0x05] = ORA;
	opcodeFuncPointers[0x15] = ORA;
	opcodeFuncPointers[0x0D] = ORA;
	opcodeFuncPointers[0x1D] = ORA;
	opcodeFuncPointers[0x19] = ORA;
	opcodeFuncPointers[0x01] = ORA;
	opcodeFuncPointers[0x11] = ORA;

	// PHA
	opcodeFuncPointers[0x48] = PHA;

	//PHP
	opcodeFuncPointers[0x08] = PHP;

	//PLA
	opcodeFuncPointers[0x68] = PLA;

	//PLP
	opcodeFuncPointers[0x28] = PLP;

	// ROL
	opcodeFuncPointers[0x2A] = ROL;
	opcodeFuncPointers[0x26] = ROL;
	opcodeFuncPointers[0x36] = ROL;
	opcodeFuncPointers[0x2E] = ROL;
	opcodeFuncPointers[0x3E] = ROL;

	// ROR
	opcodeFuncPointers[0x6A] = ROR;
	opcodeFuncPointers[0x66] = ROR;
	opcodeFuncPointers[0x76] = ROR;
	opcodeFuncPointers[0x6E] = ROR;
	opcodeFuncPointers[0x7E] = ROR;

	// RTI
	opcodeFuncPointers[0x40] = RTI;

	// RTS
	opcodeFuncPointers[0x60] = RTS;

	// SBC
	opcodeFuncPointers[0xE9] = SBC;
	opcodeFuncPointers[0xE5] = SBC;
	opcodeFuncPointers[0xF5] = SBC;
	opcodeFuncPointers[0xED] = SBC;
	opcodeFuncPointers[0xFD] = SBC;
	opcodeFuncPointers[0xF9] = SBC;
	opcodeFuncPointers[0xE1] = SBC;
	opcodeFuncPointers[0xF1] = SBC;

	// SEC
	opcodeFuncPointers[0x38] = SEC;

	// SED
	opcodeFuncPointers[0xF8] = SED;

	// SEI
	opcodeFuncPointers[0x78] = SEI;

	// STA
	opcodeFuncPointers[0x85] = STA;
	opcodeFuncPointers[0x95] = STA;
	opcodeFuncPointers[0x8D] = STA;
	opcodeFuncPointers[0x9D] = STA;
	opcodeFuncPointers[0x99] = STA;
	opcodeFuncPointers[0x81] = STA;
	opcodeFuncPointers[0x91] = STA;

	// STX
	opcodeFuncPointers[0x86] = STX;
	opcodeFuncPointers[0x96] = STX;
	opcodeFuncPointers[0x8E] = STX;

	// STY
	opcodeFuncPointers[0x84] = STY;
	opcodeFuncPointers[0x94] = STY;
	opcodeFuncPointers[0x8C] = STY;

	// TAX
	opcodeFuncPointers[0xAA] = TAX;

	// TAY
	opcodeFuncPointers[0xA8] = TAY;

	// TSX
	opcodeFuncPointers[0xBA] = TSX;

	// TXA
	opcodeFuncPointers[0x8A] = TXA;

	// TXS
	opcodeFuncPointers[0x9A] = TXS;

	// TYA
	opcodeFuncPointers[0x98] = TYA;
}
