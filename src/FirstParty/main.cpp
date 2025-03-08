
#include <iostream>
#include "Emulator.h"


int main(int argc, char* argv[])
{
    SDL_SetMainReady();

    Emulator e;

    std::string program_file_name = argv[1];

    e.GameLoop(program_file_name);

    return 0;
}
