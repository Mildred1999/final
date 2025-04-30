#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "checkers.h"

// ANSI color codes for better visualization
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_BLACK   "\x1b[30m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_BG_LIGHT      "\x1b[47m"
#define ANSI_BG_DARK       "\x1b[43m"

// create a new board
Board* createBoard(int size){
    Board* board = (Board *) malloc(sizeof(Board));
    board->size = size;
    board->grid = (Piece **) malloc(size * sizeof(Piece*));
    board->redPieces = 0;
    board->blackPieces = 0;
    board->gameState = GAME_IN_PROGRESS;

    for(int i = 0; i < size; i++){
        board->grid[i] = (Piece*) malloc(size * sizeof(Piece));
        for(int j = 0; j < size; j++){
            board->grid[i][j].type = EMPTY;
        }
    }
    return board;
}

// Initialize board with starting positions
void initializeBoard(Board* board){
    int size = board->size;
    board->redPieces = 0;
    board->blackPieces = 0;

    for (int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            if (i < 3 && (i + j) % 2 == 1){
                board->grid[i][j].type = BLACK;
                board->blackPieces++;
            } else if (i >= size - 3 && (i + j) % 2 == 1){
                board->grid[i][j].type = RED;
                board->redPieces++;
            } else{
                board->grid[i][j].type = EMPTY;
            }
        }
    }
    board->gameState = GAME_IN_PROGRESS;
}

// Display the board to the console
void displayBoard(Board* board){
    printf("\n    ");
    for(int j = 0; j < board->size; j++){
        printf("%2d ",j);
    }
    printf("\n");

    for (int i = 0; i < board->size; i++){
        printf("%2d |", i);
        for(int j =0; j < board->size; j++){
            Piece p = board->grid[i][j];
            char symbol = ' ';

            // Set background color based on square position
            if ((i + j) % 2 == 0) {
                printf(ANSI_BG_LIGHT);
            } else {
                printf(ANSI_BG_DARK);
            }

            // Set piece symbol and color
            if (p.type == RED) {
                printf(ANSI_COLOR_RED);
                symbol = 'r';
            } else if (p.type == BLACK) {
                printf(ANSI_COLOR_BLACK);
                symbol = 'b';
            } else if (p.type == RED_KING) {
                printf(ANSI_COLOR_RED);
                symbol = 'R';
            } else if (p.type == BLACK_KING) {
                printf(ANSI_COLOR_BLACK);
                symbol = 'B';
            }
            printf(" %c " ANSI_COLOR_RESET, symbol);
        }
        printf("\n");
    }
    printf("\n");
}

// free memory used by the board
void freeBoard(Board* board){
    for(int i = 0; i < board->size; i++){
        free(board->grid[i]);
    }
    free(board->grid);
    free(board);
}


// Check if a move is valid (non-capturing move)
bool isValidMove(Board* board, int srcRow, int srcCol, int destRow, int destCol, int playerTurn) {
    if (srcRow < 0 || srcRow >= board->size || srcCol < 0 || srcCol >= board->size)
        return false;
    if (destRow < 0 || destRow >= board->size || destCol < 0 || destCol >= board->size)
        return false;
    
    Piece* src = &board->grid[srcRow][srcCol];
    Piece* dest = &board->grid[destRow][destCol];

    // Check that source is not empty and destination is empty
    if(src->type == EMPTY || dest->type != EMPTY)
        return false;
    
    // Check if it's the correct player's turn
    if ((playerTurn == 1 && (src->type != RED && src->type != RED_KING)) ||
        (playerTurn == 2 && (src->type != BLACK && src->type != BLACK_KING)))
        return false;

    int rowDiff = destRow - srcRow;
    int colDiff = destCol - srcCol;

    // Normal move (1 step diagonally)
    if (abs(rowDiff) == 1 && abs(colDiff) == 1) {
        // Regular pieces can't move backward
        if (src->type == RED && rowDiff != -1) return false;
        if (src->type == BLACK && rowDiff != 1) return false;
        
        // Kings can move in any direction
        return true;
    }
    
    return false;
}

