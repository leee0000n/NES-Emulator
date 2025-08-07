#include <NES_PPU.h>

#include <fstream>
#include <iostream>
#include <vector>

//todo remove
#include <random>

#include "NES_CPU.h"
#include "Render.h"

NES_PPU* ppu;

static bool isAddressInRangeInclusive(Word address, Word addressLowerBound, Word addressUpperBound) {
	return address >= addressLowerBound && address <= addressUpperBound;
}

NES_PPU::NES_PPU() {
	this->scanlineNum = 0;
	this->ppuCycle = 0;
	this->CpuPpuLatch = 0x00;
	this->screen.fill(-1);
	this->VRAM.fill(0);

	reset();
	powerup();
	loadPalFile("dd");

	srand(124463453);

	for (int i = 0x23c0; i < 0x23c0 + 64; i++) {
		VRAM[i] = rand() % 256;
	}

	for (int i = 0x3f00; i < 0x3fff; i++) {
		VRAM[i] = rand() % 13;
	}
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
	// Render
	if (scanlineNum >=0 && scanlineNum < 240) {
		if (ppuCycle >= 0 && ppuCycle < 256) {

			if (scanlineNum == 0 && ppuCycle == 8) {
				scanlineNum = scanlineNum;
			}
			int coarseX = ppuCycle / 8;
			int coarseY = scanlineNum / 8;

			int fineY = scanlineNum % 8;

			Word nametableAddress = 0x2000 + 32 * coarseY + coarseX;
			int patternTableIndex = correctPeek(nametableAddress);
			Word patternTableAddress = patternTableIndex * 16 + 0x1000 * ((PPUCTRL & CHOSEN_PATTERN_TABLE) >> 3);

			Byte loByte = correctPeek(patternTableAddress + fineY);
			Byte hiByte = correctPeek(patternTableAddress + 8 + fineY);

			// Byte in attribute table for corresponding 4x4 tile (attribyte = attribute byte! hehe)
			Byte attribyte = correctPeek(0x23c0 + coarseX / 4 + coarseY / 4 * 8);

			// Used to determine which part of the attribyte to use
			// when deciding which palette to use
			int attribyteX = ppuCycle % 8;
			int attribyteY = scanlineNum / 8;

			int paletteChoice = attribyte >> (attribyteY % 2 * 4) >>
							    (attribyteX % 2);
			paletteChoice = paletteChoice % 4;

			addToScreen(loByte, hiByte, attribyte, paletteChoice);
		}
		
	}
	else if (scanlineNum == 240 && ppuCycle == 0) {
		render::renderScreen(screen);
	}

	// Set VBLANK
	else if (scanlineNum == 241 && ppuCycle == 1) {
		PPUCTRL |= VBLANK;
		nes_cpu->setNMI();
	}


	// VBlank lines, PPU doesn't access memory
	else if (scanlineNum > 241 && scanlineNum <= 260) {
		
	}

	// Pre render line
	else if (scanlineNum == 261) {
		// Clear VBLANK
		if (ppuCycle == 1) {
			PPUCTRL &= ~VBLANK;
			nes_cpu->setNMI();
		}
	}

	ppuCycle++;

	if (scanlineNum > 261) scanlineNum = 0;
	if (ppuCycle > 340) {
		ppuCycle = 0;
		scanlineNum++;
	}
}

void NES_PPU::addToScreen(Byte loByte, Byte hiByte, Byte attribyte, int paletteChoice) {
	int i = ppuCycle % 8;
	Byte nesColourValue = (((loByte << i) & 128) >> 7) | (((hiByte << i) & 128) >> 6);
	Word colourAddress = 0x3F0 + paletteChoice * 4 + nesColourValue;

	if ((colourAddress - 0x3F0) % 4 == 0) {
		screen[scanlineNum * 256 + ppuCycle] = -1;
		return;
	}

	int colour = correctPeek(colourAddress);
	screen[scanlineNum * 256 + ppuCycle] = colour % 0x3F;
}

bool NES_PPU::loadCHRROM(std::string path) {
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
		for (int i = 0; i < 0x2000; i++) {
			VRAM[i] = buffer[0x4000 + i + 16];
		}

	}
	else {
		std::cerr << "Error reading file.\n";
		return false;
	}

	return true;
}


