#include "Opcodes.h"

// opcode $4B
void ALR(Byte opcode, Word address) {
	Byte data = nes_cpu->immediatePeek(address);
	data = nes_cpu->getA() & data;

	nes_cpu->clearCarry();
	if (data & 1) nes_cpu->setCarry();

	data >>= 1;

	nes_cpu->clearZero();
	nes_cpu->clearNegative();

	if (data == 0) nes_cpu->setZero();

	nes_cpu->setA(data);
	nes_cpu->incrementPCBy(2);
	nes_cpu->setCycleCount(2);
}

//opcode $0B and $2B
void ANC(Byte opcode, Word address) {
	Byte data = nes_cpu->immediatePeek(address);
	data = nes_cpu->getA() & data;

	nes_cpu->clearZero();
	nes_cpu->clearCarry();
	nes_cpu->clearNegative();
	
	if (data == 0) nes_cpu->setZero();
	if (data & 128) {
		nes_cpu->setCarry();
		nes_cpu->setNegative();
	}

	nes_cpu->setA(data);
	nes_cpu->incrementPCBy(2);
	nes_cpu->setCycleCount(2);
}

// opcode $8B
// TODO check if right
void ANE(Byte opcode, Word address) {
	Byte data = nes_cpu->immediatePeek(address);
	data = (nes_cpu->getA() & nes_cpu->getX()) & data;

	nes_cpu->clearZero();
	nes_cpu->clearNegative();

	if (data == 0) nes_cpu->setZero();
	if (data & 128) nes_cpu->setNegative();

	nes_cpu->setA(data);
	nes_cpu->incrementPCBy(2);
	nes_cpu->setCycleCount(2);
}

// opcode $6B
// todo implemnet
void ARR(Byte opcode, Word address) {
	Byte data = nes_cpu->immediatePeek(address);
	data &= nes_cpu->getA();
}

void DCP(Byte opcode, Word address) {
	Byte data = 0;

	switch (opcode) {
	case 0xC7:
		data = nes_cpu->zeroPagePeek(address);
		data--;
		nes_cpu->zeroPageSet(address, data);

		nes_cpu->setCycleCount(5);
		nes_cpu->incrementPCBy(2);
		break;
	case 0XD7:
		data = nes_cpu->zeroPageXPeek(address);
		data--;
		nes_cpu->zeroPageXSet(address, data);

		nes_cpu->setCycleCount(6);
		nes_cpu->incrementPCBy(2);
		break;
	case 0xCF:
		data = nes_cpu->absolutePeek(address);
		data--;
		nes_cpu->absoluteSet(address, data);

		nes_cpu->setCycleCount(6);
		nes_cpu->incrementPCBy(3);
		break;
	case 0xDF:
		data = nes_cpu->absoluteXPeek(address);
		data--;
		nes_cpu->absoluteXSet(address, data);

		nes_cpu->setCycleCount(7);
		nes_cpu->incrementPCBy(3);
		break;
	case 0xDB:
		data = nes_cpu->absoluteYPeek(address);
		data--;
		nes_cpu->absoluteYSet(address, data);

		nes_cpu->setCycleCount(7);
		nes_cpu->incrementPCBy(3);
		break;
	case 0xC3:
		data = nes_cpu->indirectXPeek(address);
		data--;
		nes_cpu->indirectXSet(address, data);

		nes_cpu->setCycleCount(8);
		nes_cpu->incrementPCBy(2);
		break;
	case 0xD3:
		data = nes_cpu->indirectYPeek(address);
		data--;
		nes_cpu->indirectYSet(address, data);

		nes_cpu->setCycleCount(8);
		nes_cpu->incrementPCBy(2);
		break;
	default:
		break;
	}

	int result = nes_cpu->getA() - data;

	nes_cpu->clearZero();
	nes_cpu->clearCarry();
	nes_cpu->clearNegative();

	if (result == 0) nes_cpu->setZero();
	if (result >= 0) nes_cpu->setCarry();
	if (result & 128) nes_cpu->setNegative();
}

