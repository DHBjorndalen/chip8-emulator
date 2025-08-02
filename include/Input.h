#ifndef INPUT_H
#define INPUT_H

#include <SDL.h>
#include "Chip8.h"

void processInput(Chip8 &chip8, const SDL_Event &e);

#endif
