#include "othello.h"

void Othello::setCell (Othello::Cell board[8][8], int x, int y, Color color, bool canMove)
{
  board[y][x].color_ = color;
  board[y][x].canMove_ = canMove;
}

Othello::Othello()
{
  for (int i = 0; i < BOARD_SIZE; i++)
  {
    for (int j = 0; j < BOARD_SIZE; j++)
    {
      setCell (board_, i, j, Empty, false);
    }
  }

  playerColor_ = Empty;
  curCmd_ = CMD_NULL;

  //DBG logFile << "Constructed" << std::endl;
}

void Othello::rcvCommand()
{
  std::string curCmd;

  std::cin >> curCmd;

  //DBG logFile << '\'' << curCmd << '\'' << std::endl;

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

  //DBG logFile << curCmd_ << std::endl;
}

void Othello::initGame()
{
  std::string playerColor;

  std::cin >> playerColor;

  if (playerColor[0] == 'b')
  {
    playerColor_ = Black;
  }
  else
  {
    playerColor_ = White;
  }

  setCell (board_, 3, 3, White, false);
  setCell (board_, 4, 4, White, false);

  setCell (board_, 4, 3, Black, false);
  setCell (board_, 3, 4, Black, false);

  //DBG logFile << "Me: " << playerColor_ << ", other: " << OPPONENT (playerColor_) << std::endl;
}

void Othello::getOpponentMove()
{
  char xPos;
  int yPos;

  std::cin >> xPos >> yPos;

  //DBG logFile << "He moved: " << xPos << ", " << yPos << std::endl;

  boardRecalc (board_, OPPONENT (playerColor_), xPos - 'a', yPos - 1);
}

void Othello::strategy()
{
  getValidMoves (board_, playerColor_);

  Move bestMove;// = scoring();
  double bestScore = 0.0;
  double alpha = -DBL_MAX;
  double beta = DBL_MAX;

  minimax (board_, 0, 6, bestScore, bestMove, playerColor_, true, alpha, beta); // ?

  std::cout << "move " << (char) (bestMove.xCoord_ + 'a') << ' ' << bestMove.yCoord_ + 1 << std::endl;

  boardRecalc (board_, playerColor_, bestMove.xCoord_, bestMove.yCoord_);

  getValidMoves (board_, playerColor_);
}