// Check if a jump move is valid
bool isValidJump(Board* board, int srcRow, int srcCol, int destRow, int destCol, int playerTurn) {
    if (srcRow < 0 || srcRow >= board->size || srcCol < 0 || srcCol >= board->size)
        return false;
    if (destRow < 0 || destRow >= board->size || destCol < 0 || destCol >= board->size)
        return false;
    
    Piece* src = &board->grid[srcRow][srcCol];
    Piece* dest = &board->grid[destRow][destCol];
    
    // Check that source is not empty and destination is empty
    if(src->type == EMPTY || dest->type != EMPTY)
        return false;
    
    // Check if it's the correct player's turn
    if ((playerTurn == 1 && (src->type != RED && src->type != RED_KING)) ||
        (playerTurn == 2 && (src->type != BLACK && src->type != BLACK_KING)))
        return false;

    int rowDiff = destRow - srcRow;
    int colDiff = destCol - srcCol;

    // Capture move (2 steps over opponent)
    if (abs(rowDiff) == 2 && abs(colDiff) == 2) {
        // Regular pieces can't move backward
        if (src->type == RED && rowDiff > 0) return false;
        if (src->type == BLACK && rowDiff < 0) return false;
        
        int midRow = (srcRow + destRow) / 2;
        int midCol = (srcCol + destCol) / 2;
        Piece* mid = &board->grid[midRow][midCol];

        // Make sure there is an opponent piece to capture
        if ((playerTurn == 1 && (mid->type == BLACK || mid->type == BLACK_KING)) ||
            (playerTurn == 2 && (mid->type == RED || mid->type == RED_KING))) {
            return true;
        }
    }

    return false;
}

// Move a piece if the move is valid
bool movePiece(Board* board, int srcRow, int srcCol, int destRow, int destCol, int playerTurn) {
    int rowDiff = destRow - srcRow;
    int colDiff = destCol - srcCol;
    bool isCapture = (abs(rowDiff) == 2 && abs(colDiff) == 2);
    
    // Verify if the move is valid
    if (isCapture) {
        if (!isValidJump(board, srcRow, srcCol, destRow, destCol, playerTurn))
            return false;
    } else {
        if (!isValidMove(board, srcRow, srcCol, destRow, destCol, playerTurn))
            return false;
    }
    
    Piece* src = &board->grid[srcRow][srcCol];
    Piece* dest = &board->grid[destRow][destCol];
    
    // Store the original piece type
    PieceType pieceType = src->type;
    
    // Execute the move
    *dest = *src;
    src->type = EMPTY;
    
    // If it's a capture, remove the jumped piece
    if (isCapture) {
        int midRow = (srcRow + destRow) / 2;
        int midCol = (srcCol + destCol) / 2;
        Piece* mid = &board->grid[midRow][midCol];
        
        // Update piece count
        if (mid->type == RED || mid->type == RED_KING)
            board->redPieces--;
        else if (mid->type == BLACK || mid->type == BLACK_KING)
            board->blackPieces--;
            
        mid->type = EMPTY;
    }
    
    // King promotion
    if (destRow == 0 && pieceType == RED) {
        dest->type = RED_KING;
    }
    else if (destRow == board->size - 1 && pieceType == BLACK) {
        dest->type = BLACK_KING;
    }
    
    // Update game state after the move
    updateGameState(board);
    
    return true;
}

