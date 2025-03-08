
#include <iostream>
#include "Emulator.h"


int main(int argc, char* argv[])
{
    SDL_SetMainReady();

    Emulator e;

    e.GameLoop();

    return 0;
}
