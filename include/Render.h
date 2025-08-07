#pragma once

#include <array>

constexpr int SCREEN_WIDTH = 256;
constexpr int SCREEN_HEIGHT = 240;

namespace render {
	bool initSDL2();
	void closeSDL2();
	void renderScreen(const std::array<int, 256 * 256>& screen);

	extern std::array<int, 64> nesPaletteAsRGB;
}