void Othello::negascout (Othello::Cell board[8][8], int depth, int maxDepth,
  double &chosenScore, Othello::Move &chosenMove, Color player, bool alphaBeta,
  double alpha, double beta)
{
  //DBG logFile << "Hi, that's minimax " << depth << " of " << maxDepth << '\n';
  if (depth == maxDepth)
  {
    // chosenScore = 0;
    // for (int i = 0; i < 8; i++)
    // {
    //   for (int j = 0; j < 8; j++)
    //   {
    //     if (board[i][j].color_ == playerColor_) chosenScore++;
    //     if (board[i][j].color_ == OPPONENT (playerColor_)) chosenScore--;
    //   }
    // }

    DBG logFile << "CHILD\n" << chosenScore << '\n';
    DBG printBoard (board);
    Color colorBoard[8][8] = {};

    for (int i = 0; i < 8; i++)
    {
      for (int j = 0; j < 8; j++)
      {
        colorBoard[i][j] = board[i][j].color_;
      }
    }

    chosenScore = strangeScoring (colorBoard);
    return;
  }

  std::vector<Othello::Move> movesList = getValidMoves (board, player);

  if (movesList.size() == 0)
  {

    // chosenScore = 0;
    // for (int i = 0; i < 8; i++)
    // {
    //   for (int j = 0; j < 8; j++)
    //   {
    //     if (board[i][j].color_ == playerColor_) chosenScore++;
    //     if (board[i][j].color_ == OPPONENT (playerColor_)) chosenScore--;
    //   }
    // }
    Color colorBoard[8][8] = {};

    for (int i = 0; i < 8; i++)
    {
      for (int j = 0; j < 8; j++)
      {
        colorBoard[i][j] = board[i][j].color_;
      }
    }

    chosenScore = strangeScoring (colorBoard);
    return;
  }

  if (player == playerColor_)
  {
    double bestScore = -DBL_MAX, theScore = 0.0;
    Othello::Move bestMove = {0, 0, 0}, theMove = {0, 0, 0};

    for (size_t i = 0; i < movesList.size(); i++)
    {
      Othello::Cell newBoard[8][8];

      for (int i = 0; i < 8; i++)
      {
        for (int j = 0; j < 8; j++)
        {
          newBoard[i][j] = board[i][j];
        }
      }

      applyMove (newBoard, movesList[i], playerColor_);

      // logFile << "DNSAUIDNASIDASIUDAS\n\n";
      // printBoard (newBoard);

      // std::cout << "HElllo" << std::endl;

      if (i == 0)
      {
        negascout (newBoard, depth + 1, maxDepth, theScore, theMove, OPPONENT (player), alphaBeta, alpha, beta);
        theScore *= -1;
      }
      else
      {
        double value;
        negascout (newBoard, depth + 1, maxDepth, value, theMove, OPPONENT (player), alphaBeta, theScore, theScore + 1);
        value *= -1;

        if (value > theScore)
        {
          if (value >= beta)
          {
            theScore = value;
          }
          else
            negascout (newBoard, depth + 1, maxDepth, theScore, theMove, OPPONENT (player), alphaBeta, value, beta);
        }

        // if (alpha < theScore &&
        //   theScore < beta)
        // {
        //     negascout (newBoard, depth + 1, maxDepth, theScore, theMove, OPPONENT (player), alphaBeta, -beta, -theScore);
        //     theScore *= -1;
        // }
      }

      if (alpha < theScore)
      {
        alpha = theScore;
        bestScore = theScore;
        bestMove = movesList[i];
      }

      if (alpha >= beta)
      {
        break;
      }

      // if (theScore > bestScore)
      // {
      //   // logFile << "GGGGGGGGGGGGGGGGGGGGGGGG\n";
      //   bestScore = theScore;
      //   bestMove = movesList[i];
      // }

      // logFile << theScore << ' ' << bestScore << ' ' << (char) (bestMove.xCoord_ + 'a') << ' ' << bestMove.yCoord_ + 1 << " HUI\n";

      // std::cout << "ME " << alpha << ' ' << beta << ' ' << theScore << ' ' << bestScore << (char) (bestMove.xCoord_ + 'a') << ' ' << bestMove.yCoord_ + 1 << " HUI\n";

      // std::cout << "HElllo" << std::endl;

      // if (theScore > bestScore)
      // {
      //   bestScore = theScore;
      //   bestMove = movesList[i];
      // }

      // bool cut = false;

      // // if (theScore > beta)
      // // {
      // //   cut = true;
      // // }

      // if (theScore > alpha)
      // {
      //   alpha = theScore;
      //   // bestMove = movesList[i];

      //   if (alpha >= beta)
      //     {
      //       // assert (!"CUT");
      //       cut = true;
      //     }
      // }

      // if (cut)
      //   return;
    }

    chosenScore = bestScore;
    chosenMove = bestMove;
  }
  else
  {
    double bestScore = DBL_MAX, theScore = 0.0;
    Othello::Move bestMove = {0, 0, 0}, theMove = {0, 0, 0};

    for (size_t i = 0; i < movesList.size(); i++)
    {
      Othello::Cell newBoard[8][8];

      for (int i = 0; i < 8; i++)
      {
        for (int j = 0; j < 8; j++)
        {
          newBoard[i][j] = board[i][j];
        }
      }

      applyMove (newBoard, movesList[i], OPPONENT (playerColor_));

            if (i == 0)
      {
        negascout (newBoard, depth + 1, maxDepth, theScore, theMove, OPPONENT (player), alphaBeta, alpha, beta);
        theScore *= -1;
      }
      else
      {
        double value;
        negascout (newBoard, depth + 1, maxDepth, value, theMove, OPPONENT (player), alphaBeta, theScore, theScore + 1);
        value *= -1;

        if (value <= theScore)
        {
          if (value <= alpha)
          {
            theScore = value;
          }
          else
            negascout (newBoard, depth + 1, maxDepth, theScore, theMove, OPPONENT (player), alphaBeta, alpha, value);
        }
      }

      if (beta > theScore)
      {
        beta = theScore;
        bestScore = theScore;
        bestMove = movesList[i];
      }

      if (alpha >= beta)
      {
        break;
      }

      // minimax (newBoard, depth + 1, maxDepth, theScore, theMove, OPPONENT (player), alphaBeta, alpha, beta);

      // // if (theScore < bestScore)
      // // {
      // //   bestScore = theScore;
      // //   bestMove = movesList[i];
      // // }

      // bool cut = false;



      // // // logFile << "HIM " << alpha << ' ' << beta << ' ' << theScore << ' ' << bestScore << (char) (bestMove.xCoord_ + 'a') << ' ' << bestMove.yCoord_ + 1 << " HUI\n";
      // // // std::cout << theScore << ' ' << alpha << ' ' << beta << std::endl;

      // if (theScore < bestScore)
      // {
      //   bestScore = theScore;
      //   bestMove = movesList[i];
      // }

      // if (theScore < beta)
      // {
      //   beta = theScore;
      //   bestScore = theScore;
      //   bestMove = movesList[i];

      //   if (alpha >= beta)
      //   {
      //       // assert (!"CUT");
      //       cut = true;
      //     }
      // }

      // if (cut)
      //   return;

    }

    chosenScore = bestScore;
    chosenMove = bestMove;
  }

  DBG logFile << player << ' ' << depth << ' ' << (char) (chosenMove.xCoord_ + 'a') << ' ' << (chosenMove.yCoord_ + 1) << '\n';
}