void ISB(Byte opcode, Word address) {
	Byte data = 0;

	switch (opcode) {
	case 0xE7:
		data = nes_cpu->zeroPagePeek(address);

		nes_cpu->setCycleCount(5);
		nes_cpu->incrementPCBy(2);
		break;
	case 0xF7:
		data = nes_cpu->zeroPageXPeek(address);
		nes_cpu->setCycleCount(6);
		nes_cpu->incrementPCBy(2);
		break;
	case 0xEF:
		data = nes_cpu->absolutePeek(address);

		nes_cpu->setCycleCount(6);
		nes_cpu->incrementPCBy(3);
		break;
	case 0xFF:
		data = nes_cpu->absoluteXPeek(address);

		nes_cpu->setCycleCount(7);
		nes_cpu->incrementPCBy(3);
		break;
	case 0xFB:
		data = nes_cpu->absoluteYPeek(address);

		nes_cpu->setCycleCount(7);
		nes_cpu->incrementPCBy(3);
		break;
	case 0xE3:
		data = nes_cpu->indirectXPeek(address);

		nes_cpu->setCycleCount(8);
		nes_cpu->incrementPCBy(2);
		break;
	case 0xF3:
		data = nes_cpu->indirectYPeek(address);

		nes_cpu->setCycleCount(8);
		nes_cpu->incrementPCBy(2);
		break;
	default:
		break;
	}

	data++;

	switch (opcode) {
	case 0xE7:
		nes_cpu->zeroPageSet(address, data);
		break;
	case 0xF7:
		nes_cpu->zeroPageXSet(address, data);
		break;
	case 0xEF:
		nes_cpu->absoluteSet(address, data);
		break;
	case 0xFF:
		nes_cpu->absoluteXSet(address, data);
		break;
	case 0xFB:
		nes_cpu->absoluteYSet(address, data);
		break;
	case 0xE3:
		nes_cpu->indirectXSet(address, data);
		break;
	case 0xF3:
		nes_cpu->indirectYSet(address, data);
		break;
	default:
		break;
	}

	int result = nes_cpu->getA() - data - (1 - (nes_cpu->getP() & CARRY));
	nes_cpu->setA(result);

	nes_cpu->clearZero();
	nes_cpu->clearCarry();
	nes_cpu->clearOverflow();
	nes_cpu->clearNegative();

	if (result >= 0) nes_cpu->setCarry();
	if (result % 256 == 0) nes_cpu->setZero();
	if ((result ^ nes_cpu->getA()) & (result ^ ~data) & 0x80) nes_cpu->setOverflow();
	if (result & 128) nes_cpu->setNegative();
}

void LAS(Byte opcode, Word address) {
	Byte data = nes_cpu->absoluteYPeek(address);
	data &= nes_cpu->getS();

	nes_cpu->setA(data);
	nes_cpu->setX(data);
	nes_cpu->setS(data);

	nes_cpu->clearZero();
	nes_cpu->clearNegative();

	if (data == 0) nes_cpu->setZero();
	if (data & 128) nes_cpu->setNegative();

	nes_cpu->incrementPCBy(3);
	nes_cpu->setCycleCount(4);
}

void LAX(Byte opcode, Word address) {
	Byte data = 0;

	switch (opcode) {
	case 0xA7:
		data = nes_cpu->zeroPagePeek(address);

		nes_cpu->incrementPCBy(2);
		nes_cpu->setCycleCount(3);
		break;
	case 0xB7:
		data = nes_cpu->zeroPageYPeek(address);

		nes_cpu->incrementPCBy(2);
		nes_cpu->setCycleCount(4);
		break;
	case 0xAF:
		data = nes_cpu->absolutePeek(address);

		nes_cpu->incrementPCBy(3);
		nes_cpu->setCycleCount(4);
		break;
	case 0xBF:
		data = nes_cpu->absoluteYPeek(address);

		nes_cpu->incrementPCBy(3);
		nes_cpu->setCycleCount(4 + nes_cpu->wasPageBoundaryCrossedOnPeek());
		break;
	case 0xA3:
		data = nes_cpu->indirectXPeek(address);

		nes_cpu->incrementPCBy(2);
		nes_cpu->setCycleCount(6);
		break;
	case 0xB3:
		data = nes_cpu->indirectYPeek(address);

		nes_cpu->incrementPCBy(2);
		nes_cpu->setCycleCount(5 + nes_cpu->wasPageBoundaryCrossedOnPeek());
		break;
	default:
		break;
	}

	nes_cpu->setA(data);
	nes_cpu->setX(data);

	nes_cpu->clearZero();
	nes_cpu->clearNegative();

	if (data == 0) nes_cpu->setZero();
	if (data & 128) nes_cpu->setNegative();
}

