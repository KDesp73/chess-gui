#include "gui.h"
#include "colors.h"
#include "globals.h"
#include "raylib.h"
#include "utils.h"
#include <chess/board.h>
#include <chess/move.h>
#include <chess/notation.h>
#include <chess/square.h>
#include <chess/ui.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void gui_play(ui_config_t config, const char* fen)
{
    Texture2D textures[12];
    load_textures(textures, "wiki");

    Vector2 display_size = { GetScreenWidth(), GetScreenHeight() };

    game_t game;
    game_init(&game, NULL, TITLE, "Player 1", "Player 2", fen);
    board_t board;
    board_init_fen(&board, fen);

    Vector2 selected_square = VEMPTY;
    Vector2 move_square = VEMPTY;

    size_t count;
    square_t** valid = NULL;

    while (!WindowShouldClose()) {
        display_size = (Vector2){ GetScreenWidth(), GetScreenHeight() };
        Rectangle display = (Rectangle){.x = 0, .y = 0, VARGS(display_size)};
        Vector2 board_position = place_center(display, SQUARE_SIZE * BOARD_SIZE, SQUARE_SIZE * BOARD_SIZE);

        handle_exports(&board, &game);

        // Handle mouse click
        if(board.result == 0){
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (VCMP(selected_square, VEMPTY)) {
                    // Select the square
                    selected_square = square_clicked(board_position);
                    if (!VCMP(selected_square, VEMPTY)) {
                        // Highlight valid moves for the selected piece
                        square_t square;
                        square_from_coords(&square, 7 - selected_square.y, selected_square.x);
                        valid = valid_moves(&board, square, &count);
                    }
                } else {
                    // Move the piece
                    move_square = square_clicked(board_position);
                    if (!VCMP(move_square, VEMPTY)) {
                        square_t from, to;
                        square_from_coords(&from, 7-selected_square.y, selected_square.x);
                        square_from_coords(&to, 7-move_square.y, move_square.x);
                        char promotion = '\0';

                        san_move_t san;
                        move_to_san(&board, from, to, promotion, &san);
                        if(move(&board, from, to, promotion)) {
                            game_add_move(&game, san);
                        }
                        selected_square = VEMPTY;
                        move_square = VEMPTY;
                        if (valid != NULL && count != 0) {
                            squares_free(&valid, count); // Free valid moves
                        }
                    }
                }
            }
        } 

        // Drawing
        BeginDrawing();
        ClearBackground(GetColor(0x282828FF));

        draw_board(board_position, SQUARE_SIZE);

        // Highlight valid moves
        if (!VCMP(selected_square, VEMPTY)) {
            for (size_t i = 0; i < count; i++) {
                highlight_square(board_position, V(valid[i]->x, 7 - valid[i]->y), COLOR_HIGHLIGHT);
            }
        }

        // Draw the pieces
        draw_pieces(board.grid, textures, board_position);

        check_result(&board, &game, display);

        EndDrawing();
    }

    CloseWindow();
}