void Othello::minimax (Othello::Cell board[8][8], int depth, int maxDepth,
  double &chosenScore, Othello::Move &chosenMove, Color player, bool alphaBeta,
  double alpha, double beta)
{
  //DBG logFile << "Hi, that's minimax " << depth << " of " << maxDepth << '\n';
  if (depth == maxDepth)
  {
    // chosenScore = 0;
    // for (int i = 0; i < 8; i++)
    // {
    //   for (int j = 0; j < 8; j++)
    //   {
    //     if (board[i][j].color_ == playerColor_) chosenScore++;
    //     if (board[i][j].color_ == OPPONENT (playerColor_)) chosenScore--;
    //   }
    // }

    DBG logFile << "CHILD\n" << chosenScore << '\n';
    DBG printBoard (board);
    Color colorBoard[8][8] = {};

    for (int i = 0; i < 8; i++)
    {
      for (int j = 0; j < 8; j++)
      {
        colorBoard[i][j] = board[i][j].color_;
      }
    }

    chosenScore = strangeScoring (colorBoard);
    return;
  }

  std::vector<Othello::Move> movesList = getValidMoves (board, player);

  if (movesList.size() == 0)
  {

    // chosenScore = 0;
    // for (int i = 0; i < 8; i++)
    // {
    //   for (int j = 0; j < 8; j++)
    //   {
    //     if (board[i][j].color_ == playerColor_) chosenScore++;
    //     if (board[i][j].color_ == OPPONENT (playerColor_)) chosenScore--;
    //   }
    // }
    Color colorBoard[8][8] = {};

    for (int i = 0; i < 8; i++)
    {
      for (int j = 0; j < 8; j++)
      {
        colorBoard[i][j] = board[i][j].color_;
      }
    }

    chosenScore = strangeScoring (colorBoard);
    return;
  }

  if (player == playerColor_)
  {
    double bestScore = -DBL_MAX, theScore = 0.0;
    Othello::Move bestMove = {0, 0, 0}, theMove = {0, 0, 0};

    for (size_t i = 0; i < movesList.size(); i++)
    {
      Othello::Cell newBoard[8][8];

      for (int i = 0; i < 8; i++)
      {
        for (int j = 0; j < 8; j++)
        {
          newBoard[i][j] = board[i][j];
        }
      }

      applyMove (newBoard, movesList[i], playerColor_);

      // logFile << "DNSAUIDNASIDASIUDAS\n\n";
      // printBoard (newBoard);

      // std::cout << "HElllo" << std::endl;

      minimax (newBoard, depth + 1, maxDepth, theScore, theMove, OPPONENT (player), alphaBeta, alpha, beta);

      // if (theScore > bestScore)
      // {
      //   // logFile << "GGGGGGGGGGGGGGGGGGGGGGGG\n";
      //   bestScore = theScore;
      //   bestMove = movesList[i];
      // }

      // logFile << theScore << ' ' << bestScore << ' ' << (char) (bestMove.xCoord_ + 'a') << ' ' << bestMove.yCoord_ + 1 << " HUI\n";

      // std::cout << "ME " << alpha << ' ' << beta << ' ' << theScore << ' ' << bestScore << (char) (bestMove.xCoord_ + 'a') << ' ' << bestMove.yCoord_ + 1 << " HUI\n";

      // std::cout << "HElllo" << std::endl;

      if (theScore > bestScore)
      {
        bestScore = theScore;
        bestMove = movesList[i];
      }

      bool cut = false;

      // if (theScore > beta)
      // {
      //   cut = true;
      // }

      if (theScore > alpha)
      {
        alpha = theScore;
        // bestMove = movesList[i];

        if (alpha >= beta)
          {
            // assert (!"CUT");
            cut = true;
          }
      }

      if (cut)
        return;
    }

    chosenScore = bestScore;
    chosenMove = bestMove;
  }
  else
  {
    double bestScore = DBL_MAX, theScore = 0.0;
    Othello::Move bestMove = {0, 0, 0}, theMove = {0, 0, 0};

    for (size_t i = 0; i < movesList.size(); i++)
    {
      Othello::Cell newBoard[8][8];

      for (int i = 0; i < 8; i++)
      {
        for (int j = 0; j < 8; j++)
        {
          newBoard[i][j] = board[i][j];
        }
      }

      applyMove (newBoard, movesList[i], OPPONENT (playerColor_));

      minimax (newBoard, depth + 1, maxDepth, theScore, theMove, OPPONENT (player), alphaBeta, alpha, beta);

      // if (theScore < bestScore)
      // {
      //   bestScore = theScore;
      //   bestMove = movesList[i];
      // }

      bool cut = false;



      // // logFile << "HIM " << alpha << ' ' << beta << ' ' << theScore << ' ' << bestScore << (char) (bestMove.xCoord_ + 'a') << ' ' << bestMove.yCoord_ + 1 << " HUI\n";
      // // std::cout << theScore << ' ' << alpha << ' ' << beta << std::endl;

      if (theScore < bestScore)
      {
        bestScore = theScore;
        bestMove = movesList[i];
      }

      if (theScore < beta)
      {
        beta = theScore;
        bestScore = theScore;
        bestMove = movesList[i];

        if (alpha >= beta)
        {
            // assert (!"CUT");
            cut = true;
          }
      }

      if (cut)
        return;

    }

    chosenScore = bestScore;
    chosenMove = bestMove;
  }

  DBG logFile << player << ' ' << depth << ' ' << (char) (chosenMove.xCoord_ + 'a') << ' ' << (chosenMove.yCoord_ + 1) << '\n';
}

