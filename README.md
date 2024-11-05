# Console-Tetris
## Video demo: https://www.youtube.com/watch?v=0usP70Ozuf8
## Description: 
This is a simple implementation of the classic Tetris game built using C++. The objective of the game is to move and rotate falling blocks (tetrominoes) to create complete lines across the playing field, which are then cleared. The game ends when the blocks stack up and reach the top of the screen.

## Features
- Classic Tetris gameplay: Complete rows by positioning falling tetrominoes.
- Keyboard controls: Move left, right, rotate, and drop the blocks.
- Score tracking: Keep track of your score as you clear lines.
- Level progression: The game speed increases as you progress through levels.
- Game over detection: Automatically detects and handles the end of the game.
- Terminal-based graphics: Simple ASCII/character-based display.

Getting Started
Prerequisites
Before you can compile and run the Tetris game, you will need the following installed on your system:

- A C++ compiler (e.g., g++, clang++)
- A terminal or console for running the game

## Controls
The game uses the keyboard for controlling the tetrominoes. Here are the default key bindings:

- Left Arrow: Move the tetromino left.
- Right Arrow: Move the tetromino right.
- Up Arrow: Rotate the tetromino clockwise.
- Down Arrow: Soft drop (move the tetromino down faster).
- Spacebar: Hard drop (instantly place the tetromino at the bottom).
- Escape: Pause/Resume the game.

These controls offer responsive handling of the tetrominoes, allowing for precise placement and rotation.

## Game Rules
The game follows the traditional rules of Tetris:

- Tetromino Shapes: The game features seven types of tetrominoes, each shaped differently (I, O, T, S, Z, J, L).
- Movement: Tetrominoes fall from the top of the grid, and the player must move and rotate them to fit into available spaces.
- Clearing Lines: When a row is completely filled with blocks, it is cleared, and points are awarded.
- Scoring: The player earns points based on how many lines are cleared simultaneously. Clearing more lines in a single move results in a higher score.
- Game Over: The game ends when the tetrominoes stack up to the top of the playing field, and no more tetrominoes can be placed.

## File Structure
The project is organized as follows:

- main.cpp: The main source file where everything like the game loop, rendering, and input handling are implemented.
- README.md: This file, explaining the project structure.

## Implementation
By using the Windows API I was able to achieve these effects:
- Simple ASCII color graphics
- Control over windows buffer
- Low level code management

# This was CS50!

