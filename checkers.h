#ifndef CHECKERS_H
#define CHECKERS_H
#include <stdbool.h> // for boolean values

// Game state constants
#define GAME_IN_PROGRESS 0
#define RED_WINS 1
#define BLACK_WINS 2
#define DRAW 3

// Game modes
#define HUMAN_VS_HUMAN 1
#define HUMAN_VS_COMPUTER 2

typedef enum PieceType {
    EMPTY,      
    RED,      
    BLACK,     
    RED_KING,   
    BLACK_KING 
} PieceType;

// Structure for a single checker piece
typedef struct {
    PieceType type;
} Piece;

// Structure for a possible move
typedef struct {
    int srcRow, srcCol;  
    int destRow, destCol; 
    bool isCapture;      
} Move;

// Structure for the board (2D grid)
typedef struct {
    Piece** grid;
    int size;
    int redPieces;    
    int blackPieces;  
    int gameState;    
} Board;

// Function prototypes
Board* createBoard(int size);
void initializeBoard(Board* board);
void displayBoard(Board* board);
void freeBoard(Board* board);

// Movement functions
bool movePiece(Board* board, int srcRow, int srcCol, int destRow, int destCol, int playerTurn);
bool isValidMove(Board* board, int srcRow, int srcCol, int destRow, int destCol, int playerTurn);
bool isValidJump(Board* board, int srcRow, int srcCol, int destRow, int destCol, int playerTurn);
bool checkForMultipleJumps(Board* board, int row, int col, int playerTurn);
bool hasValidMoves(Board* board, int playerTurn);

// Game state functions
void updateGameState(Board* board);
void countPieces(Board* board);
int getGameState(Board* board);

// Save/Load functions
void saveGame(Board* board, const char* filename, int currentPlayer);
Board* loadGame(const char* filename, int* currentPlayer);

// Helper functions
void displayHelp();
void displayGameStatus(Board* board, int currentPlayer);

// Computer functions
Move getComputerMove(Board* board);  
void makeComputerMove(Board* board); 

#endif