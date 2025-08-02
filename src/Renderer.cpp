#include "../include/Renderer.h"

Renderer::Renderer(int windowWidth, int windowHeight, int pixelSize)
    : windowWidth(windowWidth), windowHeight(windowHeight),
      pixelSize(pixelSize), initialized(false), window(nullptr),
      screenSurface(nullptr) {}

Renderer::~Renderer() {
  if (window)
    SDL_DestroyWindow(window);
  SDL_Quit();
}

bool Renderer::init() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
    return false;

  window = SDL_CreateWindow("CHIP-8 Emulator", SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight,
                            SDL_WINDOW_SHOWN);
  if (!window)
    return false;

  screenSurface = SDL_GetWindowSurface(window);
  initialized = true;
  return true;
}

void Renderer::clear() {
  SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0, 0, 0));
}

void Renderer::draw(const Chip8 &chip8) {
    static auto lastDisplay = chip8.getDisplay(); // remembers last frame

    const auto &display = chip8.getDisplay();
    clear();

    for (int y = 0; y < 32; ++y) {
        for (int x = 0; x < 64; ++x) {
            if (display[y * 64 + x] || lastDisplay[y * 64 + x]) {
                SDL_Rect rect = {x * pixelSize, y * pixelSize, pixelSize, pixelSize};
                SDL_FillRect(screenSurface, &rect,
                             SDL_MapRGB(screenSurface->format, 255, 255, 255));
            }
        }
    }

    SDL_UpdateWindowSurface(window);
    lastDisplay = display; // update stored frame
}

