#include "../include/Chip8.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <random>

Chip8::Chip8() {
    initialize();
}

void Chip8::initialize() {
    pc = 0x200; // Program counter starts at 0x200
    opcode = 0;
    I = 0;
    sp = 0;

    memory.fill(0);
    V.fill(0);
    gfx.fill(0);
    stack.fill(0);
    key.fill(0);

    delay_timer = 0;
    sound_timer = 0;

    // Load fontset into memory (0x000–0x04F)
    uint8_t chip8_fontset[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
    for (int i = 0; i < 80; ++i) {
        memory[i] = chip8_fontset[i];
    }

    drawFlag = false;
}

void Chip8::loadROM(const std::string &filename) {
    std::ifstream rom(filename, std::ios::binary | std::ios::ate);
    if (!rom.is_open()) {
        std::cerr << "Failed to open ROM: " << filename << "\n";
        return;
    }

    std::streamsize size = rom.tellg();
    rom.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (!rom.read(buffer.data(), size)) {
        std::cerr << "Failed to read ROM: " << filename << "\n";
        return;
    }

    for (size_t i = 0; i < buffer.size(); ++i) {
        memory[0x200 + i] = static_cast<uint8_t>(buffer[i]);
    }
}

void Chip8::emulateCycle() {
    // Fetch
    opcode = memory[pc] << 8 | memory[pc + 1];

    // Decode & Execute
    executeOpcode(opcode);

}

void Chip8::executeOpcode(uint16_t opcode) {
    uint16_t nnn = opcode & 0x0FFF;
    uint8_t  nn  = opcode & 0x00FF;
    uint8_t  n   = opcode & 0x000F;
    uint8_t  x   = (opcode & 0x0F00) >> 8;
    uint8_t  y   = (opcode & 0x00F0) >> 4;

    switch (opcode & 0xF000) {
    case 0x0000:
        switch (nn) {
            case 0xE0: // 00E0: CLS
                gfx.fill(0);
                drawFlag = true;
                pc += 2;
                break;
            case 0xEE: // 00EE: RET
                --sp;
                pc = stack[sp];
                pc += 2;
                break;
            default: // 0NNN: SYS (ignored)
                pc += 2;
                break;
        }
        break;

    case 0x1000: // 1NNN: JP addr
        pc = nnn;
        break;

    case 0x2000: // 2NNN: CALL addr
        stack[sp] = pc;
        ++sp;
        pc = nnn;
        break;

    case 0x3000: // 3XNN: SE Vx, byte
        pc += (V[x] == nn) ? 4 : 2;
        break;

    case 0x4000: // 4XNN: SNE Vx, byte
        pc += (V[x] != nn) ? 4 : 2;
        break;

    case 0x5000: // 5XY0: SE Vx, Vy
        pc += (V[x] == V[y]) ? 4 : 2;
        break;

    case 0x6000: // 6XNN: LD Vx, byte
        V[x] = nn;
        pc += 2;
        break;

    case 0x7000: // 7XNN: ADD Vx, byte
        V[x] += nn;
        pc += 2;
        break;

    case 0x8000:
        switch (n) {
            case 0x0: // 8XY0: LD Vx, Vy
                V[x] = V[y];
                break;
            case 0x1: // 8XY1: OR Vx, Vy
                V[x] |= V[y];
                break;
            case 0x2: // 8XY2: AND Vx, Vy
                V[x] &= V[y];
                break;
            case 0x3: // 8XY3: XOR Vx, Vy
                V[x] ^= V[y];
                break;
            case 0x4: { // 8XY4: ADD Vx, Vy with carry
                uint16_t sum = V[x] + V[y];
                V[0xF] = (sum > 255);
                V[x] = sum & 0xFF;
                break;
            }
            case 0x5: // 8XY5: SUB Vx, Vy
                V[0xF] = (V[x] > V[y]);
                V[x] -= V[y];
                break;
            case 0x6: // 8XY6: SHR Vx {, Vy}
                V[0xF] = V[x] & 0x1;
                V[x] >>= 1;
                break;
            case 0x7: // 8XY7: SUBN Vx, Vy
                V[0xF] = (V[y] > V[x]);
                V[x] = V[y] - V[x];
                break;
            case 0xE: // 8XYE: SHL Vx {, Vy}
                V[0xF] = (V[x] & 0x80) >> 7;
                V[x] <<= 1;
                break;
        }
        pc += 2;
        break;

    case 0x9000: // 9XY0: SNE Vx, Vy
        pc += (V[x] != V[y]) ? 4 : 2;
        break;

    case 0xA000: // ANNN: LD I, addr
        I = nnn;
        pc += 2;
        break;

    case 0xB000: // BNNN: JP V0, addr
        pc = nnn + V[0];
        break;

    case 0xC000: { // CXNN: RND Vx, byte
        static std::default_random_engine randGen{std::random_device{}()};
        static std::uniform_int_distribution<int> randByte(0, 255);
        V[x] = randByte(randGen) & nn;
        pc += 2;
        break;
    }

    case 0xD000: { // DXYN: DRW Vx, Vy, nibble
        uint8_t vx = V[x] % VIDEO_WIDTH;
        uint8_t vy = V[y] % VIDEO_HEIGHT;
        V[0xF] = 0;
        for (int row = 0; row < n; ++row) {
            uint8_t spriteByte = memory[I + row];
            for (int col = 0; col < 8; ++col) {
                if (spriteByte & (0x80 >> col)) {
                    int index = (vx + col + ((vy + row) * VIDEO_WIDTH)) % (VIDEO_WIDTH * VIDEO_HEIGHT);
                    if (gfx[index] == 1) V[0xF] = 1;
                    gfx[index] ^= 1;
                }
            }
        }
        drawFlag = true;
        pc += 2;
        break;
    }

    case 0xE000:
        switch (nn) {
            case 0x9E: // EX9E: SKP Vx
                pc += (key[V[x]]) ? 4 : 2;
                break;
            case 0xA1: // EXA1: SKNP Vx
                pc += (!key[V[x]]) ? 4 : 2;
                break;
            default:
                pc += 2;
                break;
        }
        break;

    case 0xF000:
        switch (nn) {
            case 0x07: // FX07: LD Vx, DT
                V[x] = delay_timer;
                pc += 2;
                break;
            case 0x0A: { // FX0A: LD Vx, K (wait for key press)
                bool keyPress = false;
                for (int i = 0; i < 16; ++i) {
                    if (key[i]) {
                        V[x] = i;
                        keyPress = true;
                    }
                }
                if (!keyPress) return; // don't advance PC if no key press
                pc += 2;
                break;
            }
            case 0x15: // FX15: LD DT, Vx
                delay_timer = V[x];
                pc += 2;
                break;
            case 0x18: // FX18: LD ST, Vx
                sound_timer = V[x];
                pc += 2;
                break;
            case 0x1E: // FX1E: ADD I, Vx
                I += V[x];
                pc += 2;
                break;
            case 0x29: // FX29: LD F, Vx (font character)
                I = V[x] * 5;
                pc += 2;
                break;
            case 0x33: { // FX33: BCD of Vx
                memory[I] = V[x] / 100;
                memory[I + 1] = (V[x] / 10) % 10;
                memory[I + 2] = V[x] % 10;
                pc += 2;
                break;
            }
            case 0x55: // FX55: LD [I], V0..Vx
                for (int i = 0; i <= x; ++i) memory[I + i] = V[i];
                pc += 2;
                break;
            case 0x65: // FX65: LD V0..Vx, [I]
                for (int i = 0; i <= x; ++i) V[i] = memory[I + i];
                pc += 2;
                break;
        }
        break;

    default:
        std::cerr << "Unknown opcode: " << std::hex << opcode << "\n";
        pc += 2;
        break;
    }
}

void Chip8::updateTimers() {
    if (delay_timer > 0) --delay_timer;
    if (sound_timer > 0) {
        --sound_timer;
        if (sound_timer == 0) {
            // TODO: trigger beep sound
        }
    }
}


void Chip8::setKey(uint8_t keyIndex, bool pressed) {
    if (keyIndex < 16) {
        key[keyIndex] = pressed ? 1 : 0;
    }
}

const std::array<uint8_t, Chip8::VIDEO_WIDTH * Chip8::VIDEO_HEIGHT> &Chip8::getDisplay() const {
    return gfx;
}