void Othello::applyMove (Othello::Cell board[8][8], Othello::Move move, Color mover)
{
  // logFile << "RECALC" << mover << (char) (move.xCoord_ + 'a') << ' ' << move.yCoord_ + 1 <<  '\n';
  boardRecalc (board, mover, move.xCoord_, move.yCoord_);
  // printBoard (board);
}

void Othello::moveLookup (Othello::Cell board[8][8], Action action, Color curPlayerColor, int xCoord, int yCoord)
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
          if (board[y][x].color_ == Empty) break;

          if (board[y][x].color_ == curPlayerColor &&
            wasOpponent == false) break;

          if (board[y][x].color_ == OPPONENT (curPlayerColor)) wasOpponent = true;

          if (board[y][x].color_ == curPlayerColor &&
            wasOpponent == true)
          {
            if (action == GET_VALID_MOVES)
            {
              Move valid;

              valid.xCoord_ = xCoord;
              valid.yCoord_ = yCoord;
              valid.prize_ = distance;

              validMoves_.push_back (valid);

              board[yCoord][xCoord].canMove_ = true;

              break;
            }

            if (action == BOARD_RECALC)
            {
              //DBG logFile << curPlayerColor << " gained: " << distance - 1 << std::endl;
              for (int markIndex = 1; markIndex < distance; markIndex++)
              {
                setCell (board, xCoord + markIndex * dx, yCoord + markIndex * dy, curPlayerColor, false);
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

void Othello::boardRecalc (Othello::Cell board[8][8], Color curPlayerColor, int xCoord, int yCoord)
{
  //DBG logFile << "Board recalc: " << curPlayerColor << std::endl;

  setCell (board, xCoord, yCoord, curPlayerColor, false);

  moveLookup (board, BOARD_RECALC, curPlayerColor, xCoord, yCoord);
}

std::vector <Othello::Move> &Othello::getValidMoves (Othello::Cell board[8][8], Color player)
{
  validMoves_.clear();

  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      board[j][i].canMove_ = false;

      if (board[j][i].color_ == Empty)
      {
        moveLookup (board, GET_VALID_MOVES, player, i, j);
      }
    }
  }

  return validMoves_;
}

int Othello::numValidMoves()
{
  validMoves_.clear();

  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      board_[j][i].canMove_ = false;

      if (board_[j][i].color_ == Empty)
      {
        moveLookup (board_, GET_VALID_MOVES, playerColor_, i, j);
      }
    }
  }

  return validMoves_.size();
}