// TODO implement
void LXA(Byte opcode, Word address) {
	
}

// TODO NEATEN THIS CODE, too long
void RLA(Byte opcode, Word address) {
	Byte data = 0;
	bool tempFlag;

	switch (opcode) {
	case 0x27:
		data = nes_cpu->zeroPagePeek(address);

		nes_cpu->setCycleCount(5);
		nes_cpu->incrementPCBy(2);
		break;
	case 0x37:
		data = nes_cpu->zeroPageXPeek(address);

		nes_cpu->setCycleCount(6);
		nes_cpu->incrementPCBy(2);
		break;
	case 0x2F:
		data = nes_cpu->absolutePeek(address);

		nes_cpu->setCycleCount(6);
		nes_cpu->incrementPCBy(3);
		break;
	case 0x3F:
		data = nes_cpu->absoluteXPeek(address);

		nes_cpu->setCycleCount(7);
		nes_cpu->incrementPCBy(3);
		break;
	case 0x3B:
		data = nes_cpu->absoluteYPeek(address);

		nes_cpu->setCycleCount(7);
		nes_cpu->incrementPCBy(3);
		break;
	case 0x23:
		data = nes_cpu->indirectXPeek(address);

		nes_cpu->setCycleCount(8);
		nes_cpu->incrementPCBy(2);
		break;
	case 0x33:
		data = nes_cpu->indirectYPeek(address);

		nes_cpu->setCycleCount(8);
		nes_cpu->incrementPCBy(2);
		break;
	default:
		break;
	}

	tempFlag = nes_cpu->getP() & CARRY;

	nes_cpu->clearCarry();
	if (data & 128) nes_cpu->setCarry();

	data <<= 1;
	data |= tempFlag;

	switch (opcode) {
	case 0x27:
		nes_cpu->zeroPageSet(address, data);
		break;
	case 0x37:
		nes_cpu->zeroPageXSet(address, data);
		break;
	case 0x2F:
		nes_cpu->absoluteSet(address, data);
		break;
	case 0x3F:
		nes_cpu->absoluteXSet(address, data);
		break;
	case 0x3B:
		nes_cpu->absoluteYSet(address, data);
		break;
	case 0x23:
		nes_cpu->indirectXSet(address, data);
		break;
	case 0x33:
		nes_cpu->indirectYSet(address, data);
		break;
	default:
		break;
	}

	data = nes_cpu->getA() & data;
	nes_cpu->setA(data);

	nes_cpu->clearZero();
	nes_cpu->clearNegative();

	if (data == 0) nes_cpu->setZero();
	if (data & 128) nes_cpu->setNegative();
}

