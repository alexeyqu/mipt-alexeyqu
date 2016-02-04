#ifndef _OTHELLO_LIB__
#define _OTHELLO_LIB__

#include <iostream>
#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include <string>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#define BOARD_SIZE 8

#define DEBUG

#ifdef DEBUG 
  #define DBG std::cerr << "DBG: ";
#else
  #define DBG if(0)
#endif

enum Color
{
  Empty,
  Black,
  White
};

#define OPPONENT( color ) (Color) (3 - color)

enum Command
{
  CMD_NULL = 1000,
  CMD_INIT = 1001,
  CMD_TURN = 1002,
  CMD_MOVE = 1003,
  CMD_BAD = 1004,
  CMD_LOSE = 1005,
  CMD_WIN = 1006,
  CMD_DRAW = 1007
};

enum Action
{
  GET_VALID_MOVES,
  BOARD_RECALC
};

class Othello
{
public:
  class Cell
  {
  public:
    Color color_;
    bool canMove_;
  };

  class Move
  {
  public:
    int xCoord_, yCoord_;
    int prize_;
  };

  Color playerColor_;
  Command curCmd_;
  Cell board_[BOARD_SIZE][BOARD_SIZE];
  std::vector <Move> validMoves_;

  Othello();

  void setCell (int x, int y, Color color, bool canMove);

  void initGame();

  void rcvCommand();

  void getValidMoves();

  void boardRecalc (Color curPlayer, int xCoord, int yCoord);

  void strategy();

  void getOpponentMove();

  void moveLookup (Action action, Color curPlayerColor, int xCoord, int yCoord);

  Othello::Move greedy();

  void game();

  void printBoard();
};

#endif