void Othello::game()
{
  logFile.open ("log.txt");
  logFile << "Log file for Othello. (c) AQ";

  while (1)
  {
    //DBG logFile << "-----\n\n";

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
        //DBG logFile << "Endgame" << std::endl;
        return;

      case CMD_NULL:
      default:
        assert (!"Wrong cmd!");
    }

    //DBG printBoard(board_);
  }
}

void Othello::printBoard (Othello::Cell board[8][8])
{
  logFile << "Game\ncolor = " << ((playerColor_ == Black) ? "Black" : "White") << "\n\t";

  for (int j = 0; j < BOARD_SIZE; j++)
  {
    logFile << "  " << ( char) (j + 'a') << "\t";
  }
  logFile << std::endl;

  for (int i = 0; i < BOARD_SIZE; i++)
  {
    logFile << i + 1 << "\t";
    for (int j = 0; j < BOARD_SIZE; j++)
    {
      logFile << "{" << ((board[i][j].color_ == Empty) ? "_" : (board[i][j].color_ == Black) ? "B" : "W") << ", " << ((board[i][j].canMove_ == true) ? "+" : " ") << "}\t";
    }
    logFile << std::endl;
  }

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
      std::cerr << "{" << ((board[i][j].color_ == Empty) ? "_" : (board[i][j].color_ == Black) ? "B" : "W") << ", " << ((board[i][j].canMove_ == true) ? "+" : " ") << "}\t";
    }
    std::cerr << std::endl;
  }
}

