#include <iostream>
#include <array>
#include <cstdio>
#include <ctime>
#include <memory>
#include <fstream>

#include "chip8.h"


chip8::chip8() {
    fontset = {
        0xF0, 0x90, 0x90, 0x90, 0xF0,   // 0
        0x20, 0x60, 0x20, 0x20, 0x70,   // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0,   // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0,   // 3
        0x90, 0x90, 0xF0, 0x10, 0x10,   // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0,   // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0,   // 6
        0xF0, 0x10, 0x20, 0x40, 0x40,   // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0,   // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0,   // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90,   // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0,   // B
        0xF0, 0x80, 0x80, 0x80, 0xF0,   // C
        0xE0, 0x90, 0x90, 0x90, 0xE0,   // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0,   // E
        0xF0, 0x80, 0xF0, 0x80, 0x80,   // F
        
    };

    initialize();
}

void chip8::initialize() {
    pc = 0x200;     // program starts at location 0x200
    opcode = 0;     // reset opcode
    I = 0;          // reset index register
    sp = 0;         // reset stack pointer

    // reset timers
    delay_timer = 0;
    sound_timer = 0;

    // clear memory
    for (auto& byte : memory) {
        byte = 0;
    }

    // clear registers V0 - VF
    for (auto& reg : V) {
        reg = 0;
    }

    // clear stack
    for (auto& level : stack) {
        level = 0;
    }
    
    // clear keyboard
    for (auto& key : keyboard) {
        key = 0;
    }

    // clear display
    clear_display();

    
    // load fonts to memory
    for (int i = 0; i < 80; ++i) {
        memory[i] = fontset[i];
    }

    srand(time(NULL));
}

void chip8::clear_display() {
    for (auto& pixel : display) {
        pixel = 0;
    }
}

bool chip8::load_rom(const char* rom_name) {
    // load ROM
    std::fstream rom(rom_name, std::ios::in|std::ios::binary);
    
    // get size
    rom.seekg(0, rom.end);
    int rom_size = rom.tellg();
    rom.seekg(0);   // rewind rom
    // std::cout << rom_size << std::endl;

    if (4096 - 512 < rom_size) {    // ensure size of ROM is valid
        std::cerr << "ROM size too large for memory" << std::endl;
        return false;
    }
    
    // create buffer to hold ROM data
    std::unique_ptr<char, decltype(free)*>  data_buffer((char*)malloc(rom_size), free);   

    // copy rom data to buffer
    int data_size = rom.readsome(data_buffer.get(), rom_size);
    if (data_size != rom_size) {
        std::cerr << "Reading Error" << std::endl;
        return false;
    }

    // copy buffer data into CHIP-8 memory
    for (int byte = 0; byte < data_size; ++byte) {
        memory[512 + byte] = data_buffer.get()[byte];
    }

    rom.close();
    return true;
}

void chip8::emulate_cycle() {
    // fetch opcode
    opcode = (memory[pc] << 8) | memory[pc+1];
    std::unique_ptr<unsigned short> op_ptr = std::make_unique<unsigned short>(opcode);

    // decode opcode
    decode_opcode(op_ptr);

    // decrement timers
    if (delay_timer > 0) {
        --delay_timer;
    }

    if (sound_timer > 0) {
        // make sound
        --sound_timer;
    }
}

