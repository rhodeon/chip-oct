#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

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

void init_sdl() {
    /* 
     * Initializes SDL subsystems and mixer
     */

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO); 
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
}

void init_window(SDL_Window*& window, SDL_Surface*& surface, int width, int height) {
    /* 
     * Initializes main window
     */

    window = SDL_CreateWindow("CHIP-Oct", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    surface = SDL_GetWindowSurface(window);
}

void quit(SDL_Window*& window) {
    SDL_DestroyWindow(window);
    window = NULL;
}

void draw_graphics(chip8& game, SDL_Window*& window, SDL_Surface*& base_surface) {
    /* 
     * Copies the pixels in the display buffer and displays them on the corresponding positions on the screen
    */

    // width and height of CHIP-8 platform
    const int c8_width = 64;
    const int c8_height = 32;
    
    SDL_Rect pixel;
    pixel.w = base_surface->w / c8_width;
    pixel.h = base_surface->h / c8_height;
    std::array<unsigned char, c8_width * c8_height> screen = game.display;

    // Draw each pixel if corresponding value in display buffer is 1
    for (int row = 0; row < base_surface->h; row += pixel.h) {
        for (int column = 0; column < base_surface->w; column += pixel.w) {
            pixel.x = column;
            pixel.y = row;
            int pos = column/pixel.w + row/pixel.h * c8_width;
            
            if (screen[pos] == 1) { // pixel is drawn (white)
                SDL_FillRect(base_surface, &pixel, SDL_MapRGB(base_surface->format, 0xFF, 0xFF, 0xFF));
            }
            else    // pixel is erased/not drawn (black)
            {
                SDL_FillRect(base_surface, &pixel, SDL_MapRGB(base_surface->format, 0x00, 0x00, 0x00));
            }
                        
        }
    }
    SDL_UpdateWindowSurface(window);
}

void set_keys(chip8& game, SDL_Event& event) {
    /*
     * Changes keyboard state according to currently pressed keys 
     */

    // set key state to 1 if pressed
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_1: 
                game.keyboard[KEY_PRESS_1] = 1;
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

void game_loop(chip8& game, SDL_Window*& window, SDL_Surface*& base_surface, Mix_Chunk*& beep) {
    SDL_Event event;
    
    while (true) {
            game.emulate_cycle();
        
            while(SDL_PollEvent(&event)) {  // set key actions
                set_keys(game, event);

                if (event.type == SDL_QUIT) {
                exit(0);
                }
            }
            
            if (game.draw_flag) {
                draw_graphics(game, window, base_surface);
                game.draw_flag = false;
            }

            if (game.sound_timer > 0) {
                Mix_PlayChannel(-1, beep, 0);
            }

            std::this_thread::sleep_for(std::chrono::microseconds(1000)); 
        }
}

int main(int argc, const char* argv[]) {
    init_sdl();
    chip8 game;

    // check for number of arguments and load ROM
    if (argc == 2) {
        const char* rom = argv[1];
        if(!game.load_rom(rom)) {
            std::cout << "ROM not loaded" << std::endl;
            exit(0);
        }
    }
    else {  // incorrect number of arguments
        std::cout << "Usage: ./chip-oct rom_name" << std::endl;
        exit(0);
    }
    
    // data of display
    const int SCREEN_WIDTH = 960;
    const int SCREEN_HEIGHT = 480;
    SDL_Window* main_window = NULL;
    SDL_Surface* base_surface = NULL;
    
    // initialize window and beep object
    init_window(main_window, base_surface, SCREEN_WIDTH, SCREEN_HEIGHT);
    Mix_Chunk* beep = Mix_LoadWAV("resources/beep.wav");    // for sound timer
    
    // begin game loop
    game_loop(game, main_window, base_surface, beep);
    
    exit(0);
    return 0;
}
