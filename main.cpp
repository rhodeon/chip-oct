#include <iostream>
#include <SDL2/SDL.h>

#include "chip8.cpp"

void init_window(SDL_Window*& window, SDL_Surface*& surface, int width, int height) {
    window = SDL_CreateWindow("CHIP-Oct", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    surface = SDL_GetWindowSurface(window);
}

void quit(SDL_Window*& window) {
    SDL_DestroyWindow(window);
    window = NULL;
}

int main(int argc, const char* argv[]) {
    SDL_Window* main_window = NULL;
    SDL_Surface* base_surface = NULL;
    
    const int SCREEN_WIDTH = 640;
    const int SCREEN_HEIGHT = 320;

    init_window(main_window, base_surface, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_UpdateWindowSurface(main_window);
    SDL_Delay(2000);
    quit(main_window);

    chip8 game;
    game.initialize();
    game.load_rom(argv[1]);
    

    return 0;
}
