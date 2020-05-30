#include <array>

class chip8 {
private:
    std::array<unsigned char, 4096> memory;     // memory array
    unsigned short opcode;                      // current opcode
    
    std::array<unsigned char, 16> V;            // registers
    unsigned short pc;                          // program counter
    unsigned short I;                           // address pointer or index register
    
    std::array<unsigned short, 16> stack;       // stack levels
    unsigned char sp;                           // stack pointer
    
    unsigned char delay_timer;
    unsigned char sound_timer;

    std::array<unsigned char, 80> fontset;
    
public:
    chip8();
    std::array<unsigned char, 16> keyboard;
    std::array<unsigned char, 64 * 32> display;
    void initialize();
    void clear_display();
    bool load_rom(const char* rom_name);
    void emulate_cycle();
    void decode_opcode(std::unique_ptr<unsigned short>& opcode);
    bool draw_flag = 0;
};