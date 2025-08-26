#pragma once

#include "Constants.h"

#include <array>
#include <string>


class NES_PPU {
	/// <summary>
	/// CPU can write only
	/// VPHB SINN
	/// NMI enable (V), PPU master/slave (P), sprite height (H), background tile select (B), 
	/// sprite tile select (S), increment mode (I), nametable select / X and Y scroll bit 8 (NN)
	/// </summary>
	Byte PPUCTRL;

	/// <summary>
	/// CPU can write only
	/// BGRs bMmG
	/// color emphasis (BGR), sprite enable (s), background enable (b), sprite left column enable (M), #
	/// background left column enable (m), greyscale (G)
	/// </summary>
	Byte PPUMASK;

	/// <summary>
	/// CPU can read only
	/// VSO- ----	
	/// vblank (V), sprite 0 hit (S), sprite overflow (O); 
	/// reading this register sets the write toggle to first write (false)
	/// </summary>
	Byte PPUSTATUS;

	/// <summary>
	/// CPU can write only
	/// 1 byte address
	/// Determines where in OAM to read or write from
	/// </summary>
	Byte OAMADDR;

	/// <summary>
	/// CPU can write only. writes twice
	/// XXXX XXXX YYYY YYYY
	/// X byte represents X scroll
	/// Y byte represent Y scroll
	/// writing toggles writeLatch
	/// </summary>
	Word PPUSCROLL;

	/// <summary>
	/// CPU cane write only. writes twice
	/// --AA AAAA AAAA AAAA
	/// 2 most significant bits are irrelevant since PPU only
	/// has a 14 bit address space
	/// </summary>
	Word PPUADDR;

	/// <summary>
	/// CPU can write only
	/// High byte of address
	/// DMA suspends CPU and copies entire page into OAM
	/// </summary>
	Byte OAMDMA;

	Word V;

	Word T;

	Byte X;

	/// <summary>
	/// When data is written to ppu registers, bus acts as an 8 bit latch.
	/// Writes to read only registers still update this latch
	/// Reads from write only registers return data stored here
	/// </summary>
	Byte CpuPpuLatch;


	/// <summary>
	/// used when writing to PPUSCROLL and PPUADDR.
	/// false = first write
	/// true = second write
	/// </summary>
	bool writeLatch;

	/// <summary>
	/// Frame type can be either even or odd
	/// false = even
	/// true = odd
	/// </summary>
	bool isFrameOdd;

	int scanlineNum;
	int ppuDot;
	int ppuCycle;


	/// <summary>
	/// Sprite RAM
	/// 64 sprites, each 4 bytes each
	/// </summary>
	std::array<Byte, 256> OAM;

	/// <summary>
	/// Contains 8 sprites that are to be drawn on current scanline
	/// Re fetched each scanline
	/// </summary>
	std::array<Byte, 32> spriteEvalutionBuffer;

	std::array<Byte, 16384> VRAM;

	std::array<int, 65536> screen;

	int currentPixel;
	std::array<int, 24> pixelsToRender;
	

	Byte nextNametableByte;
	Byte nextAttributeByte;
	Byte nextTileLoByte;
	Byte nextTileHiByte;

public:

	NES_PPU();

	void reset();

	void powerup();

	/// <summary>
	/// run one cycle of the PPU
	/// </summary>
	void runPPUCycle();

	void renderScanLines();
	void preRenderScanlLine();
	void vblankScanLines();

	void fetchPixels();

	void default2c02Palette();

	bool loadCHRROM(std::string path);
	bool loadPalFile(std::string path);

	Byte correctPeek(Word address) const;
	void correctSet(Word address, Byte data);

	void CpuPpuLatchwrite(Byte data);
	Byte CpuPpuLatchRead() const;

	void PPUCTRLwrite(Byte data);

	void PPUMASKwrite(Byte data);

	Byte PPUSTATUSread();

	void OAMADDRwrite(Byte data);

	void OAMDATAwrite(Byte data);
	Byte OAMDATAread();

	void PPUSCROLLwrite(Byte data);

	void PPUADDRwrite(Byte data);

	void PPUDATAwrite(Byte data);
	Byte PPUDATAread();

	void OAMDMAwrite(Byte data);

	void incrementCoarseX();
	void incrementCoarseY();


	int getPPUCycle() const;
	int getPPUScanline() const;

	void drawPatternTables();
	void drawTile(int tileIndex, int tableNum);
};

extern NES_PPU* ppu;
