#include <chrono>
#include <iostream>
#include <SDL.h>
#include "../include/Chip8.h"
#include "../include/Renderer.h"
#include "../include/Input.h"

double CPU_RATE = 700.0;
const double TIMER_RATE = 60.0;

void parseArguments(int argc, char* argv[]) {
    if (argc > 1) {
        CPU_RATE = std::stod(argv[1]);
    }
    std::cout << "Running at " << CPU_RATE << " instructions/sec\n";
}

Renderer* initializeRenderer(int width, int height, int scale) {
    Renderer* renderer = new Renderer(width, height, scale);
    if (!renderer->init()) {
        std::cerr << "Failed to initialize renderer\n";
        delete renderer;
        return nullptr;
    }
    return renderer;
}

bool processEvents(Chip8& chip8) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            return true; // signal to quit
        }
        processInput(chip8, e);
    }
    return false;
}

void runEmulationFrame(Chip8& chip8, Renderer& renderer, int cyclesPerFrame) {
    bool frameNeedsDraw = false;
    for (int i = 0; i < cyclesPerFrame; i++) {
        chip8.emulateCycle();
        if (chip8.drawFlag) {
            frameNeedsDraw = true;
            chip8.drawFlag = false;
        }
    }

    chip8.updateTimers();

    if (frameNeedsDraw) {
        renderer.draw(chip8);
    }
}

int main(int argc, char* argv[]) {
    parseArguments(argc, argv);

    Chip8 chip8;
    chip8.loadROM("roms/Pong.ch8");

    Renderer* renderer = initializeRenderer(640, 320, 10);
    if (!renderer) {
        return 1;
    }

    using clock = std::chrono::high_resolution_clock;
    auto lastFrameTime = clock::now();

    int cyclesPerFrame = static_cast<int>(CPU_RATE / TIMER_RATE);
    double frameDelay = 1000.0 / TIMER_RATE;

    bool quit = false;
    while (!quit) {
        quit = processEvents(chip8);

        auto now = clock::now();
        if (std::chrono::duration<double, std::milli>(now - lastFrameTime).count() >= frameDelay) {
            runEmulationFrame(chip8, *renderer, cyclesPerFrame);
            lastFrameTime = now;
        }
    }

    delete renderer;
    return 0;
}
