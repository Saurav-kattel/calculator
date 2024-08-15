#ifndef LIB_H
#define LIB_H

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>

typedef struct {
  int x;
  int y;
  int w;
  int h;
  char name;
} Keys;

typedef int TokenType;

typedef struct {
  TokenType type;
  char value[100];
} Token;

typedef struct {
  int cursor;
  char *soruce;
  int tokenCount;
  Token *tokens;
} Lexer;

enum TokenTypes { LPAREN, RPAREN, DIVIDE, MULTIPLY, PLUS, MINUS, ERR, NUMBER };

Lexer *InitLexer(char *source);
Token *NewToken(TokenType type, char *value);
Token *GetNextToken(Lexer *lexer);

void handlePollEvents(int *flag, SDL_Event *e, char *input, const Keys *key,
                      int keySize);

void drawKey(SDL_Renderer *ren, TTF_Font *font, Keys key);
void drawKeyChar(SDL_Renderer *ren, TTF_Font *font, Keys key);
void eval(char *input);

#endif // LIB_H
