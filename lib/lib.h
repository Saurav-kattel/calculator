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
typedef enum { NODE_NUM, NODE_BINOP } NodeType;

typedef struct AstNode {
  NodeType type;
  union {
    double number;
    struct {
      struct AstNode *left;
      struct AstNode *right;
      TokenType op;
    } binary_op;
  };
} AstNode;

typedef struct {
  TokenType type;
  char value[100];
} Token;

typedef struct {
  int cursor;
  char *soruce;
  int tokenCount;
} Lexer;

typedef struct {
  Token *currentToken;
} Parser;

enum TokenTypes { LPAREN, RPAREN, DIVIDE, MULTIPLY, PLUS, MINUS, ERR, NUMBER };

Lexer *InitLexer(char *source);
Token *NewToken(TokenType type, char *value);
Token *GetNextToken(Lexer *lexer);

void handlePollEvents(int *flag, SDL_Event *e, char *input, const Keys *key,
                      int keySize);

void drawKey(SDL_Renderer *ren, TTF_Font *font, Keys key);
void drawKeyChar(SDL_Renderer *ren, TTF_Font *font, Keys key);
void eval(char *input);
static char advance(Lexer *);

void eat(TokenType type, Parser *p, Lexer *lex);
int check(TokenType, Parser *);
AstNode *term(Lexer *, Parser *);
AstNode *expr(Lexer *, Parser *);
AstNode *factor(Lexer *, Parser *);
double evalAst(AstNode *node);
void advanceParser(Lexer *, Parser *);
void freeAST(AstNode *node);
#endif // LIB_H
       //
