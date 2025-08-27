#pragma once

#include "Constants.h"

#include <array>


class NES_APU {
private:
    // APU registers (0x4000 - 0x4017)
    std::array<Byte, 0x18> registers;

    // Frame sequencer state
    Word frameSequencerCounter = 0;
    Byte frameSequencerStep = 0;

    // Frame interrupt flag
    bool frameIRQ = false;

    // Internal methods for channel clocking (to be implemented)
    void clockEnvelope();
    void clockLengthCounters();
    void clockSweepUnits();

public:
    NES_APU();

	void powerUp();
    
	void reset();

    void writeRegister(Word address, Byte data);

    // Read from APU register (0x4000 - 0x4017)
    Byte readRegister(Word address);

    // Clock the APU (called every CPU cycle)
    void runAPUCycle();

    // Step the frame sequencer
    void stepFrameSequencer();
};

extern NES_APU* apu;