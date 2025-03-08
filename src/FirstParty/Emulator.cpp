#include "Emulator.h"


void Emulator::GameLoop()
{
	Init();

    while (!quit) {

        Input();

        Update();

        Render();
    }
}

void Emulator::Init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        SDL_Log("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(0);
    }

    window = SDL_CreateWindow("CHIP-8 Emulator", 100, 100, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

    if (window == nullptr)
    {
        SDL_Log("Window could not be created! SDL error: %s\n", SDL_GetError());
        exit(0);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

    SDL_RenderSetLogicalSize(renderer, LOGICAL_WIDTH, LOGICAL_HEIGHT);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    //int i = SDL_RenderDrawPoint(renderer, LOGICAL_WIDTH/2, LOGICAL_HEIGHT/2);

    //SDL_RenderPresent(renderer);

    //initialize memory
    for (int i = 0; i < MEMORY_SIZE; ++i) {
        memory[i] = 0x00;
    }

    //initialize display
    for (int i = 0; i < LOGICAL_WIDTH; ++i) {

        for (int j = 0; j < LOGICAL_HEIGHT; ++j) {

            pixels[i][j] = false;

        }
    }

    //load fonts at 050–09F
    for (int i = 0; i < 80; ++i) {
        memory[i + 0x050] = font_data[i];
    }

    //initialize input
    


}

void Emulator::Update()
{
    for (int i = 0; i < CYCLES_PER_FRAME; ++i) {
        Fetch();
        Decode();
        Execute();
    }

    if (delay_timer > 0) delay_timer--;
    if (sound_timer > 0) sound_timer--;

}

void Emulator::Input()
{

    for (int i = 0; i < 16; ++i) {
        keys[i] = false;
    }

    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        //If event is quit type
        if (e.type == SDL_QUIT)
        {
            //End the main loop
            quit = true;
        }

        if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.scancode) {
            case SDL_SCANCODE_1:
                keys[0] = true;
                break;
            case SDL_SCANCODE_2:
                keys[1] = true;
                break;
            case SDL_SCANCODE_3:
                keys[2] = true;
                break;
            case SDL_SCANCODE_4:
                keys[3] = true;
                break;
            case SDL_SCANCODE_Q:
                keys[4] = true;
                break;
            case SDL_SCANCODE_W:
                keys[5] = true;
                break;
            case SDL_SCANCODE_E:
                keys[6] = true;
                break;
            case SDL_SCANCODE_R:
                keys[7] = true;
                break;
            case SDL_SCANCODE_A:
                keys[8] = true;
                break;
            case SDL_SCANCODE_S:
                keys[9] = true;
                break;
            case SDL_SCANCODE_D:
                keys[10] = true;
                break;
            case SDL_SCANCODE_F:
                keys[11] = true;
                break;
            case SDL_SCANCODE_Z:
                keys[12] = true;
                break;
            case SDL_SCANCODE_X:
                keys[13] = true;
                break;
            case SDL_SCANCODE_C:
                keys[14] = true;
                break;
            case SDL_SCANCODE_V:
                keys[15] = true;
                break;
            }

        }
    }

}

void Emulator::Render()
{
}

void Emulator::Fetch()
{
    program_counter %= MEMORY_SIZE;
    unsigned char first_byte = memory[program_counter];
    unsigned char second_byte = memory[program_counter+1];

    instruction = first_byte;
    instruction = instruction << 8;
    instruction |= second_byte;

    program_counter += 2;

}

void Emulator::Decode()
{
    unsigned char first_nibble = (instruction & 0xF000) >> 24;
    unsigned char X = (instruction & 0x0F00) >> 16;
    unsigned char Y = (instruction & 0x00F0) >> 8;
    unsigned char N = instruction & 0x000F;

    unsigned char NN = instruction & 0x00FF;
    unsigned short NNN = instruction & 0x0FFF;

    //TODO: USE #DEFINE INSTEAD?

    switch (first_nibble) {
    case 0x0:
        if (NNN == 0x0E0) {
            //clear screen
            for (int i = 0; i < LOGICAL_WIDTH; ++i) { for (int j = 0; j < LOGICAL_HEIGHT; ++j) { pixels[i][j] = false; } }
            break;
        }
        break;
    case 0x1:
        if (NNN == 0x111) {
            //jump
            program_counter = NNN;
            break;
        }
        break;
    case 0x2:
        break;
    case 0x3:
        break;
    case 0x4:
        break;
    case 0x5:
        break;
    case 0x6:
        //set
        variable_registers[X] = NN;
        break;
    case 0x7:
        //add
        variable_registers[X] += NN;
        break;
    case 0x8:
        break;
    case 0x9:
        break;
    case 0xa:
        //set index
        index_register = NNN;
        break;
    case 0xb:
        break;
    case 0xc:
        break;
    case 0xd: {
        //display
        int x_coord = variable_registers[X] % 64;
        int y_coord = variable_registers[Y] % 32;
        variable_registers[0xf] = 0;
        for (int row = 0; row < N; ++row) {

            unsigned char current_byte = memory[row + index_register];

            for (int bit = 0; bit < 8; ++bit) {

                bool check = (current_byte >> bit) & 1;

                if (check && pixels[x_coord + bit][y_coord + row]) {

                    pixels[x_coord + bit][y_coord + row] = false;
                    variable_registers[0xf] = 1;

                }
                else if (check) {

                    pixels[x_coord + bit][y_coord + row] = true;

                }
                if (x_coord + bit + 1 == LOGICAL_WIDTH) {
                    break;
                }
            }

            if (y_coord + row + 1 == LOGICAL_HEIGHT) {
                break;
            }
        }
        break;
    }
    case 0xe:
        break;
    case 0xf:
        break;

    }



}

void Emulator::Execute()
{
}
