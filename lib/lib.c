
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

//--------------------------------SDL---------------------------------------
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
        switch (key.name) {
        case '=':
          Lexer *lex = (Lexer *)malloc(sizeof(Lexer));
          lex->cursor = 0;
          lex->tokenCount = 0;
          int soruceLen = strlen(input);
          lex->soruce = (char *)malloc(sizeof(char) * soruceLen + 1);

          strcpy(lex->soruce, input);
          Parser *p = (Parser *)malloc(sizeof(Parser));
          p->currentToken = GetNextToken(lex);

          AstNode *ast = expr(lex, p);

          double result = evalAst(ast);
          char res[100];

          sprintf(res, "%.2lf", result);
          strcpy(input, res);

          free(lex->soruce);
          freeAST(ast);
          free(p);
          free(lex);
          break;
        case 'C':
          if (strlen(input) > 0) {
            input[strlen(input) - 1] = '\0';
          }
          break;
        case 'X':
          while (strlen(input) != 0) {
            input[strlen(input) - 1] = '\0';
          }
          break;
        case 'Q':
          exit(0);
        default:
          if (key.name != equalKey) {
            strcat(input, &key.name);
          }
        }
      }
    }
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

//-------------------------------Lexer---------------------------------------
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
    exit(1);
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

  Token *tkn = NewToken(ERR, "Error is whitespace");
  lexer->tokenCount++;
  return tkn;
}

//-----------------------------parser---------------------------------------
/*
expr    -> term (('+' | '-') term)*
term    -> factor (('*' | '/') factor)*
factor  -> NUMBER | '(' expr ')'
*/

void eat(TokenType type, Parser *p, Lexer *lex) {
  if (p->currentToken && type == p->currentToken->type) {
    p->currentToken = GetNextToken(lex);
  } else {
    exit(1);
  }
}

static double convertStrToDouble(char *s) {
  double val;
  sscanf(s, "%lf", &val);
  return val;
}
AstNode *newNumberNode(double value) {
  AstNode *node = (AstNode *)malloc(sizeof(AstNode));
  node->type = NODE_NUM;
  node->number = value;
  return node;
}
AstNode *newBinaryNode(TokenType op, AstNode *left, AstNode *right) {
  AstNode *node = (AstNode *)malloc(sizeof(AstNode));
  if (!node) {
    exit(1);
  }

  node->type = NODE_BINOP;
  node->binary_op.op = op;
  node->binary_op.left = left;
  node->binary_op.right = right;

  return node;
}

AstNode *expr(Lexer *l, Parser *p) {
  if (p->currentToken == NULL) {
    exit(1);
  }

  AstNode *node = term(l, p);

  while (p->currentToken &&
         (p->currentToken->type == PLUS || p->currentToken->type == MINUS)) {
    Token *tkn = p->currentToken;
    if (tkn == NULL) {
      exit(1);
    }

    eat(tkn->type, p, l);
    AstNode *right = term(l, p);
    node = newBinaryNode(tkn->type, node, right);
  }
  return node;
}

AstNode *factor(Lexer *l, Parser *p) {
  Token *token = p->currentToken;
  if (token->type == NUMBER) {
    eat(token->type, p, l);
    double value = convertStrToDouble(token->value);
    AstNode *node = newNumberNode(value);
    return node;

  } else if (token->type == LPAREN) {
    eat(LPAREN, p, l);
    AstNode *res = expr(l, p);
    eat(RPAREN, p, l);
    return res;

  } else {
    exit(1);
  }
}

AstNode *term(Lexer *l, Parser *p) {
  if (p == NULL) {
    exit(1);
  }

  if (p->currentToken == NULL) {
    exit(1);
  }

  AstNode *node = factor(l, p); // Assuming term calls factor

  while (p->currentToken && (p->currentToken->type == MULTIPLY ||
                             p->currentToken->type == DIVIDE)) {
    Token *tkn = p->currentToken;
    if (tkn == NULL) {
      exit(1);
    }

    eat(tkn->type, p, l); // Consume the operator
    AstNode *right = factor(l, p);

    node = newBinaryNode(tkn->type, node, right);
  }

  return node;
}

double evalAst(AstNode *node) {
  if (node->type == NODE_NUM) {
    return node->number;
  } else if (node->type == NODE_BINOP) {

    double left = evalAst(node->binary_op.left);
    double right = evalAst(node->binary_op.right);

    if (node->binary_op.op == PLUS) {
      return left + right;
    } else if (node->binary_op.op == MINUS) {
      return left - right;
    } else if (node->binary_op.op == DIVIDE) {
      return left / right;
    } else if (node->binary_op.op == MULTIPLY) {
      return left * right;
    } else {
      exit(1);
    }
  }
  return 0.0;
}

void freeAST(AstNode *node) {
  if (node->type == NODE_BINOP) {
    freeAST(node->binary_op.left);
    freeAST(node->binary_op.right);
  }
  free(node);
}
