# NES-Emulator
A work in progress NES Emulator. The project was built as a way to learn more about low-level hardware emulation and to explore how the NES worked under the hood.

## Table of Contents
- [Contact Info](#contact-info)
- [How It's Made](#how-its-made)
  - [Tech Used](#tech-used)
  - [Code](#code)
- [How To Set Up](#how-to-set-up)
- [How To Use](#how-to-use)
- [Upcoming Features](#upcoming-features)
- [Known Issues](#known-issues) 

## Contact Info
Please contact me at leee0000n@hotmail.com if you need help building the project or wish to report a bug.

## How It's Made
### Tech Used
- **Programming Language:** C++
- **Libraries:** SDL2
- **Build System:** CMake

### Code
Currently, the emulator includes:
- A fully implemented and tested Ricoh 2A03 emulator that handles all official instructions, most unofficial instructions (except the unstable ones), memory access, and interrupts.
- A partially implemented PPU that renders background tiles and supports basic scrolling
- A way to load iNES roms.
- Functions for tracing the state of the CPU
- Functions for tracing the state of the PPU and actions it has taken

## How To Set Up
This has only been tested on **Windows 10 and 11** with the **MSVC compiler**. Attempting to run this on another operating system or with another compiler may not work.**CMake** and **Git** are also required.

In Visual Studio, when the project is cloned and first opened, it should set itself up automatically. 
It can also be set up by using the following commands, which are not IDE-dependent.
``` bash
cd <CMakeLists.txt dir goes here>

cmake ..
cmake --build

myproject.exe
```

## How To Use
Currently, it is very barebones and contains no features common in emulators. The emulator only works with iNES ROMs. To change what ROM is being used, navigate to ```App.cpp``` and pass the path to the ROM as an argument to the init function. For example
``` cpp
init("C:/games/pacman.nes");
```

## Upcoming Features
- Implementing various mappers so that later games can be played
- Implementing the APU for sound
- Adding separate UI for things like rebinding inputs, selecting different ROMs, etc

## Known Issues
- No way to change ROMs
- No Input handling
- PPU display is buggy, only displaying every other line
- The NES fails many test ROMs that run on the original NES
- The speed at which it runs is not regulated
- No sprite rendering
