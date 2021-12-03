#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define USER 1
#define COMPUTER 0
#define MAX_GAME_LENGTH 100
#define INFINITY 1000
#define PAUSE_AT_END _defined_
// #define WORST_PLAY _defined_

#define playMove(move, player) if((board[player ? move&1 : (move>>1)|2] += board[player ? (move>>1)|2 : move&1]) > 4) \
                                board[player ? move&1 : (move>>1)|2] -= 5
#define undoMove(move, player) if((board[player ? move&1 : (move>>1)|2] -= board[player ? (move>>1)|2 : move&1]) < 0) \
                                board[player ? move&1 : (move>>1)|2] += 5
#define isValid(move, player) (board[move&1] && board[(move>>1)|2])
#define isGameOver() (board[0] | board[1] ? !(board[2] | board[3]) : -1)

void print_board(int* board){
  int i, j;
  for (i=0; i<2; ++i, printf("\n"))
    for (j=0; j<2; ++j, printf(" "))
      printf("%d", board[i*2+j]);
  printf("\n");
}

int negascout(int* board, int depth, int alpha, int beta, int player){ //Negascout algorithm
  int nodeScore = isGameOver(); //See if the game has finished in the board
  if(nodeScore || !depth) return nodeScore * player * depth; //Calculate score

  int move, score, i, j=0;
  for(i=0; i<4; ++i){ //Search all moves
    if(isValid(i, player<0)) { //If legal
      playMove(i, player<0); //Try the move
      if (!j++){ //If child is first child
        score = -negascout(board, depth-1, -beta, -alpha, -player); //Do normal alpha-beta pruning
        move = i;
      } else {
        score = -negascout(board, depth-1, -alpha-1, -alpha, -player); //Search with a null window
        move = i;
        if (alpha < score && score < beta){ //If it failed high
          score = -negascout(board, depth-1, -beta, -score, -player); //Do a full research
          move = i;
        }
      }
      undoMove(i, player<0); //Undo the move
      if (score > alpha) alpha = score;
      if (alpha >= beta) break; //Beta cut off
    }
  }
  return alpha; //Return score
}

void computerMove(int* board, int depth){
//Search all nodes at 0 depth and display scores and play the best move
#ifdef WORST_PLAY
  int bestScore = +INFINITY;
#else
  int bestScore = -INFINITY;
#endif /* WORST_PLAY */
  int scores[4]={0}, bestMoves[4]={0}, index=0, i;
  for(i=0; i<4; ++i){ //Search all moves
    if(isValid(i, COMPUTER)){ //If legal
      playMove(i, COMPUTER); //Try it
      scores[i] = -negascout(board, depth, -INFINITY, +INFINITY, -1); //Find if the move is bad for user
#ifdef WORST_PLAY
      if (scores[i] < bestScore) //If a better score is found
#else
      if (scores[i] > bestScore) //If a better score is found
#endif /* WORST_PLAY */
        bestScore = scores[i]; //Update it
      undoMove(i, COMPUTER); //Undo it
    }
  }
  printf("\n");
  for (i=0; i<4; ++i) //Search all moves
    if (isValid(i, COMPUTER)){ //If legal
      printf("Move number %d score: %d\n", i+1, scores[i]); //Print the score
      if (bestScore == scores[i]) //If it is one of the best scores
        bestMoves[index++] = i; //Save it
    }
  int randomMove = bestMoves[rand()%index]; //Choose a random one out of the best ones
  printf("\nChose move number %d, out of %d best move(s)\n\n", randomMove+1, index);
  playMove(randomMove, COMPUTER);
}

void playUser(int* board){
  int move;
  for (;;){
    printf("Enter Move: ");
    scanf("%d", &move);
    printf("\n");
    if (move>0 && move<5 && isValid(move-1, USER))
      break;
  }
  playMove(move-1, USER);
}

int main (){
  printf("# Chopsticks - Build 9 by Swastik Mozumder #\n\n");
  printf("Development history:\n* Used bitwise instructions to play and undo moves\n");
  printf("* Displayed score of all nodes to the user\n");
  printf("* Used alpha-beta algorithm with negamax\n");
  printf("* Used negascout algorithm\n");
  printf("* Introduced better depth handling\n");
  printf("* Enabled worst play too\n");
  printf("* Enabled the user to input custom depth\n\n");
  printf("INSTRUCTIONS TO PLAY:\n");
  printf("Input depth to set computer's strength of thinking (in plys)\n");
  printf("Input your chance to play\n");
  printf("Input moves:\n1 - hit your left number to computer's left number\n");
  printf("2 - hit your left number to computer's right number\n");
  printf("3 - hit your right number to computer's left number\n");
  printf("4 - hit your right number to computer's right number\n");
  printf("For computer's perspective, it is just the opposite like:\n");
  printf("1 means hitting user's left number to computer's left number\n");
  printf("2 means hitting user's left number to computer's right number and so on...\n\n");
  printf("Play (1)st or (2)nd?: ");
  int board[4]={1, 1, 1, 1}, turn, player, depth;
  scanf("%d",&player);
  printf("\nEnter maximum depth: ");
  scanf("%d", &depth);
  printf("\n");
  print_board(board);
  srand(time(NULL));
  for(turn=0; turn<MAX_GAME_LENGTH && !isGameOver(); ++turn){
    if((turn + player) & 1) //Find whose turn
      playUser(board);
    else
      computerMove(board, depth);
    print_board(board);
  }
  printf("\n");
  switch(isGameOver()){ //Check the end of the game
    case 0:
      printf("DRAW!!!\n");
      break;
    case 1:
#ifdef WORST_PLAY
      printf("YOU LOST!!! How?!\n");
#else
      printf("YOU LOST!!!");
#endif
      break;
    case -1:
#ifdef WORST_PLAY
      printf("YOU WON!!!");
#else
      printf("YOU WON!!! Inconceivable!\n");
#endif
      break;
  }
#ifdef PAUSE_AT_END
  getchar();
  printf("\n\nPress any key to continue . . . \n");
  getchar();
#endif /* PAUSE_AT_END */
  return 0;
}
