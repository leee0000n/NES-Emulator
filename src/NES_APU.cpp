#include "NES_APU.h"

#include <SDL_audio.h>
#include <random>

struct AudioData {
    float phase;
    float phaseIncrement;
};

void audioCallback(void* userdata, Uint8* stream, int len) {
    AudioData* data = static_cast<AudioData*>(userdata);
    float* buffer = reinterpret_cast<float*>(stream);
    int samples = len / sizeof(float);

    for (int i = 0; i < samples; i += 2) { // Stereo: left and right
        float sample;
    }
}

NES_APU::NES_APU() {
	registers.fill(0);
	frameSequencerCounter = 0;
	frameSequencerStep = 0;
	frameIRQ = false;
}

void NES_APU::clockEnvelope() {
	
}

void NES_APU::clockLengthCounters() {
	
}

void NES_APU::clockSweepUnits() {
	
}

// Write to APU register (0x4000 - 0x4017)
void NES_APU::writeRegister(Word address, Byte data) {
	registers[address] = data;
}

// Read from APU register (0x4000 - 0x4017)
Byte NES_APU::readRegister(Word address) {
	if (address == 0x4015) return registers[address];
	return rand(); // Return random data for other registers

}


// TODO IMPLEMENT 5 STEP SEQUENCE OPTION
void NES_APU::runAPUCycle() {
    // The APU frame sequencer runs at half the CPU runAPUCycle (NTSC: 1789773/2 Hz)
    // Increment the frame sequencer counter
    frameSequencerCounter++;
    // NTSC: Frame sequencer steps every 7457 CPU cycles (approx)
    // 4-step sequence: 7457, 14913, 22371, 29829
    // 5-step sequence: 7457, 14913, 22371, 29829, 37281
    // We'll use 4-step mode for this basic implementation
    constexpr uint16_t FRAME_SEQUENCER_PERIOD = 7457;

    if (frameSequencerCounter >= FRAME_SEQUENCER_PERIOD) {
        frameSequencerCounter = 0;
        stepFrameSequencer();
    }

    // TODO: Clock individual channels here (pulse, triangle, noise, DMC)	
}

// Step the frame sequencer
void NES_APU::stepFrameSequencer() {
    // 4-step sequence: 0, 1, 2, 3
    // Steps: 0 - envelope & linear, 1 - length & sweep, 2 - envelope & linear, 3 - length & sweep + IRQ
    frameSequencerStep = (frameSequencerStep + 1) % 4;

    switch (frameSequencerStep) {
    case 0:
    case 2:
        // Clock envelope and linear counter
        clockEnvelope();
        break;
    case 1:
    case 3:
        // Clock length counters and sweep units
        clockLengthCounters();
        clockSweepUnits();
        if (frameSequencerStep == 3) {
            // Set frame IRQ flag (if not inhibited)
            if (!(registers[0x17] & 0x40)) { // Bit 6: inhibit frame IRQ
                frameIRQ = true;
            }
        }
        break;
    }
}

// Reset the APU
void NES_APU::reset() {
	registers.fill(0);
	frameSequencerCounter = 0;
	frameSequencerStep = 0;
	frameIRQ = false;
	registers[APU_STATUS_REGISTER_ADDRESS] = 0;
}