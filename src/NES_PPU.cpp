#include <NES_PPU.h>

NES_PPU* ppu;

static bool isAddressInRangeInclusive(Word address, Word addressLowerBound, Word addressUpperBound) {
	return address >= addressLowerBound && address <= addressUpperBound;
}

void NES_PPU::reset() {
	PPUCTRL = 0x00;
	PPUMASK = 0x00;
	PPUSTATUS = PPUSTATUS & 0x80;
	// OAMADDR unchanged
	PPUSCROLL = 0x00;
	// PPUADDR unchanged
	PPUDATA = 0x00;

	writeLatch = FIRST_WRITE;
	frameType = EVEN_FRAME;
}

void NES_PPU::powerup() {
	PPUCTRL = 0x00;
	PPUMASK = 0x00;
	PPUSTATUS = 0xA0;
	OAMADDR = 0X00;
	PPUSCROLL = 0x00;
	PPUADDR = 0x00;
	PPUDATA = 0x00;

	writeLatch = FIRST_WRITE;
	frameType = EVEN_FRAME;
}

void NES_PPU::run() {
	
}


Byte NES_PPU::correctPeek(Word address) {

	if (isAddressInRangeInclusive(address, 0x3000, 0x3EFF)) {
		return VRAM[address % 3840 + 0x2000];
	}

	if (isAddressInRangeInclusive(address, 0x3000, 0X3FF)) {
		return VRAM[address % 224 + 0x3F00];
	}

	return VRAM[address];
}


void NES_PPU::CpuPpuLatchwrite(Byte data) {
	CpuPpuLatch = data;
}


Byte NES_PPU::CpuPpuLatchRead()  const {
	return CpuPpuLatch;
}

void NES_PPU::PPUCTRLwrite(Byte data) {
	PPUCTRL = data;
	CpuPpuLatch = data;
}

void NES_PPU::PPUMASKwrite(Byte data) {
	PPUMASK = data;
	CpuPpuLatch = data;
}

Byte NES_PPU::PPUSTATUSread() const {
	return PPUSTATUS;
}

void NES_PPU::OAMADDRwrite(Byte data) {
	OAMADDR = data;
	CpuPpuLatch = data;
}

void NES_PPU::OAMDATAwrite(Byte data) {
	OAMDATA = data;
	CpuPpuLatch = data;
}

Byte NES_PPU::OAMDATAread() const {
	return OAMDATA;
}

void NES_PPU::PPUSCROLLwrite(Byte data) {
	PPUSCROLL = data << (8 * writeLatch);
	writeLatch = !writeLatch;

	CpuPpuLatch = data;
}

void NES_PPU::PPUADDRwrite(Byte data) {
	PPUADDR = data << (8 * writeLatch);
	writeLatch = !writeLatch;

	CpuPpuLatch = data;
}

void NES_PPU::PPUDATAwrite(Byte data) {
	PPUDATA = data;
	CpuPpuLatch = data;
}

Byte NES_PPU::PPUDATAread() const {
	return PPUDATA;
}

void NES_PPU::OAMDMAwrite(Byte data) {
	OAMDMA = data;
	CpuPpuLatch = data;
}









