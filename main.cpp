#include <iostream>
#include <SDL2/SDL.h>

#include <chrono>
#include <thread>

#include "chip8.cpp"

enum KEY_PRESSES {
    KEY_PRESS_0,
    KEY_PRESS_1,
    KEY_PRESS_2,
    KEY_PRESS_3,
    KEY_PRESS_4,
    KEY_PRESS_5,
    KEY_PRESS_6,
    KEY_PRESS_7,
    KEY_PRESS_8,
    KEY_PRESS_9,
    KEY_PRESS_A,
    KEY_PRESS_B,
    KEY_PRESS_C,
    KEY_PRESS_D,
    KEY_PRESS_E,
    KEY_PRESS_F,
};

void init_window(SDL_Window*& window, SDL_Surface*& surface, int width, int height) {
    window = SDL_CreateWindow("CHIP-Oct", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    surface = SDL_GetWindowSurface(window);
}

void quit(SDL_Window*& window) {
    SDL_DestroyWindow(window);
    window = NULL;
}

void draw_graphics(chip8& game, SDL_Window*& window, SDL_Surface*& base_surface, int width, int height) {
    SDL_Rect pixel;
    pixel.w = 10;
    pixel.h = 10;
    std::array<unsigned char, 64 * 32> screen = game.display;

    for (int row = 0; row < height; row += 10) {
        for (int column = 0; column < width; column += 10) {
            pixel.x = column;
            pixel.y = row;
            int pos = column/10 + row/10 * 64;
            
            if (screen[pos] == 1) {
                SDL_FillRect(base_surface, &pixel, SDL_MapRGB(base_surface->format, 0xFF, 0xFF, 0xFF));
            }
            else
            {
                SDL_FillRect(base_surface, &pixel, SDL_MapRGB(base_surface->format, 0x00, 0x00, 0x00));
            }
                        
        }
    }
    SDL_UpdateWindowSurface(window);
}

void set_keys(chip8& game, SDL_Event& event) {
    // set key state to 1 if pressed
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_1: 
                game.keyboard[KEY_PRESS_1] = 1;

                std::cout << "" << std::endl; 
                for (auto key : game.keyboard) {
                    std::cout << (int) key << std::endl;
                }
                std::cout << "" << std::endl;

            break;
                 
            case SDLK_2: 
                game.keyboard[KEY_PRESS_2] = 1;
                break;
                
            case SDLK_3: 
                game.keyboard[KEY_PRESS_3] = 1; 
                break;
                
            case SDLK_4: 
                game.keyboard[KEY_PRESS_C] = 1; 
                break;
                
            case SDLK_q: 
                game.keyboard[KEY_PRESS_4] = 1; 
                break;
                
            case SDLK_w: 
                game.keyboard[KEY_PRESS_5] = 1; 
                break;
                
            case SDLK_e: 
                game.keyboard[KEY_PRESS_6] = 1; 
                break;
                
            case SDLK_r: 
                game.keyboard[KEY_PRESS_D] = 1; 
                break;
                
            case SDLK_a: 
                game.keyboard[KEY_PRESS_7] = 1; 
                break;
                
            case SDLK_s: 
                game.keyboard[KEY_PRESS_8] = 1; 
                break;
                
            case SDLK_d: 
                game.keyboard[KEY_PRESS_9] = 1; 
                break;
                
            case SDLK_f: 
                game.keyboard[KEY_PRESS_E] = 1; 
                break;
                
            case SDLK_z: 
                game.keyboard[KEY_PRESS_A] = 1; 
                break;
                
            case SDLK_x: 
                game.keyboard[KEY_PRESS_0] = 1; 
                break;
                
            case SDLK_c: 
                game.keyboard[KEY_PRESS_B] = 1; 
                break;
                
            case SDLK_v: 
                game.keyboard[KEY_PRESS_F] = 1; 
                break;
                
        }
    }

    // set key state to 0 if not pressed
    if (event.type == SDL_KEYUP) {
        switch (event.key.keysym.sym) {
            case SDLK_1: 
                game.keyboard[KEY_PRESS_1] = 0; 
            break;
                 
            case SDLK_2: 
                game.keyboard[KEY_PRESS_2] = 0;
                break;
                
            case SDLK_3: 
                game.keyboard[KEY_PRESS_3] = 0; 
                break;
                
            case SDLK_4: 
                game.keyboard[KEY_PRESS_C] = 0; 
                break;
                
            case SDLK_q: 
                game.keyboard[KEY_PRESS_4] = 0; 
                break;
                
            case SDLK_w: 
                game.keyboard[KEY_PRESS_5] = 0; 
                break;
                
            case SDLK_e: 
                game.keyboard[KEY_PRESS_6] = 0; 
                break;
                
            case SDLK_r: 
                game.keyboard[KEY_PRESS_D] = 0; 
                break;
                
            case SDLK_a: 
                game.keyboard[KEY_PRESS_7] = 0; 
                break;
                
            case SDLK_s: 
                game.keyboard[KEY_PRESS_8] = 0; 
                break;
                
            case SDLK_d: 
                game.keyboard[KEY_PRESS_9] = 0; 
                break;
                
            case SDLK_f: 
                game.keyboard[KEY_PRESS_E] = 0; 
                break;
                
            case SDLK_z: 
                game.keyboard[KEY_PRESS_A] = 0; 
                break;
                
            case SDLK_x: 
                game.keyboard[KEY_PRESS_0] = 0; 
                break;
                
            case SDLK_c: 
                game.keyboard[KEY_PRESS_B] = 0; 
                break;
                
            case SDLK_v: 
                game.keyboard[KEY_PRESS_F] = 0; 
                break;      
        }
    }
}

int main(int argc, const char* argv[]) {
    chip8 game;
    game.load_rom(argv[1]);

    const int SCREEN_WIDTH = 640;
    const int SCREEN_HEIGHT = 320;

    SDL_Window* main_window = NULL;
    SDL_Surface* base_surface = NULL;
    
    // SDL_Event event;
    bool exit_flag = 0;

    init_window(main_window, base_surface, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_UpdateWindowSurface(main_window);
    
        while (true) {
            game.emulate_cycle();
            
            SDL_Event event;
            
            while(SDL_PollEvent(&event)) {
                set_keys(game, event);
                
                if (event.type == SDL_QUIT) {
                exit(0);
                }
            }
            
            if (game.draw_flag) {
                draw_graphics(game, main_window, base_surface, SCREEN_WIDTH, SCREEN_HEIGHT);
            }
            game.draw_flag = 0;

            std::this_thread::sleep_for(std::chrono::microseconds(1200)); 
        
        }

    quit(main_window);
    return 0;
}
