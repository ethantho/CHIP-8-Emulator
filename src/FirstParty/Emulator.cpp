#include "Emulator.h"


void Emulator::GameLoop(std::string& program_file_name)
{
	Init(program_file_name);

    while (!quit) {

        Input();

        Update();

        Render();
    }
}

void Emulator::Init(std::string& program_file_name)
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

    //initialize memory
    for (int i = 0; i < MEMORY_SIZE; ++i) {
        memory[i] = 0x00;
    }

    LoadProgram(program_file_name);

    //initialize display
    for (int i = 0; i < LOGICAL_WIDTH; ++i) {

        for (int j = 0; j < LOGICAL_HEIGHT; ++j) {

            pixels[i][j] = false;

        }
    }

    //load fonts at 050–09F
    for (int i = 0; i < 80; ++i) {
        memory[i + FONT_START] = font_data[i];
    }

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
    for (int x = 0; x < LOGICAL_WIDTH; ++x) {
        for (int y = 0; y < LOGICAL_HEIGHT; ++y) {
            if (pixels[x][y]) {
                int i = SDL_RenderDrawPoint(renderer, x, y);
            }
        }
    }

    SDL_RenderPresent(renderer);
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
    unsigned char first_nibble = (instruction & 0xF000) >> 12;
    unsigned char X = (instruction & 0x0F00) >> 8;
    unsigned char Y = (instruction & 0x00F0) >> 4;
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
        if (NNN == 0x0EE) {
            //return after subroutine
            program_counter = stack.top();
            stack.pop();
        }
        break;
    case 0x1:
        program_counter = NNN;
        break;
    case 0x2:
        //subroutine
        stack.push(program_counter);
        program_counter = NNN;
        break;
    case 0x3:
        //skip if value in VX is equal to NN
        if (variable_registers[X] == NN) {
            program_counter += 2;
        }
        break;
    case 0x4:
        if (variable_registers[X] != NN) {
            program_counter += 2;
        }
        break;
    case 0x5:
        if (variable_registers[X] == variable_registers[Y]) {
            program_counter += 2;
        }
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
        switch (N) {
        case 0:
            variable_registers[X] = variable_registers[Y];
            break;
        case 1:
            variable_registers[X] = variable_registers[X] | variable_registers[Y];
            break;
        case 2:
            variable_registers[X] = variable_registers[X] & variable_registers[Y];
            break;
        case 3:
            variable_registers[X] = variable_registers[X] ^ variable_registers[Y];
            break;
        case 4: {
            int overflow_check = static_cast<int>(variable_registers[X]) + static_cast<int>(variable_registers[Y]);
            
            variable_registers[X] = variable_registers[X] + variable_registers[Y];

            variable_registers[0xf] = 0;
            if (overflow_check > 255) {
                variable_registers[0xf] = 1;
            }
            break;
        }
        case 5: {
            bool check = variable_registers[X] < variable_registers[Y];
            variable_registers[X] = variable_registers[X] - variable_registers[Y];
            variable_registers[0xf] = 1;
            if (check) variable_registers[0xf] = 0;
            break;
        }
        case 6: {
            if (SHIFT_OPTIONAL_STEP) variable_registers[X] = variable_registers[Y];
            int flag = variable_registers[X] & 0x1;
            variable_registers[X] = variable_registers[X] >> 1;
            variable_registers[0xf] = flag;
            break;
        }
        case 7: {
            bool check = variable_registers[X] > variable_registers[Y];
            variable_registers[X] = variable_registers[Y] - variable_registers[X];
            variable_registers[0xf] = 1;
            if (check) variable_registers[0xf] = 0;
            break;
        }
        case 0xe: {
            if (SHIFT_OPTIONAL_STEP) variable_registers[X] = variable_registers[Y];
            int flag = (variable_registers[X] & 0b10000000) >> 7;
            variable_registers[X] = variable_registers[X] << 1;
            variable_registers[0xf] = flag;
            break;
        }  
        }
        break;
    case 0x9:
        if (variable_registers[X] != variable_registers[Y]) {
            program_counter += 2;
        }
        break;
    case 0xa:
        //set index
        index_register = NNN;
        break;
    case 0xb:
        //jump with offset
        if(!JUMP_WITH_OFFSET_NEW) program_counter = NNN + variable_registers[0];
        else { program_counter = NNN + variable_registers[X]; }
        break;
    case 0xc:
        //random
        variable_registers[X] = rand() & NN;
        break;
    case 0xd: {
        //display
        int x_coord = variable_registers[X] % 64;
        int y_coord = variable_registers[Y] % 32;
        variable_registers[0xf] = 0;
        for (int row = 0; row < N; ++row) {

            unsigned char current_byte = memory[row + index_register];

            for (int bit = 0; bit < 8; ++bit) {

                bool check = (current_byte >> (7 - bit)) & 1;

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
        if (NN == 0x9E) {
            if (keys[X]) program_counter += 2;
            break;
        }
        if (NN == 0xA1) {
            if (!keys[X]) program_counter += 2;
            break;
        }
        break;
    case 0xf:
        switch (NN) {
        case 0x07:
            variable_registers[X] = delay_timer;
            break;
        case 0x15:
            delay_timer = variable_registers[X];
            break;
        case 0x18:
            sound_timer = variable_registers[X];
            break;
        case 0x1e:
            //add index
            if (ADD_INDEX_OVERFLOW && static_cast<int>(index_register) + static_cast<int>(variable_registers[X]) > 0xfff) variable_registers[0xf] = 1;
            index_register += variable_registers[X];
            break;
        case 0x0A: {
            //wait for key
            bool pressed = false;
            for (uint8_t i = 0; i < 16; ++i) {
                if (keys[i]) {
                    pressed = true;
                    variable_registers[X] = i;
                }
            }
            if (!pressed) program_counter -= 2;
            break;
        }
        case 0x29:
            //font character
            index_register = FONT_START + (variable_registers[X] * 5);
            break;
        case 0x33: {
            uint8_t num = variable_registers[X];
            uint8_t digit_3 = num % 10;
            num /= 10;
            uint8_t digit_2 = num % 10;
            num /= 10;
            uint8_t digit_1 = num % 10;
            memory[index_register] = digit_1;
            memory[index_register + 1] = digit_2;
            memory[index_register + 2] = digit_3;
            break;
        }
        case 0x55:
            //store memory
            for (int i = 0; i <= X; ++i) {
                memory[index_register + i] = variable_registers[i];
            }
            break;
        case 0x65:
            //load memory
            for (int i = 0; i <= X; ++i) {
                variable_registers[i] = memory[index_register + i];
            }
            break;
        }
    

    }



}

void Emulator::Execute()
{
}

void Emulator::LoadProgram(std::string& program_file_name)
{
    std::cout << "Loading " << program_file_name << std::endl;

    std::ifstream program_file(program_file_name, std::fstream::binary);

    if (!program_file.is_open()) {
        std::cerr << "Error opening " << program_file_name << std::endl;
        exit(1);
    }

    
    uintmax_t file_length{ std::filesystem::file_size(program_file_name) };
    //std::vector<unsigned char> holder(file_length);

    program_file.read(reinterpret_cast<char*>(memory + 0x200), file_length);

}
