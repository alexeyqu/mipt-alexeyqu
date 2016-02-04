#include "othello.h"

void Othello::setCell (int x, int y, Color color, bool canMove)
{
  board_[y][x].color_ = color;
  board_[y][x].canMove_ = canMove;
}

Othello::Othello()
{
  for (int i = 0; i < BOARD_SIZE; i++)
  {
    for (int j = 0; j < BOARD_SIZE; j++)
    {
      setCell (i, j, Empty, false);
    }
  }

  playerColor_ = Empty;
  curCmd_ = CMD_NULL;

  DBG std::cerr << "Constructed" << std::endl;
}

void Othello::rcvCommand()
{
  std::string curCmd;

  std::cin >> curCmd;

  DBG std::cerr << '\'' << curCmd << '\'' << std::endl;

  switch (curCmd[0])
  {
    case 'i':
      curCmd_ = CMD_INIT;
      break;

    case 't':
      curCmd_ = CMD_TURN;
      break;

    case 'm':
      curCmd_ = CMD_MOVE;
      break;

    case 'b':
      curCmd_ = CMD_BAD;
      break;

    case 'l':
      curCmd_ = CMD_LOSE;
      break;

    case 'w':
      curCmd_ = CMD_WIN;
      break;

    case 'd':
      curCmd_ = CMD_DRAW;
      break;

    default:
      assert (!"Wrong cmd!");
  }

  DBG std::cerr << curCmd_ << std::endl;
}

void Othello::initGame()
{
  std::string playerColor;

  std::cin >> playerColor;

  if (playerColor[0] == 'b')
  {
    playerColor_ = Black;
    // opponentColor_ = White;
  }
  else 
  {
    playerColor_ = White;
    // opponentColor_ = Black;
  }

  setCell (3, 3, White, false);
  setCell (4, 4, White, false);

  setCell (4, 3, Black, false);
  setCell (3, 4, Black, false);

  DBG std::cerr << "Me: " << playerColor_ << ", other: " << OPPONENT (playerColor_) << std::endl;
}

void Othello::getOpponentMove()
{
  char xPos;
  int yPos;

  std::cin >> xPos >> yPos;

  DBG std::cerr << "He moved: " << xPos << ", " << yPos - 1 << std::endl;

  boardRecalc (OPPONENT (playerColor_), xPos - 'a', yPos - 1);
}

void Othello::strategy()
{
  getValidMoves();

  Move bestMove = greedy();

  std::cout << "move " << (char) (bestMove.xCoord_ + 'a') << ' ' << bestMove.yCoord_ + 1 << std::endl;

  boardRecalc (playerColor_, bestMove.xCoord_, bestMove.yCoord_);

  getValidMoves();
}

Othello::Move Othello::greedy()
{
  Move bestMoveGreedy;

  bestMoveGreedy.prize_ = 0;

  for (size_t i = 0; i < validMoves_.size(); i++)
  {
    if (validMoves_[i].prize_ > bestMoveGreedy.prize_) bestMoveGreedy = validMoves_[i];
  }

  return bestMoveGreedy;
}

void Othello::moveLookup (Action action, Color curPlayerColor, int xCoord, int yCoord)
{
  for (int dy = -1; dy <= 1; dy++)
  {
    for (int dx = -1; dx <= 1; dx++)
    {
      if (!dx && !dy) continue;

      bool wasOpponent = false;

      for (int distance = 1; distance < 8; distance++)
      {
        int x = xCoord + distance * dx;
        int y = yCoord + distance * dy;

        if (x >= 0 && y >= 0 
          && x < 8 && y < 8)
        {
          if (board_[y][x].color_ == Empty) break;

          if (board_[y][x].color_ == curPlayerColor &&
            wasOpponent == false) break;

          if (board_[y][x].color_ == OPPONENT (curPlayerColor)) wasOpponent = true;

          if (board_[y][x].color_ == curPlayerColor &&
            wasOpponent == true)
          {
            if (action == GET_VALID_MOVES)
            {
              Move valid;

              valid.xCoord_ = xCoord;
              valid.yCoord_ = yCoord;
              valid.prize_ = distance;

              validMoves_.push_back (valid);

              board_[yCoord][xCoord].canMove_ = true; 

              break;
            }
            
            if (action == BOARD_RECALC)
            {
              DBG std::cerr << curPlayerColor << " gained: " << distance - 1 << std::endl;
              for (int markIndex = 1; markIndex < distance; markIndex++)
              {
                setCell (xCoord + markIndex * dx, yCoord + markIndex * dy, curPlayerColor, false);
              }

              break;
            }
          }
        }
        else break;
      }
    }
  }
}

void Othello::boardRecalc (Color curPlayerColor, int xCoord, int yCoord)
{
  DBG std::cerr << "Board recalc: " << curPlayerColor << std::endl;

  setCell (xCoord, yCoord, curPlayerColor, false);

  moveLookup (BOARD_RECALC, curPlayerColor, xCoord, yCoord);
}

void Othello::getValidMoves()
{
  validMoves_.clear();

  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      board_[j][i].canMove_ = false;

      if (board_[j][i].color_ == Empty)
      {
        moveLookup (GET_VALID_MOVES, playerColor_, i, j);
      }
    }
  }
}

void Othello::game()
{
  while (1)
  {
    DBG std::cerr << "-----\n\n";

    rcvCommand();

    switch (curCmd_)
    {
      case CMD_INIT:
        initGame();
        break;

      case CMD_TURN:
        strategy();
        break;

      case CMD_MOVE:
        getOpponentMove();
        break;

      case CMD_BAD:
      case CMD_WIN:
      case CMD_LOSE:
      case CMD_DRAW:
        DBG std::cerr << "Endgame" << std::endl;
        return;

      case CMD_NULL:
      default:
        assert (!"Wrong cmd!");
    }

    DBG printBoard();
  }
}

void Othello::printBoard()
{
  std::cerr << "Game\ncolor = " << ((playerColor_ == Black) ? "Black" : "White") << "\n\t";

  for (int j = 0; j < BOARD_SIZE; j++)
  {
    std::cerr << "  " << ( char) (j + 'a') << "\t";
  }
  std::cerr << std::endl;

  for (int i = 0; i < BOARD_SIZE; i++)
  {
    std::cerr << i + 1 << "\t";
    for (int j = 0; j < BOARD_SIZE; j++)
    {
      std::cerr << "{" << ((board_[i][j].color_ == Empty) ? "_" : (board_[i][j].color_ == Black) ? "B" : "W") << ", " << ((board_[i][j].canMove_ == true) ? "+" : " ") << "}\t";
    }
    std::cerr << std::endl;
  }
}