void chip8::decode_opcode(std::unique_ptr<unsigned short>& opcode) {
    switch (*opcode & 0xF000) {
        // 0NNN ignored
        case 0x0000: {   // 00E0 or 00EE
            switch (*opcode & 0x00FF) {
                case 0x00E0: {    // 00E0
                    // clear the screen
                    clear_display();
                    draw_flag = true;
                    pc += 2;
                    break;
                }

                case 0x00EE: {   // 00EE
                    // return from subroutine
                    --sp;
                    pc = stack[sp];
                    pc += 2;
                    break;
                }

                default:
                    std::cout << "Unknown Opcode:" << *opcode << std::endl;
                    break;
            }
            break;
        }

        case 0x1000: {   // 1NNN
            // jump to address NNN
            pc = *opcode & 0x0FFF;
            break;
        }

        case 0x2000: {   // 2NNN
            // execute subroutine starting at NNN
            stack[sp] = pc;    // save current address
            ++sp;
            pc = *opcode & 0x0FFF;
            break;
        }

        case 0x3000: {   // 3XNN
            // skip next instruction if value of VX equals NN
            unsigned char VX = V[(*opcode & 0x0F00) >> 8];
            unsigned char NN = *opcode & 0x00FF;
            if (VX == NN) {
                pc += 4;
            }
            else {
                pc += 2;
            }
            break; 
        }

        case 0x4000: {   // 4XNN
            // skip next instruction if value of VX doesn't equal NN
            unsigned char VX = V[(*opcode & 0x0F00) >> 8];
            unsigned char NN = *opcode & 0x00FF;
            if (VX != NN) {
                pc += 4;
            }
            else {
                pc += 2;
            }
            break;
        }

        case 0x5000: {    // 5XY0
            // skip next instruction if VX equals VY
            unsigned char VX = V[(*opcode & 0x0F00) >> 8];
            unsigned char VY = V[(*opcode & 0x00F0) >> 4];
            if (VX == VY) {
                pc += 4;
            }
            else {
                pc += 2;
            }
            break;
        }

        case 0x6000: {   // 6XNN
            // set VX to NN
            unsigned char& VX = V[(*opcode & 0x0F00) >> 8];
            unsigned char NN = *opcode & 0x00FF;
            VX = NN;
            pc += 2;
            break;
        }

        case 0x7000: {   // 7XNN
            // Add NN to VX
            unsigned char& VX = V[(*opcode & 0x0F00) >> 8];
            unsigned char NN = *opcode & 0x00FF;
            VX += NN;
            pc += 2;
            break;
        }

        case 0x8000: {    // 8XY0, 8XY1, 8XY2, 8XY3, 8XY4, 8XY5, 8XY6, 8XY7, or 8XYE
            switch (*opcode & 0x000F) {
                case 0x0000: {   // 8XY0
                    //  set VX to VY
                    unsigned char& VX = V[(*opcode & 0x0F00) >> 8];
                    unsigned char VY = V[(*opcode & 0x00F0) >> 4];
                    VX = VY;
                    pc += 2;
                    break;
                }

                case 0x0001: {   // 8XY1
                    //  set VX to (VX OR VY)
                    unsigned char& VX = V[(*opcode & 0x0F00) >> 8];
                    unsigned char VY = V[(*opcode & 0x00F0) >> 4];
                    VX |= VY;
                    pc += 2;
                    break;
                }

                case 0x0002: {   // 8XY2
                    //  set VX to (VX AND VY)
                    unsigned char& VX = V[(*opcode & 0x0F00) >> 8];
                    unsigned char VY = V[(*opcode & 0x00F0) >> 4];
                    VX &= VY;
                    pc += 2;
                    break;
                }

                case 0x0003: {   // 8XY3
                    //  set VX to VX XOR VY
                    unsigned char& VX = V[(*opcode & 0x0F00) >> 8];
                    unsigned char VY = V[(*opcode & 0x00F0) >> 4];
                    VX ^= VY;
                    pc += 2;
                    break;
                }

                case 0x0004: {   // 8XY4
                    // add VY to VX,
                    // set VF to 1 if carry occurs, and 0 otherwise
                    unsigned char& VX = V[(*opcode & 0x0F00) >> 8];
                    unsigned char VY = V[(*opcode & 0x00F0) >> 4];
                    unsigned char VSUM = VX + VY;
                    
                    if ((VSUM < VX) || (VSUM < VY)) {       // overflow occurs
                        V[0xF] = 1;
                    }
                    else {
                        V[0xF] = 0;
                    }
                    
                    VX = VSUM;
                    pc += 2;
                    break;
                }

                case 0x0005: {   // 8XY5
                    // subtract VY from VX
                    // set VF to 0 if borrow occurs, and 1 otherwise
                    unsigned char& VX = V[(*opcode & 0x0F00) >> 8];
                    unsigned char VY = V[(*opcode & 0x00F0) >> 4];
                    
                    if (VX < VY) {      // borrow occurs
                        V[0xF] = 0;
                    }
                    else {
                        V[0xF] = 1;
                    }
                    VX -= VY;
                    pc += 2;
                    break;
                }

                case 0x0006: {   // 8XY6
                    // set VF to the LSB of VX
                    // shift VX right once
                    unsigned char& VX = V[(*opcode & 0x0F00) >> 8];
                    V[0xF] = (VX & 0x01);    // LSB of VX
                    VX >>= 1;
                    pc += 2;
                    break;
                }

                case 0x0007: {   // 8XY7
                    // set VX to VY - VX
                    // set VF to 0 if borrow occurs, and 1 otherwise
                    unsigned char& VX = V[(*opcode & 0x0F00) >> 8];
                    unsigned char VY = V[(*opcode & 0x00F0) >> 4];
                    
                    if (VY < VX) {      // borrow occurs
                        V[0xF] = 0;
                    }
                    else {
                        V[0xF] = 1;
                    }
                    
                    VX = VY - VX;
                    pc += 2;
                    break;
                }

                case 0x000E: {   // 8XYE
                    // set VF to the MSB of VX
                    // shift VX left once
                    unsigned char& VX = V[(*opcode & 0x0F00) >> 8];
                    V[0xF] = (VX >> 7);    // MSB of VX
                    VX <<= 1;
                    pc += 2;
                    break;
                }

                default:
                    std::cout << "Unknown Opcode:" << *opcode << std::endl;
                    break;
            }
            break;
        }

        case 0x9000: {   // 9XY0
            // skip next instruction if VX doesn't equal VY
            unsigned char VX = V[(*opcode & 0x0F00) >> 8];
            unsigned char VY = V[(*opcode & 0x00F0) >> 4];
            if (VX != VY) {
                pc += 4;
            }
            else {
                pc += 2;
            }
            break;
        }

        case 0xA000: {   // ANNN
            // store NNN in I
            I = *opcode & 0x0FFF;
            pc += 2;
            break;
        }

        case 0xB000: {   // BNNN
            // jump to address NNN + V0
            pc = (*opcode & 0x0FFF) + V[0];
            break;
        }

        case 0xC000: {    // CXNN
            // set VX to a random number with a mask of NN
            // range between 00 and FF
            // VX = number & mask
            unsigned char mask = *opcode & 0x00FF;      // mask = NN
            unsigned char& VX = V[(*opcode & 0x0F00) >> 8];
            // srand(time(NULL));
            unsigned char rand_num = rand() % (0xFF + 1);    // range of 00 - FF
            VX = rand_num & mask;
            pc += 2;
            break;
        }

        case 0xD000: {   // DXYN
            // draw sprite at VX, VY with N bytes of sprite data starting at I
            // flip pixel on screen if corresponding pixel in memory is 1
            // set VF to 1 if a pixel is unset, and 0 otherwise
            int X = V[(*opcode & 0x0F00) >> 8];    // starting X point (column)
            int Y = V[(*opcode & 0x00F0) >> 4];    // starting y point (row)
            int height = *opcode & 0x000F;  // number of rows (N)
            unsigned char pixels;           // pixels in memory (starting at address I)

            V[0xF] = 0;

            // traverse over rows
            for (int row = 0; row < height; ++row) {
                pixels = memory[I + row];
                int abs_row = Y + row;
                
                // traverse over columns
                for (int column = 0; column < 8; ++column) {
                    int abs_column = X + column;
                    unsigned char& pixel_on_display = display[abs_column + (abs_row * 64)];  // current pixel on screen
                    char pixel_in_memory = pixels & (0x80 >> column);

                    // check if pixel in memory is 1
                    if (pixel_in_memory != 0) {
                        if (pixel_on_display == 1) { // pixel is unset
                            V[0xF] = 1;
                        }
                        pixel_on_display ^= 1;  // flip corresponding pixel
                    }
                }
            }

            pc += 2;
            draw_flag = true;
            break;
        }

        case 0xE000: {   // EX9E, EXA1
            switch (*opcode & 0x00FF) {
                case 0x009E: {   // EX9E
                    // skip next instruction if key corresponding to value of VX is pressed
                    unsigned char VX = V[(*opcode & 0x0F00) >> 8];
                    if (keyboard[VX] == 1) {    // key is pressed
                        pc += 4;
                    }
                    else {
                        pc += 2;
                    }
                    break;
                }

                case 0x00A1: {   // EXA1
                    // skip next instruction if key corresponding to value of VX is not pressed
                    unsigned char VX = V[(*opcode & 0x0F00) >> 8];
                    if (keyboard[VX] == 0) {    // key is not pressed
                        pc += 4;
                    }
                    else {
                        pc += 2;
                    }
                    break;
                }

                default:
                    std::cout << "Unknown Opcode:" << *opcode << std::endl;
                    break;
            }
            break;
        }

        case 0xF000: {    // FX07, FX0A, FX15, FX18, FX1E, FX29, FX33, FX55, or FX65
            switch (*opcode & 0x00FF) {
                case 0x0007: {   // FX07
                    // set VX to value of delay timer
                    unsigned char& VX = V[(*opcode & 0x0F00) >> 8];
                    VX = delay_timer;
                    pc += 2;
                    break;
                }

                case 0x000A: {   // FX0A
                    // wait for a keypress and store its value in VX
                    unsigned char& VX = V[(*opcode & 0x0F00) >> 8];
                    bool pressed = false;    // state of keys
                    
                    for (int key = 0; key < 16; ++key) {    
                        if (keyboard[key] != 0) {   // a key is pressed
                            pressed = true;
                            VX = key;
                            break;
                        }
                    }

                    if (!pressed) {    // no key is pressed
                        return;
                    }
                    pc += 2;
                    break;
                }

                case 0x0015: {   // FX15
                    // set delay timer to VX
                    unsigned char VX = V[(*opcode & 0x0F00) >> 8];
                    delay_timer = VX;
                    pc += 2;
                    break;
                }

                case 0x0018: {   // FX18
                    // set sound timer to VX
                    unsigned char VX = V[(*opcode & 0x0F00) >> 8];
                    sound_timer = VX;
                    pc += 2;
                    break;
                }

                case 0x001E: {   // FX1E
                    // add VX to I
                    // set VF to 1 if range overflow occurs (sum > 0xFFF)
                    unsigned char VX = V[(*opcode & 0x0F00) >> 8];
                    
                    if (I + VX > 0xFFF) {
                        V[0xF] = 1;
                    }
                    else {
                        V[0xF] = 0;
                    }
                    
                    I += VX;
                    pc += 2;
                    break;
                }

                case 0x0029: {   // FX29
                    // Set I to the memory address of the sprite data corresponding to the hexadecimal digit in VX
                    unsigned char sprite = V[(*opcode & 0x0F00) >> 8];
                    I = sprite * 5;      // since each sprite occupies 5 bytes 
                    pc += 2;
                    break;
                }

                case 0x0033: {   // FX33
                    // Store the BCD of the value in register VX at addresses I, I+1, and I+2
                    unsigned char VX = V[(*opcode & 0x0F00) >> 8];
                    memory[I] = VX / 100;               // extract 1st digit
                    memory[I + 1] = (VX % 100) / 10;    // extract 2nd digit
                    memory[I + 2] = VX % 10;            // extract 3rd digit
                    pc += 2;
                    break;
                }

                case 0x0055: {   // FX55
                    // store values of V0-VX in memory starting at address I
                    // set I to I + X + 1
                    char X = (*opcode & 0x0F00) >> 8;
                    for (int reg = 0; reg <= X; ++reg) {
                        memory[I + reg] = V[reg];
                    }
                    I += X + 1;
                    pc += 2;
                    break;
                }

                case 0x0065: {   // FX65
                    // fill V0-VX with values at memory from address I
                    // set I to I + X + 1
                    char X = (*opcode & 0x0F00) >> 8;
                    for (int reg = 0; reg <= X; ++reg) {
                        V[reg] = memory[I + reg];
                    }
                    I += X + 1;
                    pc += 2;
                    break;
                }

                default:
                    std::cout << "Unknown Opcode:" << *opcode << std::endl;
                    break;
            }
            break;
        }
        
        default:
            std::cout << "Unknown Opcode:" << *opcode << std::endl;
            break;
    }
}
