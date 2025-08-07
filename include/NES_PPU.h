#pragma once

#include <array>
#include <string>

using Byte = unsigned char;
using Word = unsigned short;

constexpr bool FIRST_WRITE = true;
constexpr bool SECOND_WRITE = false;

constexpr bool EVEN_FRAME = false;
constexpr bool ODD_FRAME = true;

constexpr int VBLANK = 0x80;

constexpr int CHOSEN_PATTERN_TABLE = 0x08;

struct Tile {
	std::array<Byte, 16> tile8x8;
};

/* PPUCTRL
VPHB SINN
|||| ||||
|||| ||++- Base nametable address
|||| ||    (0 = $2000; 1 = $2400; 2 = $2800; 3 = $2C00)
|||| |+--- VRAM address increment per CPU read/write of PPUDATA
|||| |     (0: add 1, going across; 1: add 32, going down)
|||| +---- Sprite pattern table address for 8x8 sprites
||||       (0: $0000; 1: $1000; ignored in 8x16 mode)
|||+------ Background pattern table address (0: $0000; 1: $1000)
||+------- Sprite size (0: 8x8 pixels; 1: 8x16 pixels  see PPU OAM#Byte 1)
|+-------- PPU master/slave select
|          (0: read backdrop from EXT pins; 1: output color on EXT pins)
+--------- Vblank NMI enable (0: off, 1: on)
*/

/* PPUMASK
BGRs bMmG
|||| ||||
|||| |||+- Greyscale (0: normal color, 1: greyscale)
|||| ||+-- 1: Show background in leftmost 8 pixels of screen, 0: Hide
|||| |+--- 1: Show sprites in leftmost 8 pixels of screen, 0: Hide
|||| +---- 1: Enable background rendering
|||+------ 1: Enable sprite rendering
||+------- Emphasize red (green on PAL/Dendy)
|+-------- Emphasize green (red on PAL/Dendy)
+--------- Emphasize blue
*/

/* PPUSTATUS
VSOx xxxx
|||| ||||
|||+-++++- (PPU open bus or 2C05 PPU identifier)
||+------- Sprite overflow flag
|+-------- Sprite 0 hit flag
+--------- Vblank flag, cleared on read. Unreliable; see below.
*/

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
	/// CPU can read or write
	/// 1 byte of data
	/// data read from or written to OAM address in OAMADDR register
	/// </summary>
	Byte OAMDATA;

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
	/// CPU can read and write
	/// 1 byte of data
	/// data read from or written to ppu memory
	/// </summary>
	Byte PPUDATA;

	/// <summary>
	/// CPU can write only
	/// High byte of address
	/// DMA suspends CPU and copies entire page into OAM
	/// </summary>
	Byte OAMDMA;

	Word V;

	Word T;
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
	bool frameType;

	int scanlineNum;
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

	std::array<int, 256 * 256> screen;

public:

	NES_PPU();

	void reset();

	void powerup();

	/// <summary>
	/// run 3 ppu cycles. called from NES_CPU::RUN every cpu
	/// cycle
	/// </summary>
	void run();

	void addToScreen(Byte loByte, Byte hiByte, Byte attribyte, int paletteChoice);

	void default2c02Palette();

	bool loadCHRROM(std::string path);
	bool loadPalFile(std::string path);

	Byte correctPeek(Word address);

	void CpuPpuLatchwrite(Byte data);
	Byte CpuPpuLatchRead() const;

	void PPUCTRLwrite(Byte data);

	void PPUMASKwrite(Byte data);

	Byte PPUSTATUSread() const;

	void OAMADDRwrite(Byte data);

	void OAMDATAwrite(Byte data);
	Byte OAMDATAread() const;

	void PPUSCROLLwrite(Byte data);

	void PPUADDRwrite(Byte data);

	void PPUDATAwrite(Byte data);
	Byte PPUDATAread() const;

	void OAMDMAwrite(Byte data);

	int getPPUCycle();
	int getPPUScanline();

	void drawPatternTables();
	void drawTile(int tileIndex, int tableNum);
};

extern NES_PPU* ppu;
