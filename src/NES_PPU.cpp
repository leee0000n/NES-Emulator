#include <NES_PPU.h>

#include <fstream>
#include <iostream>
#include <vector>

//todo remove
#include <random>

#include "NES_CPU.h"
#include "Render.h"

#include "NES_PPUdebug.h"

bool inline IS_RENDERING_ENABLED(Byte PPUMASK) {
	return PPUMASK & 0x18;
}


int inline EXTRACT_FINE_Y(Word V) {
	return (V & 0x7000) >> 12;
}

int inline EXTRACT_COARSE_X(Word V) {
	return V & 0x1F;
}

int inline EXTRACT_COARSE_Y(Word V) {
	return (V >> 5) & 0x1F;
}

NES_PPU* ppu;

static bool isAddressInRangeInclusive(Word address, Word addressLowerBound, Word addressUpperBound) {
	return address >= addressLowerBound && address <= addressUpperBound;
}

NES_PPU::NES_PPU() {
	this->scanlineNum = 0;
	this->ppuDot = 0;
	this->CpuPpuLatch = 0x00;
	this->screen.fill(-1);
	this->VRAM.fill(0);
	this->V = 0;
	this->T = 0;
	this->pixelsToRender.fill(-1);
	this->currentPixel = 0;
	this->renderingEnabled = false;
	this->ppuCycle = 0;

	this->frameCount = 0;

	powerup();
}


void NES_PPU::reset() {
	PPUCTRL = 0x00;
	PPUMASK = 0x00;
	PPUSTATUS = PPUSTATUS & 0x80;
	// OAMADDR unchanged
	PPUSCROLL = 0x00;
	// PPUADDR unchanged

	writeLatch = FIRST_WRITE;
	isFrameOdd = false;
}

void NES_PPU::powerup() {
	PPUCTRL = 0x00;
	PPUMASK = 0x00;
	PPUSTATUS = 0xA0;
	OAMADDR = 0X00;
	PPUSCROLL = 0x00;
	PPUADDR = 0x00;

	writeLatch = FIRST_WRITE;
	isFrameOdd = false;
}

void NES_PPU::runPPUCycle() {

	// Skip cycle 0 if odd frame and on prerender line
	if (scanlineNum == 261 && ppuDot == 340 && IS_RENDERING_ENABLED(PPUMASK) && isFrameOdd) {
		scanlineNum = 0;
		ppuDot = 0;
		isFrameOdd = !isFrameOdd;
		frameCount++;
	}

	if (scanlineNum < 240) renderScanLines();
	else if (scanlineNum == 261) preRenderScanlLine();
	else vblankScanLines();
	
	ppuDot++;

	// Next scanline
	if (ppuDot > 340) {
		ppuDot = 0;
		scanlineNum++;
	}

	// End of frame
	if (scanlineNum > 261) {
		scanlineNum = 0;
		isFrameOdd = !isFrameOdd;
		frameCount++;
	}

	ppuCycle++;
}

void NES_PPU::renderScanLines() {
	if ((ppuDot <= 256 || (ppuDot >= 321) && ppuDot <= 336) && ppuDot > 0) {
		if (ppuDot % 8 == 0) incrementCoarseX();
		if (ppuDot % 8 == 1) fetchPixels();
		if (ppuDot == 256) {
			incrementCoarseY();
		}
	}
	else if (ppuDot == 257) {
		V = (V & 0xFFE0) | (T & 0x001F);
		OAMADDR = 0;
	}
	else if (ppuDot > 257 && ppuDot <= 320) {
		OAMADDR = 0;
	}

	if (ppuDot <= 256 && ppuDot > 0) {
		screen[ppuDot - 1 + scanlineNum * 256] = pixelsToRender[(currentPixel + 2) % 3 * 8 + (ppuDot - 1) % 8];
	}
}

void NES_PPU::preRenderScanlLine() {

	if (ppuDot == 1) {
		// Clear vblank, sprite 0 hit and sprite overflow
		PPUSTATUS &= ~0xE0;
		nes_cpu->clearNMI();
	}
	else if (ppuDot < 256 && ppuDot > 0) {
		incrementCoarseX();
	}
	else if (ppuDot == 256) {
		incrementCoarseY();
	}
	// Copy coarse and x nametable from T to V
	else if (ppuDot == 257) {
		V = (V & 0xfBE0) | (T & 0x41F);
	}
	// Copy coarse Y and y nametable from T to V
	else if (ppuDot >= 280 && ppuDot <= 304 && IS_RENDERING_ENABLED(PPUMASK)) {
		V = (V & 0x41F) | (T & 0x7BE0);
	}
	else if (ppuDot >= 321 && ppuDot <= 336) {
		if (ppuDot % 8 == 0) incrementCoarseX();
		if (ppuDot % 8 == 1) {
			fetchPixels();
		}
	}
}

