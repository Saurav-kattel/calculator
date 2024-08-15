
#include "lib.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void handlePollEvents(int *flag, SDL_Event *e, char *input, const Keys *keys,
                      int keySize) {

  int mouseX, mouseY;
  switch (e->type) {
  case SDL_QUIT:
    *flag = 1;
    break;
  case SDL_KEYDOWN:
    switch (e->key.keysym.sym) {
    case SDLK_ESCAPE:
      *flag = 1;
      break;
    case SDLK_BACKSPACE:
      if (strlen(input) > 0) {
        input[strlen(input) - 1] = '\0';
      }
    default:
      break;
    }
    break;
  case SDL_TEXTINPUT:
    strcat(input, e->text.text);
    break;
  case SDL_MOUSEBUTTONDOWN:
    SDL_GetMouseState(&mouseX, &mouseY);
    for (int i = 0; i < keySize; i++) {
      Keys key = keys[i];
      char equalKey = '=';
      if (mouseX >= key.x && mouseX <= key.x + key.w && mouseY >= key.y &&
          mouseY <= key.y + key.h) {
        if (key.name != equalKey) {
          strcat(input, &key.name);
        } else {
          Lexer *lex = InitLexer(input);
          Token *tkn = GetNextToken(lex);

          while ((tkn != NULL) && (tkn->type != ERR)) {
            printf("%s\n", tkn->value);
            tkn = GetNextToken(lex);
          }

          free(tkn); // Ensure memory is freed for the last token
          free(lex);
        }
      }
    }

  default:
    break;
  }
}

void drawKey(SDL_Renderer *ren, TTF_Font *font, Keys key) {
  SDL_SetRenderDrawColor(ren, 0XFF, 0XFF, 0XFF, 0XFF);
  SDL_Rect rect = {key.x, key.y, key.w, key.h};
  SDL_RenderFillRect(ren, &rect);
  drawKeyChar(ren, font, key);
}

void drawKeyChar(SDL_Renderer *ren, TTF_Font *font, Keys key) {
  SDL_Color textColor = {0, 0, 0, 244};
  SDL_Surface *textureSurface =
      TTF_RenderText_Solid(font, &key.name, textColor);

  if (textureSurface) {
    SDL_Texture *texture = SDL_CreateTextureFromSurface(ren, textureSurface);
    SDL_Rect textRect = {key.x + (key.w - textureSurface->w) / 2,
                         key.y + (key.h - textureSurface->h) / 2,
                         textureSurface->w, textureSurface->h};
    SDL_RenderCopy(ren, texture, NULL, &textRect);
    SDL_FreeSurface(textureSurface);
    SDL_DestroyTexture(texture);
  }
}

Token *NewToken(TokenType type, char *value) {
  Token *tkn = (Token *)malloc(sizeof(Token));
  tkn->type = type;
  strcpy(tkn->value, value);
  return tkn;
}

static int isAtEnd(Lexer *lexr) {
  int strLen = strlen(lexr->soruce);
  return lexr->cursor >= strLen;
}

static char advance(Lexer *lexr) {
  if (!isAtEnd(lexr)) {
    char chr = lexr->soruce[lexr->cursor];
    lexr->cursor++;
    return chr;
  }
  return ' ';
}

static char peek(Lexer *lexr) {
  if (!isAtEnd(lexr)) {
    return lexr->soruce[lexr->cursor];
  }
  return ' ';
}

static char peekNext(Lexer *lexr) {
  if (!isAtEnd(lexr)) {
    return lexr->soruce[lexr->cursor + 1];
  }
  return ' ';
}
void skipWhiteSpace(Lexer *lxr) {
  if (isspace(peek(lxr))) {
    advance(lxr);
  }
}

static int isOperator(char c) {
  if (c == '+' || c == '-' || c == '/' || c == '*' || c == '(' || c == ')') {
    return 1;
  }
  return 0;
}

Lexer *InitLexer(char *source) {
  Lexer *lex = (Lexer *)malloc(sizeof(Lexer));
  lex->cursor = 0;
  lex->tokenCount = 0;
  lex->tokens = (Token *)malloc(sizeof(Token) * 100);
  int soruceLen = strlen(source);
  lex->soruce = (char *)malloc(sizeof(char) * soruceLen + 1);
  strcpy(lex->soruce, source);
  return lex;
}

Token *GetNextToken(Lexer *lexer) {
  skipWhiteSpace(lexer);
  if (isAtEnd(lexer)) {
    return NULL; // End of input
  }

  char c = advance(lexer);

  // Handle single-character tokens
  if (c == '(') {
    Token *tkn = NewToken(LPAREN, "LPAREN");
    lexer->tokenCount++;
    return tkn;
  }

  if (c == ')') {
    Token *tkn = NewToken(RPAREN, "RPAREN");
    lexer->tokenCount++;
    return tkn;
  }

  if (c == '*') {
    Token *tkn = NewToken(MULTIPLY, "MULTIPLY");
    lexer->tokenCount++;
    return tkn;
  }

  if (c == '+') {
    Token *tkn = NewToken(PLUS, "PLUS");
    lexer->tokenCount++;
    return tkn;
  }

  if (c == '-') {
    Token *tkn = NewToken(MINUS, "MINUS");
    lexer->tokenCount++;
    return tkn;
  }

  if (c == '/') {
    Token *tkn = NewToken(DIVIDE, "DIVIDE");
    lexer->tokenCount++;
    return tkn;
  }
  if (isalpha(c) && !isOperator(c)) {
    Token *tkn = NewToken(ERR, "Error");
    lexer->tokenCount++;
    return tkn;
  }

  // Handle numbers
  if (isdigit(c)) {
    int start = lexer->cursor - 1;
    while (isdigit(peek(lexer))) {
      advance(lexer);
    }
    if (peek(lexer) == '.' && isdigit(peekNext(lexer))) {
      advance(lexer); // Skip the '.'
      while (isdigit(peek(lexer))) {
        advance(lexer);
      }
    }
    int length = lexer->cursor - start;
    char *buffer = (char *)malloc(length + 1);
    strncpy(buffer, lexer->soruce + start, length);
    buffer[length] = '\0';
    Token *tkn = NewToken(NUMBER, buffer);
    free(buffer);
    lexer->tokenCount++;
    return tkn;
  }

  printf("unknown token: %c\n", c);
  Token *tkn = NewToken(ERR, "Error");
  lexer->tokenCount++;
  return tkn;
}
