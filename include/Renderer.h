#ifndef RENDERER_H
#define RENDERER_H

#include "Chip8.h"
#include <SDL.h>


class Renderer {
public:
  Renderer(int windowWidth, int windowHeight, int pixelSize);
  ~Renderer();
  bool init();
  void clear();
  void draw(const Chip8 &chip8);
  bool isInitialized() const { return initialized; }
  SDL_Window *getWindow() { return window; }

private:
  int windowWidth;
  int windowHeight;
  int pixelSize;
  bool initialized;
  SDL_Window *window;
  SDL_Surface *screenSurface;
};

#endif