double Othello::strangeScoring (Color grid[8][8])
{
  int my_tiles = 0, opp_tiles = 0, i, j, k, my_front_tiles = 0, opp_front_tiles = 0, x, y;
  double p = 0, c = 0, l = 0, m = 0, f = 0, d = 0;

  int X1[] = {-1, -1, 0, 1, 1, 1, 0, -1};
  int Y1[] = {0, 1, 1, 1, 0, -1, -1, -1};
  int V[8][8];


  auto init = std::initializer_list<int> {20, -3, 11, 8, 8, 11, -3, 20};
  std::copy (init.begin(), init.end(), V[0]);
  init = std::initializer_list<int> {-3, -7, -4, 1, 1, -4, -7, -3};
  std::copy (init.begin(), init.end(), V[1]);
   init = std::initializer_list<int> {11, -4, 2, 2, 2, 2, -4, 11};
  std::copy (init.begin(), init.end(), V[2]);
   init = std::initializer_list<int> {8, 1, 2, -3, -3, 2, 1, 8};
  std::copy (init.begin(), init.end(), V[3]);
   init = std::initializer_list<int> {8, 1, 2, -3, -3, 2, 1, 8};
  std::copy (init.begin(), init.end(), V[4]);
   init = std::initializer_list<int> {11, -4, 2, 2, 2, 2, -4, 11};
  std::copy (init.begin(), init.end(), V[5]);
   init = std::initializer_list<int> {-3, -7, -4, 1, 1, -4, -7, -3};
  std::copy (init.begin(), init.end(), V[6]);
   init = std::initializer_list<int> {20, -3, 11, 8, 8, 11, -3, 20};
  std::copy (init.begin(), init.end(), V[7]);

// Piece difference, frontier disks and disk squares
  for(i=0; i<8; i++)
    for(j=0; j<8; j++)  {
      if(grid[i][j] == playerColor_)  {
        d += V[i][j];
        my_tiles++;
      } else if(grid[i][j] == OPPONENT (playerColor_))  {
        d -= V[i][j];
        opp_tiles++;
      }
      if(grid[i][j] != Empty)   {
        for(k=0; k<8; k++)  {
          x = i + X1[k]; y = j + Y1[k];
          if(x >= 0 && x < 8 && y >= 0 && y < 8 && grid[x][y] == Empty) {
            if(grid[i][j] == playerColor_)  my_front_tiles++;
            else opp_front_tiles++;
            break;
          }
        }
      }
    }
  if(my_tiles > opp_tiles)
    p = (100.0 * my_tiles)/(my_tiles + opp_tiles);
  else if(my_tiles < opp_tiles)
    p = -(100.0 * opp_tiles)/(my_tiles + opp_tiles);
  else p = 0;

  if(my_front_tiles > opp_front_tiles)
    f = -(100.0 * my_front_tiles)/(my_front_tiles + opp_front_tiles);
  else if(my_front_tiles < opp_front_tiles)
    f = (100.0 * opp_front_tiles)/(my_front_tiles + opp_front_tiles);
  else f = 0;

// Corner occupancy
  my_tiles = opp_tiles = 0;
  if(grid[0][0] == playerColor_) my_tiles++;
  else if(grid[0][0] == OPPONENT (playerColor_)) opp_tiles++;
  if(grid[0][7] == playerColor_) my_tiles++;
  else if(grid[0][7] == OPPONENT (playerColor_)) opp_tiles++;
  if(grid[7][0] == playerColor_) my_tiles++;
  else if(grid[7][0] == OPPONENT (playerColor_)) opp_tiles++;
  if(grid[7][7] == playerColor_) my_tiles++;
  else if(grid[7][7] == OPPONENT (playerColor_)) opp_tiles++;
  c = 25 * (my_tiles - opp_tiles);

// Corner closeness
  my_tiles = opp_tiles = 0;
  if(grid[0][0] == Empty)   {
    if(grid[0][1] == playerColor_) my_tiles++;
    else if(grid[0][1] == OPPONENT (playerColor_)) opp_tiles++;
    if(grid[1][1] == playerColor_) my_tiles++;
    else if(grid[1][1] == OPPONENT (playerColor_)) opp_tiles++;
    if(grid[1][0] == playerColor_) my_tiles++;
    else if(grid[1][0] == OPPONENT (playerColor_)) opp_tiles++;
  }
  if(grid[0][7] == Empty)   {
    if(grid[0][6] == playerColor_) my_tiles++;
    else if(grid[0][6] == OPPONENT (playerColor_)) opp_tiles++;
    if(grid[1][6] == playerColor_) my_tiles++;
    else if(grid[1][6] == OPPONENT (playerColor_)) opp_tiles++;
    if(grid[1][7] == playerColor_) my_tiles++;
    else if(grid[1][7] == OPPONENT (playerColor_)) opp_tiles++;
  }
  if(grid[7][0] == Empty)   {
    if(grid[7][1] == playerColor_) my_tiles++;
    else if(grid[7][1] == OPPONENT (playerColor_)) opp_tiles++;
    if(grid[6][1] == playerColor_) my_tiles++;
    else if(grid[6][1] == OPPONENT (playerColor_)) opp_tiles++;
    if(grid[6][0] == playerColor_) my_tiles++;
    else if(grid[6][0] == OPPONENT (playerColor_)) opp_tiles++;
  }
  if(grid[7][7] == Empty)   {
    if(grid[6][7] == playerColor_) my_tiles++;
    else if(grid[6][7] == OPPONENT (playerColor_)) opp_tiles++;
    if(grid[6][6] == playerColor_) my_tiles++;
    else if(grid[6][6] == OPPONENT (playerColor_)) opp_tiles++;
    if(grid[7][6] == playerColor_) my_tiles++;
    else if(grid[7][6] == OPPONENT (playerColor_)) opp_tiles++;
  }
  l = -12.5 * (my_tiles - opp_tiles);

// Mobility
  my_tiles = numValidMoves();
  opp_tiles = numValidMoves();
  if(my_tiles > opp_tiles)
    m = (100.0 * my_tiles)/(my_tiles + opp_tiles);
  else if(my_tiles < opp_tiles)
    m = -(100.0 * opp_tiles)/(my_tiles + opp_tiles);
  else m = 0;

// final weighted score
  double score = (10 * p) + (801.724 * c) + (382.026 * l) + (78.922 * m) + (74.396 * f) + (10 * d) + 10000.0;

  //DBG logFile << score << std::endl;
  return score;
}


