#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 
#include "checkers.h"

// clear the console screen
void clearScreen(){
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

int main() {
    srand(time(NULL)); 
    int size = 8;
    Board* board = createBoard(size);
    initializeBoard(board);

    int choice;
    char filename[100];
    int currentPlayer = 1; // 1 = Red(Human), 2 = Black (Human or Computer)
    int gameMode = 0; // 1 = Human vs Human, 2 = Human vs Computer

    // Game mode selection
    printf("Choose game mode:\n1. Human vs Human\n2. Human vs Computer\n");
    scanf("%d", &gameMode);
    if (gameMode != 1 && gameMode != 2) gameMode = 1; // Default to HvH

    while (1) {
        clearScreen();

        // Computer's turn (if in Human vs Computer mode)
        if (gameMode == 2 && currentPlayer == 2 && getGameState(board) == GAME_IN_PROGRESS) {
            printf("Computer's turn (BLACK)...\n");
            makeComputerMove(board);
            currentPlayer = 1; // Switch back to human
            continue; // Skip to next iteration
        }

        // check if game is over
        if(getGameState(board) != GAME_IN_PROGRESS){
            displayBoard(board);
            displayGameStatus(board, currentPlayer);

            printf("\nGame over! Play again? (y/n: )");
            char playAgain;
            scanf(" %c", &playAgain);

            if(playAgain == 'y' || playAgain == 'Y'){
                freeBoard(board);
                board = createBoard(size);
                initializeBoard(board);
                currentPlayer = 1;
                continue;
            }
            else {
                break;
            }
        }

        displayGameStatus(board, currentPlayer);
        displayBoard(board);


        printf("\n1. Move Piece\n");
        printf("2. Save Game\n");
        printf("3. Load Game\n");
        printf("4. Help\n");
        printf("5. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);

        if (choice == 1) {
            int srcRow, srcCol, destRow, destCol;
            printf("Enter source row and column: ");
            scanf("%d%d", &srcRow, &srcCol);
            printf("Enter destination row and column: ");
            scanf("%d%d", &destRow, &destCol);

            // validate source position
            if (srcRow < 0 || srcRow >= board->size || srcCol < 0 || srcCol >= board->size){
                printf("Invalid position! Press Enter to continue....");
                getchar(); getchar();
                continue;
            }

            // Validate destination position
            if (destRow < 0 || destRow >= board->size || destCol < 0 || destCol >= board->size) {
                printf("Invalid position! Press Enter to continue...");
                getchar(); getchar();
                continue;
            }
            
            if (movePiece(board, srcRow, srcCol, destRow, destCol, currentPlayer)) {
                printf("Move successful!\n");
                // Check for multiple jumps
                bool canJumpAgain = false;
                if (abs(destRow - srcRow) == 2) { // potential capture move
                    canJumpAgain = checkForMultipleJumps(board, destRow, destCol, currentPlayer);
                    
                    if (canJumpAgain) {
                        printf("Additional jump is available from position (%d,%d)!\n", destRow, destCol);
                        printf("Continue with another jump? (y/n): ");
                        char jumpAgain;
                        scanf(" %c", &jumpAgain);
                        
                        if (jumpAgain == 'y' || jumpAgain == 'Y') {
                            printf("Enter your next jump destination row and column: ");
                            int nextRow, nextCol;
                            scanf("%d%d", &nextRow, &nextCol);
                            
                            if (isValidJump(board, destRow, destCol, nextRow, nextCol, currentPlayer)) {
                                movePiece(board, destRow, destCol, nextRow, nextCol, currentPlayer);
                                printf("Jump successful!\n");                              
                            } else {
                                printf("Invalid jump!\n");
                            }
                        }
                    }
                }
                
                // After a successful move with no further jumps, change player
                if (!canJumpAgain) {
                    currentPlayer = (currentPlayer == 1) ? 2 : 1;
                }
                
                printf("Press Enter to continue...");
                getchar(); getchar();
            } else {
                printf("Invalid move! Press Enter to try again...");
                getchar(); getchar();
            }
        }
        else if (choice == 2) {
            printf("Enter filename to save: ");
            scanf("%s", filename);
            if (strlen(filename) > 0) {
                saveGame(board, filename, currentPlayer);
            } else {
                printf("Invalid filename!\n");
            }
            printf("Press Enter to continue...");
            getchar(); getchar();
        } else if (choice == 3) {
            printf("Enter filename to load: ");
            scanf("%s", filename);
            if (strlen(filename) > 0) {
                Board* loaded = loadGame(filename, &currentPlayer);
                if (loaded) {
                    freeBoard(board);
                    board = loaded;
                }
            } else {
                printf("Invalid filename!\n");
            }
            printf("Press Enter to continue...");
            getchar(); getchar();
        } else if (choice == 4) {
            displayHelp();
            printf("Press Enter to continue...");
            getchar(); getchar();
        } else if (choice == 5) {
            printf("Are you sure you want to exit? (y/n): ");
            char confirm;
            scanf(" %c", &confirm);
            if (confirm == 'y' || confirm == 'Y') {
                break;
            }
        } else {
            printf("Invalid choice. Press Enter to continue...");
            getchar(); getchar();
        }
    }

    freeBoard(board);
    return 0;
}