void NES_PPU::default2c02Palette() {
	render::nesPaletteAsRGB[0x00] = 0x626262;
	render::nesPaletteAsRGB[0x01] = 0x001c95;
	render::nesPaletteAsRGB[0x02] = 0x1904ac;
	render::nesPaletteAsRGB[0x03] = 0x42009d;
	render::nesPaletteAsRGB[0x04] = 0x61006b;
	render::nesPaletteAsRGB[0x05] = 0x6e0025;
	render::nesPaletteAsRGB[0x06] = 0x650500;
	render::nesPaletteAsRGB[0x07] = 0x491e00;
	render::nesPaletteAsRGB[0x08] = 0x223700;
	render::nesPaletteAsRGB[0x09] = 0x004900;
	render::nesPaletteAsRGB[0x0A] = 0x004f00;
	render::nesPaletteAsRGB[0x0B] = 0x004816;
	render::nesPaletteAsRGB[0x0C] = 0x00355e;
	render::nesPaletteAsRGB[0x0D] = 0x000000;
	render::nesPaletteAsRGB[0x0E] = 0x000000;
	render::nesPaletteAsRGB[0x0F] = 0x000000;

	render::nesPaletteAsRGB[0x10] = 0xababab;
	render::nesPaletteAsRGB[0x11] = 0x0c4edb;
	render::nesPaletteAsRGB[0x12] = 0x3d2eff;
	render::nesPaletteAsRGB[0x13] = 0x7115f3;
	render::nesPaletteAsRGB[0x14] = 0x9b0bb9;
	render::nesPaletteAsRGB[0x15] = 0xb01262;
	render::nesPaletteAsRGB[0x16] = 0xa92704;
	render::nesPaletteAsRGB[0x17] = 0x894600;
	render::nesPaletteAsRGB[0x18] = 0x576600;
	render::nesPaletteAsRGB[0x19] = 0x237f00;
	render::nesPaletteAsRGB[0x1A] = 0x008900;
	render::nesPaletteAsRGB[0x1B] = 0x008332;
	render::nesPaletteAsRGB[0x1C] = 0x006d90;
	render::nesPaletteAsRGB[0x1D] = 0x000000;
	render::nesPaletteAsRGB[0x1E] = 0x000000;
	render::nesPaletteAsRGB[0x1F] = 0x000000;

	render::nesPaletteAsRGB[0x20] = 0xffffff;
	render::nesPaletteAsRGB[0x21] = 0x57a5ff;
	render::nesPaletteAsRGB[0x22] = 0x8287ff;
	render::nesPaletteAsRGB[0x23] = 0xb46dff;
	render::nesPaletteAsRGB[0x24] = 0xdf60ff;
	render::nesPaletteAsRGB[0x25] = 0xf863c6;
	render::nesPaletteAsRGB[0x26] = 0xf8746d;
	render::nesPaletteAsRGB[0x27] = 0xde9020;
	render::nesPaletteAsRGB[0x28] = 0xb3ae00;
	render::nesPaletteAsRGB[0x29] = 0x81c800;
	render::nesPaletteAsRGB[0x2A] = 0x56d522;
	render::nesPaletteAsRGB[0x2B] = 0x3dd36f;
	render::nesPaletteAsRGB[0x2C] = 0x3ec1c8;
	render::nesPaletteAsRGB[0x2D] = 0x4e4e4e;
	render::nesPaletteAsRGB[0x2E] = 0x000000;
	render::nesPaletteAsRGB[0x2F] = 0x000000;

	render::nesPaletteAsRGB[0x30] = 0xffffff;
	render::nesPaletteAsRGB[0x31] = 0xbee0ff;
	render::nesPaletteAsRGB[0x32] = 0xcdd4ff;
	render::nesPaletteAsRGB[0x33] = 0xe0caff;
	render::nesPaletteAsRGB[0x34] = 0xf1c4ff;
	render::nesPaletteAsRGB[0x35] = 0xfcc4ef;
	render::nesPaletteAsRGB[0x36] = 0xfdcace;
	render::nesPaletteAsRGB[0x37] = 0xf5d4af;
	render::nesPaletteAsRGB[0x38] = 0xe6df9c;
	render::nesPaletteAsRGB[0x39] = 0xd3e99a;
	render::nesPaletteAsRGB[0x3A] = 0xc2efa8;
	render::nesPaletteAsRGB[0x3B] = 0xaee4bb;
	render::nesPaletteAsRGB[0x3C] = 0xb6eae5;
	render::nesPaletteAsRGB[0x3D] = 0xb8b8b8;
	render::nesPaletteAsRGB[0x3E] = 0x000000;
	render::nesPaletteAsRGB[0x3F] = 0x000000;
}