Othello::Move Othello::random()
{
  Move bestMoveRandom;

  bestMoveRandom = validMoves_[rand() % validMoves_.size()];

  return bestMoveRandom;
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

Othello::Move Othello::scoring()
{
  Move bestMoveScoring = {0, 0, 0};

  bestMoveScoring.prize_ = 0;

  for (size_t i = 0; i < validMoves_.size(); i++)
  {
    // std::cout << "Try " << validMoves_[i].xCoord_ << ' ' << validMoves_[i].yCoord_ << std::endl;
    Othello::Cell buf[8][8] = {};
    std::vector <Move> MovesBuf = validMoves_;

    std::copy(std::begin(board_), std::end(board_), std::begin(buf));

    boardRecalc (board_, playerColor_, validMoves_[i].xCoord_, validMoves_[i].yCoord_);

    // printBoard();

    Color colorBoard[8][8] = {};

    for (int i = 0; i < 8; i++)
    {
      for (int j = 0; j < 8; j++)
      {
        colorBoard[i][j] = board_[i][j].color_;
      }
    }

    std::copy(std::begin(buf), std::end(buf), std::begin(board_));
    getValidMoves (board_, playerColor_);

    // for (size_t k = 0; k < validMoves_.size(); k++)
    // {
    //   std::cout << validMoves_[k].xCoord_ << ',' << validMoves_[k].yCoord_ << '\n';
    // }

    if (strangeScoring (colorBoard) > bestMoveScoring.prize_)
      {
    // std::cout << bestMoveScoring.xCoord_ << ' ' << bestMoveScoring.yCoord_ << std::endl;
    // std::cout << validMoves_[i].xCoord_ << ' ' << validMoves_[i].yCoord_ << std::endl;
        bestMoveScoring.xCoord_ = validMoves_[i].xCoord_;
        bestMoveScoring.yCoord_ = validMoves_[i].yCoord_;
      }
  }


    // std::cout << bestMoveScoring.xCoord_ << ' ' << bestMoveScoring.yCoord_ << std::endl;


  return bestMoveScoring;
}
