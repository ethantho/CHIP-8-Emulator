#ifndef EMULATOR_H

#include <string>
#include <stack>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <vector>
#include <cstdlib>

#define SDL_MAIN_HANDLED
#include "SDL.h"

class Emulator
{
public:

	Emulator(){}

	void GameLoop(std::string& program_file_name);

private:

	const int LOGICAL_WIDTH = 64;
	const int LOGICAL_HEIGHT = 32;

	const int WINDOW_WIDTH = 640;
	const int WINDOW_HEIGHT = 320;

	const int CYCLES_PER_FRAME = 500;

	const bool SHIFT_OPTIONAL_STEP = true;
	const bool JUMP_WITH_OFFSET_NEW = false;
	const bool ADD_INDEX_OVERFLOW = false;

	const uint8_t FONT_START = 0x050;

	SDL_Window* window = nullptr;
	SDL_Surface* surface = nullptr;
	SDL_Renderer* renderer = nullptr;

	bool quit = false;

	void Init(std::string& program_file_name);
	void Update();
	void Input();
	void Render();

	void Fetch();
	void Decode();
	void Execute();

	void LoadProgram(std::string& program_file_name);

	const int MEMORY_SIZE = 4096;
	unsigned char memory[4096];

	bool pixels[64][32];

	int program_counter = 0x200;

	uint16_t index_register = 0;

	std::stack<uint16_t> stack;

	uint8_t delay_timer = 0;

	uint8_t sound_timer = 0;

	uint8_t variable_registers[16];

	////////////////FONTS////////////////
	unsigned char font_data[80] = { 0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80 };  // F

	bool keys[16];

	uint16_t instruction;

};

#endif // !EMULATOR_H