void RRA(Byte opcode, Word address) {
	Byte data = 0;

	switch (opcode) {
	case 0x67:
		data = nes_cpu->zeroPagePeek(address);

		nes_cpu->setCycleCount(5);
		nes_cpu->incrementPCBy(2);
		break;
	case 0X77:
		data = nes_cpu->zeroPageXPeek(address);

		nes_cpu->setCycleCount(6);
		nes_cpu->incrementPCBy(2);
		break;
	case 0x6F:
		data = nes_cpu->absolutePeek(address);

		nes_cpu->setCycleCount(6);
		nes_cpu->incrementPCBy(3);
		break;
	case 0x7F:
		data = nes_cpu->absoluteXPeek(address);

		nes_cpu->setCycleCount(7);
		nes_cpu->incrementPCBy(3);
		break;
	case 0x7B:
		data = nes_cpu->absoluteYPeek(address);

		nes_cpu->setCycleCount(7);
		nes_cpu->incrementPCBy(3);
		break;
	case 0x63:
		data = nes_cpu->indirectXPeek(address);

		nes_cpu->setCycleCount(8);
		nes_cpu->incrementPCBy(2);
		break;
	case 0x73:
		data = nes_cpu->indirectYPeek(address);

		nes_cpu->setCycleCount(8);
		nes_cpu->incrementPCBy(2);
		break;
	default:
		break;
	}

	int tempCarryFlag = (nes_cpu->getP() & CARRY) << 7;

	nes_cpu->clearCarry();
	if (data & 1) nes_cpu->setCarry();

	// ROR
	data >>= 1;
	data |= tempCarryFlag;

	// Put modified data back in memory
	switch (opcode) {
	case 0x67:
		nes_cpu->zeroPageSet(address, data);
		break;
	case 0X77:
		nes_cpu->zeroPageXSet(address, data);
		break;
	case 0x6F:
		nes_cpu->absoluteSet(address, data);
		break;
	case 0x7F:
		nes_cpu->absoluteXSet(address, data);
		break;
	case 0x7B:
		nes_cpu->absoluteYSet(address, data);
		break;
	case 0x63:
		nes_cpu->indirectXSet(address, data);
		break;
	case 0x73:
		nes_cpu->indirectYSet(address, data);
		break;
	default:
		break;
	}

	// ADC
	int result = nes_cpu->getA() + data + nes_cpu->isCarrySet();
	nes_cpu->setA(result);

	nes_cpu->clearZero();
	nes_cpu->clearCarry();
	nes_cpu->clearOverflow();
	nes_cpu->clearNegative();

	if (result % 256 == 0) nes_cpu->setZero();
	if (result > 0xFF) nes_cpu->setCarry();
	if ((result ^ nes_cpu->getA()) & (result ^ data) & 0x80) nes_cpu->setOverflow();
	if (result & 128) nes_cpu->setNegative();
}

void SAX(Byte opcode, Word address) {
	Byte data = nes_cpu->getA() & nes_cpu->getX();

	switch (opcode) {
	case 0x87:
		nes_cpu->zeroPageSet(address, data);
		nes_cpu->incrementPCBy(2);
		nes_cpu->setCycleCount(3);
		break;
	case 0x97:
		nes_cpu->zeroPageYSet(address, data);
		nes_cpu->incrementPCBy(2);
		nes_cpu->setCycleCount(4);
		break;
	case 0x8F:
		nes_cpu->absoluteSet(address, data);
		nes_cpu->incrementPCBy(3);
		nes_cpu->setCycleCount(4);
		break;
	case 0x83:
		nes_cpu->indirectXSet(address, data);
		nes_cpu->incrementPCBy(2);
		nes_cpu->setCycleCount(6);
		break;
	default:
		break;
	}
}

void SBX(Byte opcode, Word address) {
	Byte data = nes_cpu->immediatePeek(address);
	int result = (nes_cpu->getA() & nes_cpu->getX()) - data;

	nes_cpu->clearCarry();
	nes_cpu->clearNegative();
	nes_cpu->clearZero();

	if (result >= 0) nes_cpu->setCarry();
	if (result == 0) nes_cpu->setZero();
	if (result & 128) nes_cpu->setNegative();

}

// Not tested by nestest.nes
// unstable
// todo consider implementing or not supporting
void SHA(Byte opcode, Word address) {
	Byte data = 0;

	switch (opcode) {
	case 0x9F:
		break;
	case 0x93:
		break;
	default:
		break;
	}
}

// Not tested by nestest.nes
// unstable
// todo consider removing or implementing
void SHX(Byte opcode, Word address) {
	
}

// Not tested by nestest.nes
// unstable
// todo consider removing or implementing
void SHY(Byte opcode, Word address) {

}

