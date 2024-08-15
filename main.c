#include "lib/lib.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <string.h>

#define FRAME_TIME 16.67

const int keyWidth = 60;
const int keyHeight = 60;
const int keySpacing = 10; // Space between keys
const int startX = 150;    // Starting X position
const int startY = 200;    // Starting Y position

const Keys keys[] = {
    // Row 1
    {startX, startY, keyWidth, keyHeight, '7'},
    {startX + (keyWidth + keySpacing) * 1, startY, keyWidth, keyHeight, '8'},
    {startX + (keyWidth + keySpacing) * 2, startY, keyWidth, keyHeight, '9'},
    {startX + (keyWidth + keySpacing) * 3, startY, keyWidth, keyHeight, '/'},

    // Row 2
    {startX, startY + (keyHeight + keySpacing) * 1, keyWidth, keyHeight, '4'},
    {startX + (keyWidth + keySpacing) * 1,
     startY + (keyHeight + keySpacing) * 1, keyWidth, keyHeight, '5'},
    {startX + (keyWidth + keySpacing) * 2,
     startY + (keyHeight + keySpacing) * 1, keyWidth, keyHeight, '6'},
    {startX + (keyWidth + keySpacing) * 3,
     startY + (keyHeight + keySpacing) * 1, keyWidth, keyHeight, '*'},

    // Row 3
    {startX, startY + (keyHeight + keySpacing) * 2, keyWidth, keyHeight, '1'},
    {startX + (keyWidth + keySpacing) * 1,
     startY + (keyHeight + keySpacing) * 2, keyWidth, keyHeight, '2'},
    {startX + (keyWidth + keySpacing) * 2,
     startY + (keyHeight + keySpacing) * 2, keyWidth, keyHeight, '3'},
    {startX + (keyWidth + keySpacing) * 3,
     startY + (keyHeight + keySpacing) * 2, keyWidth, keyHeight, '-'},

    // Row 4
    {startX, startY + (keyHeight + keySpacing) * 3, keyWidth, keyHeight, '0'},
    {startX + (keyWidth + keySpacing) * 1,
     startY + (keyHeight + keySpacing) * 3, keyWidth, keyHeight, '.'},
    {startX + (keyWidth + keySpacing) * 2,
     startY + (keyHeight + keySpacing) * 3, keyWidth, keyHeight, '='},
    {startX + (keyWidth + keySpacing) * 3,
     startY + (keyHeight + keySpacing) * 3, keyWidth, keyHeight, '+'}};

const int keySize = sizeof(keys) / sizeof(keys[0]);
int main() {
  int quit = 0;
  Uint32 startTime = 0;
  char input[300] = "";

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not be initialized, Error: %s\n", SDL_GetError());
    return 1;
  }

  if (TTF_Init() == -1) {
    printf("SDL_ttf could not be initialized, Error: %s\n", TTF_GetError());
    SDL_Quit();
    return 1;
  }

  SDL_Event e;
  SDL_Window *window =
      SDL_CreateWindow("calculator", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, 600, 600, SDL_WINDOW_SHOWN);
  if (!window) {
    fprintf(stderr, "Error creating window: %s\n", SDL_GetError());
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    fprintf(stderr, "Error creating renderer: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  TTF_Font *font =
      TTF_OpenFont("/usr/share/fonts/TTF/JetBrainsMono-Light.ttf", 16);
  if (!font) {
    fprintf(stderr, "Error loading font: %s\n", TTF_GetError());
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  SDL_Rect inputField = {100, 100, 400, 60};
  SDL_StartTextInput();

  while (!quit) {
    while (SDL_PollEvent(&e) != 0) {
      startTime = SDL_GetTicks();
      handlePollEvents(&quit, &e, input, keys, keySize);
    }

    // Clear the screen
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);

    // Draw the input field
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF); // White color
    SDL_RenderFillRect(renderer, &inputField);

    // Render the text inside the input field
    SDL_Color textColor = {0, 0, 0, 255}; // Black color
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, input, textColor);

    if (textSurface) {
      SDL_Texture *textTexture =
          SDL_CreateTextureFromSurface(renderer, textSurface);
      SDL_Rect textRect = {inputField.x + 5, inputField.y + 5, textSurface->w,
                           textSurface->h};
      SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
      SDL_FreeSurface(textSurface);
      SDL_DestroyTexture(textTexture);
    }

    for (int i = 0; i < keySize; i++) {
      drawKey(renderer, font, keys[i]);
    }

    SDL_RenderPresent(renderer);

    Uint32 frameTime = SDL_GetTicks() - startTime;
    if (frameTime < FRAME_TIME) {
      SDL_Delay(FRAME_TIME - frameTime);
    }
  }

  SDL_StopTextInput();
  TTF_CloseFont(font);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  TTF_Quit();
  SDL_Quit();

  return 0;
}
