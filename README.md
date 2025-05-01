## Overview
A terminal-based Checkers game implemented in C, featuring:
- Human vs Human and Human vs Computer modes
- ANSI-colored board display
- Save/Load game functionality
- Full adherence to standard Checkers rules

## Features
- **Two Game Modes**:
  - Player vs Player (local)
  - Player vs Computer (computer plays as Black)
- **Save & Load**:
  - Save game progress
  - Resume games exactly where you left off
- **Game Rules**:
  - Forced captures
  - King promotions 
  - Win/draw detection
- **Terminal UI**:
  - Colored pieces (red/black)
  - Coordinated board (numbered rows/columns)

## Game Architecture
- **Board & Pieces**:
  - The game uses an 8x8 grid where each square tracks its state:
    - Empty, Red piece, Black piece, Red King, or Black King
    - Remaining pieces on board are tracked to detect wins

- **Turns & Moves**:
  - Red (Player 1) moves first, followed by Black (Player 2 or Computer).
  - Players input moves as coordinates (source row and column, destination row and column).
  - The game enforces standard rules:
    - Regular pieces move diagonally forward.
    - Kings move diagonally in any direction.
    - Captures are possible (jump over opponent's piece).
  - Human vs Computer Mode
    - Computer automatically plays as Black with the following logic:
      - Prioritizes capturing opponent pieces.
      - Chooses random valid moves if no captures exist.
      - Promotes to Kings when reaching the opponent's last row.

## Key Features
- **Save/Load System**
  - Games can be saved to binary files and resumed later.
  - Saves board state, piece counts, and whose turn is next.
- **Terminal Interface**
  - Displays colored pieces (Red/Black) on a checkerboard.
  - Shows row/column numbers for easy move input.
- **Win Detection**
  - Checks if either player has no pieces left or valid moves.
  - Declares a draw if neither player can win.

## Technical Highlights
  - Dynamic Memory Allocation: Adjusts to any board size (default 8x8).
  - Efficient Rules Engine: Validates moves and captures.
  - ANSI Colors: Enhances readability in terminal.
  - File I/O: Saves and loads game state to allow game resumption.
  - Structured Design: Uses C structures to represent pieces and board state clearly and modularly.