void SLO(Byte opcode, Word address) {
	Byte data = 0;

	switch (opcode) {
	case 0x07:
		data = nes_cpu->zeroPagePeek(address);
		

		nes_cpu->setCycleCount(5);
		nes_cpu->incrementPCBy(2);
		break;
	case 0X17:
		data = nes_cpu->zeroPageXPeek(address);

		nes_cpu->setCycleCount(6);
		nes_cpu->incrementPCBy(2);
		break;
	case 0x0F:
		data = nes_cpu->absolutePeek(address);

		nes_cpu->setCycleCount(6);
		nes_cpu->incrementPCBy(3);
		break;
	case 0x1F:
		data = nes_cpu->absoluteXPeek(address);

		nes_cpu->setCycleCount(7);
		nes_cpu->incrementPCBy(3);
		break;
	case 0x1B:
		data = nes_cpu->absoluteYPeek(address);

		nes_cpu->setCycleCount(7);
		nes_cpu->incrementPCBy(3);
		break;
	case 0x03:
		data = nes_cpu->indirectXPeek(address);

		nes_cpu->setCycleCount(8);
		nes_cpu->incrementPCBy(2);
		break;
	case 0x13:
		data = nes_cpu->indirectYPeek(address);

		nes_cpu->setCycleCount(8);
		nes_cpu->incrementPCBy(2);
		break;
	default:
		break;
	}

	nes_cpu->clearCarry();
	if (data & 128) nes_cpu->setCarry();

	// arithmetic shift left
	data <<= 1;

	switch (opcode) {
	case 0x07:
		nes_cpu->zeroPageSet(address, data);
		break;
	case 0X17:
		nes_cpu->zeroPageXSet(address, data);
		break;
	case 0x0F:
		nes_cpu->absoluteSet(address, data);
		break;
	case 0x1F:
		nes_cpu->absoluteXSet(address, data);
		break;
	case 0x1B:
		nes_cpu->absoluteYSet(address, data);
		break;
	case 0x03:
		nes_cpu->indirectXSet(address, data);
		break;
	case 0x13:
		nes_cpu->indirectYSet(address, data);
		break;
	default:
		break;
	}

	// ORA
	int result = data | nes_cpu->getA();
	nes_cpu->setA(result);

	nes_cpu->clearZero();
	nes_cpu->clearNegative();

	if (result % 256 == 0) nes_cpu->setZero();
	if (result & 128) nes_cpu->setNegative();
}

void SRE(Byte opcode, Word address) {
	Byte data = 0;

	switch (opcode) {
	case 0x47:
		data = nes_cpu->zeroPagePeek(address);

		nes_cpu->setCycleCount(5);
		nes_cpu->incrementPCBy(2);
		break;
	case 0X57:
		data = nes_cpu->zeroPageXPeek(address);

		nes_cpu->setCycleCount(6);
		nes_cpu->incrementPCBy(2);
		break;
	case 0x4F:
		data = nes_cpu->absolutePeek(address);

		nes_cpu->setCycleCount(6);
		nes_cpu->incrementPCBy(3);
		break;
	case 0x5F:
		data = nes_cpu->absoluteXPeek(address);

		nes_cpu->setCycleCount(7);
		nes_cpu->incrementPCBy(3);
		break;
	case 0x5B:
		data = nes_cpu->absoluteYPeek(address);

		nes_cpu->setCycleCount(7);
		nes_cpu->incrementPCBy(3);
		break;
	case 0x43:
		data = nes_cpu->indirectXPeek(address);

		nes_cpu->setCycleCount(8);
		nes_cpu->incrementPCBy(2);
		break;
	case 0x53:
		data = nes_cpu->indirectYPeek(address);

		nes_cpu->setCycleCount(8);
		nes_cpu->incrementPCBy(2);
		break;
	default:
		break;
	}

	nes_cpu->clearCarry();
	if (data & 1) nes_cpu->setCarry();

	// logic shift right
	data >>= 1;

	switch (opcode) {
	case 0x47:
		nes_cpu->zeroPageSet(address, data);
		break;
	case 0X57:
		nes_cpu->zeroPageXSet(address, data);
		break;
	case 0x4F:
		nes_cpu->absoluteSet(address, data);
		break;
	case 0x5F:
		nes_cpu->absoluteXSet(address, data);
		break;
	case 0x5B:
		nes_cpu->absoluteYSet(address, data);
		break;
	case 0x43:
		nes_cpu->indirectXSet(address, data);
		break;
	case 0x53:
		nes_cpu->indirectYSet(address, data);
		break;
	default:
		break;
	}

	// ORA
	data ^= nes_cpu->getA();
	nes_cpu->setA(data);
}