// Check if a piece can make multiple jumps
bool checkForMultipleJumps(Board* board, int row, int col, int playerTurn) {
    int directions[][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
    Piece piece = board->grid[row][col];
    
    // For regular pieces, restrict directions based on player
    int start = 0, end = 4;
    if (piece.type == RED) {
        // Red regular piece can only move upward (first two directions)
        start = 0; end = 2;
    } else if (piece.type == BLACK) {
        // Black regular piece can only move downward (last two directions)
        start = 2; end = 4;
    }
    
    // Check all valid directions for possible jumps
    for (int d = start; d < end; d++) {
        int jumpRow = row + 2 * directions[d][0];
        int jumpCol = col + 2 * directions[d][1];
        
        if (isValidJump(board, row, col, jumpRow, jumpCol, playerTurn)) {
            return true;
        }
    }
    
    return false;
}

// Check if a player has any valid moves
bool hasValidMoves(Board* board, int playerTurn) {
    for (int i = 0; i < board->size; i++) {
        for (int j = 0; j < board->size; j++) {
            Piece piece = board->grid[i][j];
            
            // Check if this is the current player's piece
            if ((playerTurn == 1 && (piece.type == RED || piece.type == RED_KING)) ||
                (playerTurn == 2 && (piece.type == BLACK || piece.type == BLACK_KING))) {
                    
                // Check in all four diagonal directions
                int directions[][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
                
                for (int d = 0; d < 4; d++) {
                    // Check for regular moves
                    int moveRow = i + directions[d][0];
                    int moveCol = j + directions[d][1];
                    
                    if (isValidMove(board, i, j, moveRow, moveCol, playerTurn)) {
                        return true;
                    }
                    
                    // Check for jump moves
                    int jumpRow = i + 2 * directions[d][0];
                    int jumpCol = j + 2 * directions[d][1];
                    
                    if (isValidJump(board, i, j, jumpRow, jumpCol, playerTurn)) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

// Count the number of pieces for each player
void countPieces(Board* board) {
    board->redPieces = 0;
    board->blackPieces = 0;
    
    for (int i = 0; i < board->size; i++) {
        for (int j = 0; j < board->size; j++) {
            if (board->grid[i][j].type == RED || board->grid[i][j].type == RED_KING) {
                board->redPieces++;
            } else if (board->grid[i][j].type == BLACK || board->grid[i][j].type == BLACK_KING) {
                board->blackPieces++;
            }
        }
    }
}

// Update the game state based on the board
void updateGameState(Board* board) {
    // Update piece counts
    countPieces(board);
    
    // Check win conditions
    if (board->redPieces == 0) {
        board->gameState = BLACK_WINS;
    } else if (board->blackPieces == 0) {
        board->gameState = RED_WINS;
    } else if (!hasValidMoves(board, 1) && !hasValidMoves(board, 2)) {
        board->gameState = DRAW;
    } else {
        board->gameState = GAME_IN_PROGRESS;
    }
}

// Get current game state
int getGameState(Board* board) {
    return board->gameState;
}

// Save game state to a file (including current player)
void saveGame(Board* board, const char* filename, int currentPlayer) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        printf("Error opening file for saving.\n");
        return;
    }
    
    // Save board size and current player
    fwrite(&board->size, sizeof(int), 1, file);
    fwrite(&currentPlayer, sizeof(int), 1, file);
    fwrite(&board->redPieces, sizeof(int), 1, file);
    fwrite(&board->blackPieces, sizeof(int), 1, file);
    fwrite(&board->gameState, sizeof(int), 1, file);
    
    // Save the grid
    for(int i = 0; i < board->size; i++) {
        fwrite(board->grid[i], sizeof(Piece), board->size, file);
    }
    
    fclose(file);
    printf("Game saved successfully to %s.\n", filename);
}

// Load game state from a file
Board* loadGame(const char* filename, int* currentPlayer) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("Error opening file for loading.\n");
        return NULL;
    }
    
    // Read board size and current player
    int size;
    fread(&size, sizeof(int), 1, file);
    fread(currentPlayer, sizeof(int), 1, file);
    
    // Create new board
    Board* board = createBoard(size);
    
    // Read piece counts and game state
    fread(&board->redPieces, sizeof(int), 1, file);
    fread(&board->blackPieces, sizeof(int), 1, file);
    fread(&board->gameState, sizeof(int), 1, file);
    
    // Read the grid
    for(int i = 0; i < size; i++) {
        fread(board->grid[i], sizeof(Piece), size, file);
    }
    
    fclose(file);
    printf("Game loaded successfully from %s.\n", filename);
    return board;
}

Move getComputerMove(Board* board) {
    Move possibleMoves[100];
    int numMoves = 0;
    bool mustCapture = false;

    // First, check if any captures are available 
    for (int i = 0; i < board->size; i++) {
        for (int j = 0; j < board->size; j++) {
            if (board->grid[i][j].type == BLACK || board->grid[i][j].type == BLACK_KING) {
                int directions[][2] = {{-1,-1},{-1,1},{1,-1},{1,1}};
                for (int d = 0; d < 4; d++) {
                    int jumpRow = i + 2 * directions[d][0];
                    int jumpCol = j + 2 * directions[d][1];
                    if (isValidJump(board, i, j, jumpRow, jumpCol, 2)) {
                        possibleMoves[numMoves++] = (Move){i, j, jumpRow, jumpCol, true};
                        mustCapture = true;
                    }
                }
            }
        }
    }

    // If no captures, look for regular moves
    if (!mustCapture) {
        for (int i = 0; i < board->size; i++) {
            for (int j = 0; j < board->size; j++) {
                if (board->grid[i][j].type == BLACK || board->grid[i][j].type == BLACK_KING) {
                    int directions[][2] = {{-1,-1},{-1,1},{1,-1},{1,1}};
                    for (int d = 0; d < 4; d++) {
                        int moveRow = i + directions[d][0];
                        int moveCol = j + directions[d][1];
                        if (isValidMove(board, i, j, moveRow, moveCol, 2)) {
                            possibleMoves[numMoves++] = (Move){i, j, moveRow, moveCol, false};
                        }
                    }
                }
            }
        }
    }

    // Randomly select a move
    if (numMoves > 0) {
        return possibleMoves[rand() % numMoves];
    }

    // If no moves available
    return (Move){-1, -1, -1, -1, false};
}

void makeComputerMove(Board* board) {
    Move computerMove = getComputerMove(board);
    if (computerMove.srcRow != -1) {
        movePiece(board, computerMove.srcRow, computerMove.srcCol,
                 computerMove.destRow, computerMove.destCol, 2);
        
        // Check for additional jumps 
        while (checkForMultipleJumps(board, computerMove.destRow, computerMove.destCol, 2)) {
            computerMove = getComputerMove(board); // Get next jump
            if (computerMove.srcRow == -1) break;
            movePiece(board, computerMove.srcRow, computerMove.srcCol,
                     computerMove.destRow, computerMove.destCol, 2);
        }
    }
}

// Display game status
void displayGameStatus(Board* board, int currentPlayer) {
    printf("\n====== CHECKERS GAME ======\n");
    printf("Current Player: %s\n", currentPlayer == 1 ? "RED (r/R)" : "BLACK (b/B)");
    printf("Red pieces: %d, Black pieces: %d\n", board->redPieces, board->blackPieces);
    
    switch (board->gameState) {
        case GAME_IN_PROGRESS:
            break;
        case RED_WINS:
            printf("RED WINS!\n");
            break;
        case BLACK_WINS:
            printf("BLACK WINS!\n");
            break;
        case DRAW:
            printf("GAME DRAW!\n");
            break;
    }
    
    printf("==========================\n\n");
}

// Display help/instructions
void displayHelp() {
    printf("\n======= CHECKERS GAME HELP =======\n");
    printf("RULES:\n");
    printf("1. RED moves first, then players alternate turns.\n");
    printf("2. Regular pieces can only move diagonally forward one square.\n");
    printf("3. To capture, jump diagonally over an opponent's piece to an empty square.\n");
    printf("4. Multiple jumps in a single turn are allowed if possible.\n");
    printf("5. When a piece reaches the opposite end, it becomes a king.\n");
    printf("6. Kings can move diagonally in any direction.\n");
    printf("7. The game ends when one player captures all opponent pieces or\n");
    printf("   when a player cannot make any valid moves.\n\n");
    
    printf("COMMANDS:\n");
    printf("- To move: Enter source row/column and destination row/column.\n");
    printf("- Save/Load: Enter filename when prompted.\n");
    printf("- Display Board: Shows the current board state.\n");
    printf("- Help: Shows this help message.\n");
    printf("- Exit: Quit the game.\n");
    
    printf("\nPIECE SYMBOLS:\n");
    printf("r = Red piece\n");
    printf("b = Black piece\n");
    printf("R = Red king\n");
    printf("B = Black king\n");
    printf("==================================\n\n");
}
