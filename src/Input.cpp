#include "../include/Input.h"

void processInput(Chip8 &chip8, const SDL_Event &e) {
    if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
        bool isPressed = (e.type == SDL_KEYDOWN);
        switch (e.key.keysym.sym) {
            case SDLK_1: chip8.setKey(0x1, isPressed); break;
            case SDLK_2: chip8.setKey(0x2, isPressed); break;
            case SDLK_3: chip8.setKey(0x3, isPressed); break;
            case SDLK_4: chip8.setKey(0xC, isPressed); break;
            case SDLK_q: chip8.setKey(0x4, isPressed); break;
            case SDLK_w: chip8.setKey(0x5, isPressed); break;
            case SDLK_e: chip8.setKey(0x6, isPressed); break;
            case SDLK_r: chip8.setKey(0xD, isPressed); break;
            case SDLK_a: chip8.setKey(0x7, isPressed); break;
            case SDLK_s: chip8.setKey(0x8, isPressed); break;
            case SDLK_d: chip8.setKey(0x9, isPressed); break;
            case SDLK_f: chip8.setKey(0xE, isPressed); break;
            case SDLK_z: chip8.setKey(0xA, isPressed); break;
            case SDLK_x: chip8.setKey(0x0, isPressed); break;
            case SDLK_c: chip8.setKey(0xB, isPressed); break;
            case SDLK_v: chip8.setKey(0xF, isPressed); break;
        }
    }
}