void NES_PPU::vblankScanLines() {
	if (scanlineNum == 240 && ppuDot == 0) render::renderScreen(screen, correctPeek(0x3f00));

	// Set VBLANK
	else if (scanlineNum == 241 && ppuDot == 1) {
		// If PPUSTATUS was read one cycle before, do not set VBLANK flag
		/*if (PPUSTATUS_read) {
			PPUSTATUS_read = false;
			return;
		}*/

		PPUSTATUS |= VBLANK;
		NES_PPUdebug::logVBLANKSet();

		if (PPUCTRL & 0x80) {
			nes_cpu->setNMI();
		}
	}
}

void NES_PPU::fetchPixels() {
	// Get nametable Byte
	Word nametableAddress = 0x2000 + (V & 0x0FFF);
	Byte nametableByte = correctPeek(nametableAddress);

	// Get both planes of all pixels in one row of tile
	int tileOffset = nametableByte * 16;
	int pageOffset = (PPUCTRL & BACKGROUND_RENDERING) * 0x1000;
	Word patternTableAddress = tileOffset + pageOffset + EXTRACT_FINE_Y(V);
	Byte loByte = correctPeek(patternTableAddress);
	Byte hiByte = correctPeek(patternTableAddress + 8);

	int indexToEdit = (currentPixel + 2) % 3;

	// Add each colour to array
	for (int i = 0; i < 8; i++) {
		int colourOffset = (loByte >> 7) + ((hiByte & 128) >> 6);

		if (colourOffset == 0) {
			pixelsToRender[indexToEdit * 8 + i] = -1;
		}
		else {
			Word attributeAddress = 0x23C0 | (V & 0x0C00) | ((V >> 4) & 0x38) | ((V >> 2) & 0x07);
			Byte attribute = correctPeek(attributeAddress);

			int coarseX = V & 0x001F;
			int coarseY = (V & 0x03E0) >> 5;

			// Determines whhich bits are put in bit position 0 and 1
			int shift = ((coarseY % 4) / 2) * 4 + ((coarseX % 4) / 2) * 2;

			// Get correct palette index
			int paletteOffset = (attribute >> shift) & 0x03;
			paletteOffset *= 4; // Get correct palette offset

			int colour = correctPeek(0x3F00 + paletteOffset + colourOffset);

			pixelsToRender[indexToEdit * 8 + i] = colour;
		}

		loByte <<= 1;
		hiByte <<= 1;
	}

	currentPixel = (currentPixel + 1) % 3;
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
		if (buffer[5] == 0) {
			std::cout << "There is no CHR ROM. CHR RAM is used\n";
			return true;
		}

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

Byte NES_PPU::correctPeek(Word address) const {

	if (isAddressInRangeInclusive(address, 0x2000, 0x3000)) {
		return VRAM[address % 3840 + 0x2000];
	}

	if (isAddressInRangeInclusive(address, 0x3000, 0X3FFF)) {
		return VRAM[address % 32 + 0x3F00];
	}

	return VRAM[address];
}

void NES_PPU::correctSet(Word address, Byte data) {
	if (isAddressInRangeInclusive(address, 0x3000, 0x3EFF)) {
		VRAM[address % 3840 + 0x2000] = data;
		return;
	}

	if (isAddressInRangeInclusive(address, 0x3000, 0X3FFF)) {
		VRAM[address % 32 + 0x3F00] = data;
		return;
	}

	VRAM[address] = data;
}

void NES_PPU::CpuPpuLatchwrite(Byte data) {
	CpuPpuLatch = data;
}

Byte NES_PPU::CpuPpuLatchRead()  const {
	return CpuPpuLatch;
}

void NES_PPU::PPUCTRLwrite(Byte data) {

	// Rising edge of NMI enable bit triggers NMI if VBLANK is set
	if (data & 0x80 && !(PPUCTRL & 0x80) && PPUSTATUS & 0x80) {
		nes_cpu->setNMI();
	}

	PPUCTRL = data;
	CpuPpuLatch = data;

	data &= 0x03;
	T = (T & 0xF3FF) | (data << 10);
}

void NES_PPU::PPUMASKwrite(Byte data) {

	PPUMASK = data;
	CpuPpuLatch = data;
}

Byte NES_PPU::PPUSTATUSread() {
	writeLatch = FIRST_WRITE;
	Byte data = PPUSTATUS;

	/*if (ppuDot == 1 && scanlineNum == 241) {
		data &= 0x7F;
		nes_cpu->clearNMI();
	}*/

	// Clear vblank flag
	PPUSTATUS &= 0x7F;
	NES_PPUdebug::logVBLANKClear(); // TODO remove debug

	PPUSTATUS_read = true;

	

	return data;
}

void NES_PPU::OAMADDRwrite(Byte data) {
	OAMADDR = data;
	CpuPpuLatch = data;
}

void NES_PPU::OAMDATAwrite(Byte data) {
	OAM[OAMADDR] = data;
	CpuPpuLatch = data;
	OAMADDR++;
}

Byte NES_PPU::OAMDATAread() {
	Byte data = OAM[OAMADDR];
	OAMADDR++;
	return data;
}

void NES_PPU::PPUSCROLLwrite(Byte data) {
	if (writeLatch == FIRST_WRITE) {
		// First write: horizontal scroll
		X = data & 0x07; // fine X scroll (3 bits)
		T = (T & 0xFFE0) | ((data & 0xF8) >> 3); // coarse X (5 bits)
	}
	else {
		// Second write: vertical scroll
		T = (T & 0x8C1F) | // keep nametable and coarse X
			((data & 0xF8) << 2) | // coarse Y (5 bits)
			((data & 0x07) << 12); // fine Y (3 bits)
	}
	writeLatch = !writeLatch;
	CpuPpuLatch = data;
}

void NES_PPU::PPUADDRwrite(Byte data) {
	if (writeLatch == FIRST_WRITE) {
		PPUADDR = (PPUADDR & 0x00FF) | ((data & 0x3F) << 8); // REMOVE 2 MSB since they are irrelevant
		int CDEFGH = data & 0x3F;
		T = (T & 0x00FF) | (CDEFGH << 8);
	}
	else {
		PPUADDR = (PPUADDR & 0xFF00) | data;
		T = (T & 0xFF00) | (data);
		V = T;
	}
	writeLatch = !writeLatch;

	CpuPpuLatch = data;
}

void NES_PPU::PPUDATAwrite(Byte data) {
	correctSet(PPUADDR, data);

	// Bit 2 of PPUCTRL determines how much to increment PPUADDR by after writes
	if (PPUCTRL & 4) PPUADDR += 32;
	else PPUADDR++;

	if (PPUADDR >= 0X3FFe) {
		V = V;
	}

	if (PPUADDR >= 0x4000) {
		PPUADDR %= 0x4000;
	}

	CpuPpuLatch = data;
}

Byte NES_PPU::PPUDATAread() {
	Byte data = CpuPpuLatch;
	CpuPpuLatch = correctPeek(PPUADDR);

	// Bit 2 of PPUCTRL determines how much to increment PPUADDR by after writes
	if (PPUCTRL & 4) PPUADDR += 32;
	else PPUADDR++;
	PPUADDR %= 0x4000;

	if (PPUADDR >= 0x3F00) return CpuPpuLatch;
	return data;
}

void NES_PPU::OAMDMAwrite(Byte data) {
	OAMDMA = data;
	CpuPpuLatch = data;
}

long NES_PPU::getPPUCycle() const {
	return ppuCycle;
}

int NES_PPU::getPPUScanline() const {
	return scanlineNum;
}

int NES_PPU::getPPUDot() const {
	return ppuDot;
}

int NES_PPU::getFrameCount() const {
	return frameCount;
}

void NES_PPU::incrementCoarseX() {
	if ((V & 0x001F) == 31) { // if coarse X == 31
		V &= ~0x001F; // coarse X = 0
		V ^= 0x0400; // switch horizontal nametable
	}
	else {
		V += 1; // coarse X++
	}
}

void NES_PPU::incrementCoarseY() {
	if ((V & 0x7000) != 0x7000) { // if fine Y < 7
		V += 0x1000; // increment fine Y
	}
	else {
		V &= ~0x7000; // fine Y = 0
		int y = (V & 0x03E0) >> 5; // coarse Y

		if (y == 29) {
			y = 0;
			V ^= 0x0800; // switch vertical nametable
		}
		else if (y == 31) {
			y = 0; // coarse Y wraps around without changing NT
		}
		else {
			y += 1;
		}
		V = (V & ~0x03E0) | (y << 5);
	}
}

void NES_PPU::drawPatternTables() {
	for (int i = 0; i < 255; i++) {
		drawTile(i, 0);
		drawTile(i, 1);
	}

	render::renderScreen(screen, correctPeek(0x3f00));
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

				screen[j + i * 256 + tileX + tileY + tableNum * 128] = 0x15;
			}
			int b = j + i * 256 + tileX + tileY;
			loByte <<= 1;
			hiByte <<= 1;
		}
	}
}

Byte NES_PPU::getPPUCTRL() const { return PPUCTRL; }
Byte NES_PPU::getPPUMASK() const { return PPUMASK; }
Byte NES_PPU::getPPUSTATUS() const { return PPUSTATUS; }
Word NES_PPU::getPPUADDR() const { return PPUADDR; }
Word NES_PPU::getPPUSCROLL() const { return PPUSCROLL; }
Word NES_PPU::getV() const { return V; }
Word NES_PPU::getT() const { return T; }
Byte NES_PPU::getX() const { return X; }
bool NES_PPU::getWriteLatch() const { return writeLatch; }
bool NES_PPU::getIsFrameOdd() const { return isFrameOdd; }





