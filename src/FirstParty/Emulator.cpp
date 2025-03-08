#include "Emulator.h"


void Emulator::GameLoop()
{
	Init();

    while (!quit) {
        Update();
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
    
    //Get window surface
    surface = SDL_GetWindowSurface(window);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

    SDL_RenderSetLogicalSize(renderer, LOGICAL_WIDTH, LOGICAL_HEIGHT);
}

void Emulator::Update()
{
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        //If event is quit type
        if (e.type == SDL_QUIT)
        {
            //End the main loop
            quit = true;
        }
    }
}
