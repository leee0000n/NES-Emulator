#include "Render.h"

// todo implement cmake auto imports
#include <SDL.h>
#include <stdio.h>

SDL_Window* gWindow;
SDL_Renderer* gRenderer;

/// Converts nes colour to rgb colour
/// @param colour nes colour
void setPixelColour(int colour) {
	SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, 255);
}

void render::renderScreen(std::array<int, 256 * 240> screen) {
	for (int i = 0; i < 61440; i++) {
		const int x = i % 256;
		const int y = i / 256;
		const int colour = screen[i];

		setPixelColour(colour);
		SDL_RenderDrawPoint(gRenderer, x, y);
	}
}

bool render::initSDL2() {
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else {
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL) {
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else {
			//Create vsynced renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (gRenderer == NULL) {
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else {
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
			}
		}
	}

	return success;
}

void render::closeSDL2() {
	//Destroy window    
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}