// Not tested by nestest.nes
// unstable
// TODO IMPLEMNET OR REMOVE
void TAS(Byte opcode, Word address) {
	
}

// Not tested by nestest.nes
// not unstable
// Just an immediate addressed SBC instruction that is
// undocumented.
void USBC(Byte opcode, Word address) {
	Byte data = nes_cpu->immediatePeek(address);

	int result = nes_cpu->getA() - data - (1 - (nes_cpu->getP() & CARRY));

	nes_cpu->clearZero();
	nes_cpu->clearCarry();
	nes_cpu->clearOverflow();
	nes_cpu->clearNegative();

	if (result >= 0) nes_cpu->setCarry();
	if (result % 256 == 0) nes_cpu->setZero();
	if ((result ^ nes_cpu->getA()) & (result ^ ~data) & 0x80) nes_cpu->setOverflow();
	if (result & 128) nes_cpu->setNegative();

	nes_cpu->setA(result);

	nes_cpu->setCycleCount(2);
	nes_cpu->incrementPCBy(2);
}

/*
 * The following section of code contains various functions that all correspond
 * to illegal forms of NOP. Each form uses a different addressing mode thus takes
 * up a different amount cycles and is stored using a different number of bytes.
 */

void NOPimplied(Byte opcode, Word address) {
	nes_cpu->setCycleCount(2);
	nes_cpu->incrementPCBy(1);
}

void NOPimmediate(Byte opcode, Word address) {
	nes_cpu->setCycleCount(2);
	nes_cpu->incrementPCBy(2);
}

void NOPzeropage(Byte opcode, Word address) {
	nes_cpu->setCycleCount(3);
	nes_cpu->incrementPCBy(2);
}

void NOPzeropageX(Byte opcode, Word address) {
	nes_cpu->setCycleCount(4);
	nes_cpu->incrementPCBy(2);
}

void NOPabsolute(Byte opcode, Word address) {
	nes_cpu->setCycleCount(4);
	nes_cpu->incrementPCBy(3);
}

// Todo implement increased cycle count on page border crossing
void NOPabsoluteX(Byte opcode, Word address) {
	nes_cpu->absoluteXPeek(address);

	nes_cpu->setCycleCount(4 + nes_cpu->wasPageBoundaryCrossedOnPeek());
	nes_cpu->incrementPCBy(3);
}

/*
 * NOP section finished
 */

// Freezes cpu with $FF in bus.
// TODO check if putting FF in openBusValue is valid
// Freeze CPU by not incrementing PC
void JAM(Byte opcode, Word address) {
	nes_cpu->setJAMOpcodeEncountered();
}

