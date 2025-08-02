#ifndef CHIP8_H
#define CHIP8_H

#include <array>
#include <cstdint>
#include <string>

class Chip8 {
public:
    static const unsigned int VIDEO_WIDTH = 64;
    static const unsigned int VIDEO_HEIGHT = 32;

    Chip8();

    void updateTimers();

    void loadROM(const std::string &filename);
    void emulateCycle();
    void setKey(uint8_t key, bool pressed);
    const std::array<uint8_t, VIDEO_WIDTH * VIDEO_HEIGHT> &getDisplay() const;

    bool drawFlag = false;

private:
    uint16_t opcode;
    std::array<uint8_t, 4096> memory;
    std::array<uint8_t, 16> V;
    uint16_t I;
    uint16_t pc;

    std::array<uint16_t, 16> stack;
    uint8_t sp;

    uint8_t delay_timer;
    uint8_t sound_timer;

    std::array<uint8_t, VIDEO_WIDTH * VIDEO_HEIGHT> gfx;
    std::array<uint8_t, 16> key;

    void initialize();
    void executeOpcode(uint16_t opcode);
};

#endif
