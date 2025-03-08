#ifndef EMULATOR_H

#include <cstddef>
#include <string>

#define SDL_MAIN_HANDLED
#include "SDL.h"

class Emulator
{
public:

	Emulator(){}

	void GameLoop();

private:

	const int LOGICAL_WIDTH = 64;
	const int LOGICAL_HEIGHT = 32;

	const int WINDOW_WIDTH = 640;
	const int WINDOW_HEIGHT = 320;

	SDL_Window* window = nullptr;
	SDL_Surface* surface = nullptr;
	SDL_Renderer* renderer = nullptr;

	bool quit = false;

	void Init();
	void Update();

	//std::byte memory[4096];

};

#endif // !EMULATOR_H