bool NES_PPU::loadPalFile(std::string path) {
	// Open the file in binary mode and move cursor to the end to get size
	std::ifstream file(path, std::ios::binary | std::ios::ate);

	if (!file) {
		default2c02Palette();
		std::cerr << "File path invalid. Defaulted to NTSC colours!\n";
		return false;
	}

	// Get size and allocate buffer
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	// If size not required size, default to 2c02 palette
	if (size < 192) {
		default2c02Palette();
		std::cerr << "Not enough bytes in file. Defaulted to NTSC colours!\n";
		return false;
	}

	std::vector<char> buffer(size);

	if (file.read(buffer.data(), size)) {
		// Load rgb values for each nes colour
		for (int i = 0; i < 192; i+=3) {
			int rgb = (buffer[i] << 16) +
					  (buffer[i + 1] << 8) +
					  buffer[i + 2];

			render::nesPaletteAsRGB[i / 3] = rgb; 
		}

	}
	else {
		default2c02Palette();
		std::cerr << "Error reading file. Defaulted to NTSC colours!\n";
		return false;
	}

	return true;
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
	OAM[OAMADDR] = data;
	CpuPpuLatch = data;
}

Byte NES_PPU::OAMDATAread() const {
	return OAMDATA;
}

void NES_PPU::PPUSCROLLwrite(Byte data) {
	if (writeLatch == FIRST_WRITE) {
		PPUSCROLL = (PPUSCROLL & 0x00FF) | ((data) << 8); // REMOVE 2 MSB since they are irrelevant
	}
	else {
		PPUSCROLL = (PPUSCROLL & 0xFF00) | data;
	}
	writeLatch = !writeLatch;

	CpuPpuLatch = data;
}

void NES_PPU::PPUADDRwrite(Byte data) {
	if (writeLatch == FIRST_WRITE) {
		PPUADDR = (PPUADDR & 0x00FF) | ((data & 0x3F) << 8); // REMOVE 2 MSB since they are irrelevant
	}
	else {
		PPUADDR = (PPUADDR & 0xFF00) | data;
	}
	writeLatch = !writeLatch;

	CpuPpuLatch = data;
}

void NES_PPU::PPUDATAwrite(Byte data) {
	if (PPUADDR >= 0x2fc0 && PPUADDR < 0x2fc0 + 64 ) {
		ppuCycle = ppuCycle;
	}

	VRAM[PPUADDR] = data;
	CpuPpuLatch = data;
}

Byte NES_PPU::PPUDATAread() const {
	return PPUDATA;
}

void NES_PPU::OAMDMAwrite(Byte data) {
	OAMDMA = data;
	CpuPpuLatch = data;
}

int NES_PPU::getPPUCycle() {
	return ppuCycle;
}

int NES_PPU::getPPUScanline() {
	return scanlineNum;
}

void NES_PPU::drawPatternTables() {
	for (int i = 0; i < 255; i++) {
		drawTile(i, 0);
		drawTile(i, 1);
	}

	render::renderScreen(screen);
}
void NES_PPU::drawTile(int tileIndex, int tableNum) {
	for (int i = 0; i < 8; i++) {
		Word loByteAddress = tileIndex * 16 + i + 0x1000 * tableNum;
		Byte loByte = correctPeek(loByteAddress);
		Byte hiByte = correctPeek(loByteAddress + 8);

		int tileX = tileIndex % 16 * 8;
		int tileY = tileIndex / 16 * 256 * 8;

		for (int j = 0; j < 8; j++) {
			int colourChoice = ((loByte & 128) >> 7) + ((hiByte & 128) >> 6);

			
			if (colourChoice == 0) {
				screen[j + i * 64] = -1;
			}
			
			else {
				int x = tileIndex % 16;
				int y = tileIndex / 16;

				int gridX = x / 2;
				int gridY = y / 2;

				int attribyte = correctPeek(0x23C0 + gridX + gridY * 8);

				int decideX = x % 2;
				int decideY = y % 2;

				int paletteIndex = (attribyte >> (decideX * 2)) >> (decideY * 4);
				paletteIndex &= 0x03;

				Word colouraddress = 0x3F00 + paletteIndex * 4 + colourChoice;
				int colour = correctPeek(colouraddress);

				screen[j + i * 256 + tileX + tileY + tableNum * 128] = colour;
			}
			int b = j + i * 256 + tileX + tileY;
			loByte <<= 1;
			hiByte <<= 1;
		}
	}
}







