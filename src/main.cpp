#include <chrono>
#include <iostream>
#include <SDL.h>
#include "../include/Chip8.h"
#include "../include/Renderer.h"
#include "../include/Input.h"

// Adjustable from command line: default 700
double CPU_RATE = 700.0; // instructions/sec
const double TIMER_RATE = 60.0; // Hz

int main(int argc, char* argv[]) {
    // Allow CPU rate override from command line
    if (argc > 1) {
        CPU_RATE = std::stod(argv[1]);
    }
    std::cout << "Running at " << CPU_RATE << " instructions/sec\n";

    Chip8 chip8;
    chip8.loadROM("roms/pong.ch8");

    Renderer renderer(640, 320, 10);
    if (!renderer.init()) {
        std::cerr << "Failed to initialize renderer\n";
        return 1;
    }

    bool quit = false;
    SDL_Event e;

    using clock = std::chrono::high_resolution_clock;
    auto lastFrameTime = clock::now();

    // Number of CPU cycles to run per 60Hz frame
    int cyclesPerFrame = static_cast<int>(CPU_RATE / TIMER_RATE);
    double frameDelay = 1000.0 / TIMER_RATE; // ~16.67 ms

    while (!quit) {
        // Handle input
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = true;
            processInput(chip8, e);
        }

        auto now = clock::now();

        // Run once per 60Hz frame
        if (std::chrono::duration<double, std::milli>(now - lastFrameTime).count() >= frameDelay) {
            bool frameNeedsDraw = false; // <-- NEW: buffer the draw flag

            // Run multiple CPU cycles in one frame
            for (int i = 0; i < cyclesPerFrame; i++) {
                chip8.emulateCycle();

                // If any cycle sets drawFlag, remember to draw later
                if (chip8.drawFlag) {
                    frameNeedsDraw = true;
                    chip8.drawFlag = false; // reset so we don't render mid-frame
                }
            }

            // Update timers once per frame
            chip8.updateTimers();

            // Draw only once, after all cycles in this frame
            if (frameNeedsDraw) {
                renderer.draw(chip8);
            }

            lastFrameTime = now;
        }
    }

    return 0;
}
