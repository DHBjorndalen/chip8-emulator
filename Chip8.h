#ifndef CHIP8_H
#define CHIP8_H

#include <cstdint>
#include <array>

class Chip8 {
public:
    Chip8();
    void loadROM(const char* filename);
    void emulateCycle();
    void setKeys();
    const std::array<uint8_t, 64 * 32>& getDisplay() const;

private:
    uint16_t opcode;
    std::array<uint8_t, 4096> memory;
    std::array<uint8_t, 16> V;         // Registers V0-VF
    uint16_t I;                        // Index register
    uint16_t pc;                       // Program counter
    std::array<uint16_t, 16> stack;
    uint16_t sp;                       // Stack pointer
    uint8_t delay_timer;
    uint8_t sound_timer;
    std::array<uint8_t, 64 * 32> gfx;  // Graphics (pixels)
    std::array<uint8_t, 16> key;       // Keypad state

    void executeOpcode();
};

#endif