void opcodes::loadIllegalOpcodes() {
	opcodeFuncPointers[0x4B] = ALR;

	opcodeFuncPointers[0x0B] = ANC;
	opcodeFuncPointers[0x2B] = ANC;

	opcodeFuncPointers[0x8B] = ANE;

	//ARR

	opcodeFuncPointers[0xC7] = DCP;
	opcodeFuncPointers[0xD7] = DCP;
	opcodeFuncPointers[0xCF] = DCP;
	opcodeFuncPointers[0xDF] = DCP;
	opcodeFuncPointers[0xDB] = DCP;
	opcodeFuncPointers[0xC3] = DCP;
	opcodeFuncPointers[0xD3] = DCP;

	opcodeFuncPointers[0xE7] = ISB;
	opcodeFuncPointers[0xF7] = ISB;
	opcodeFuncPointers[0xEF] = ISB;
	opcodeFuncPointers[0xFF] = ISB;
	opcodeFuncPointers[0xFB] = ISB;
	opcodeFuncPointers[0xE3] = ISB;
	opcodeFuncPointers[0xF3] = ISB;

	opcodeFuncPointers[0xBB] = LAS;

	opcodeFuncPointers[0xA7] = LAX;
	opcodeFuncPointers[0xB7] = LAX;
	opcodeFuncPointers[0xAF] = LAX;
	opcodeFuncPointers[0xBF] = LAX;
	opcodeFuncPointers[0xA3] = LAX;
	opcodeFuncPointers[0xB3] = LAX;

	opcodeFuncPointers[0x27] = RLA;
	opcodeFuncPointers[0x37] = RLA;
	opcodeFuncPointers[0x2F] = RLA;
	opcodeFuncPointers[0x3F] = RLA;
	opcodeFuncPointers[0x3B] = RLA;
	opcodeFuncPointers[0x23] = RLA;
	opcodeFuncPointers[0x33] = RLA;

	opcodeFuncPointers[0x67] = RRA;
	opcodeFuncPointers[0x77] = RRA;
	opcodeFuncPointers[0x6F] = RRA;
	opcodeFuncPointers[0x7F] = RRA;
	opcodeFuncPointers[0x7B] = RRA;
	opcodeFuncPointers[0x63] = RRA;
	opcodeFuncPointers[0x73] = RRA;

	opcodeFuncPointers[0x87] = SAX;
	opcodeFuncPointers[0x97] = SAX;
	opcodeFuncPointers[0x8F] = SAX;
	opcodeFuncPointers[0x83] = SAX;

	opcodeFuncPointers[0xCB] = SBX;

	opcodeFuncPointers[0x07] = SLO;
	opcodeFuncPointers[0x17] = SLO;
	opcodeFuncPointers[0x0F] = SLO;
	opcodeFuncPointers[0x1F] = SLO;
	opcodeFuncPointers[0x1B] = SLO;
	opcodeFuncPointers[0x03] = SLO;
	opcodeFuncPointers[0x13] = SLO;

	opcodeFuncPointers[0x47] = SRE;
	opcodeFuncPointers[0x57] = SRE;
	opcodeFuncPointers[0x4F] = SRE;
	opcodeFuncPointers[0x5F] = SRE;
	opcodeFuncPointers[0x5B] = SRE;
	opcodeFuncPointers[0x43] = SRE;
	opcodeFuncPointers[0x53] = SRE;

	opcodeFuncPointers[0xEB] = USBC;

	opcodeFuncPointers[0x1A] = NOPimplied;
	opcodeFuncPointers[0x3A] = NOPimplied;
	opcodeFuncPointers[0x5A] = NOPimplied;
	opcodeFuncPointers[0x7A] = NOPimplied;
	opcodeFuncPointers[0xDA] = NOPimplied;
	opcodeFuncPointers[0xFA] = NOPimplied;

	opcodeFuncPointers[0x80] = NOPimmediate;
	opcodeFuncPointers[0x82] = NOPimmediate;
	opcodeFuncPointers[0x89] = NOPimmediate;
	opcodeFuncPointers[0xC2] = NOPimmediate;
	opcodeFuncPointers[0xE2] = NOPimmediate;

	opcodeFuncPointers[0x04] = NOPzeropage;
	opcodeFuncPointers[0x44] = NOPzeropage;
	opcodeFuncPointers[0x64] = NOPzeropage;

	opcodeFuncPointers[0x14] = NOPzeropageX;
	opcodeFuncPointers[0x34] = NOPzeropageX;
	opcodeFuncPointers[0x54] = NOPzeropageX;
	opcodeFuncPointers[0x74] = NOPzeropageX;
	opcodeFuncPointers[0xD4] = NOPzeropageX;
	opcodeFuncPointers[0xF4] = NOPzeropageX;

	opcodeFuncPointers[0x0C] = NOPabsolute;

	opcodeFuncPointers[0x1C] = NOPabsoluteX;
	opcodeFuncPointers[0x3C] = NOPabsoluteX;
	opcodeFuncPointers[0x5C] = NOPabsoluteX;
	opcodeFuncPointers[0x7C] = NOPabsoluteX;
	opcodeFuncPointers[0xDC] = NOPabsoluteX;
	opcodeFuncPointers[0xFC] = NOPabsoluteX;

}
