#include "gui.h"
#include "colors.h"
#include "globals.h"
#include "raylib.h"
#include <chess/board.h>
#include <chess/move.h>
#include <chess/notation.h>
#include <chess/square.h>
#include <chess/ui.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "raymath.h"
#include "utils.h"

char promotion_gui(Vector2 board_position, Texture2D textures[12], int turn)
{
    while (true) { // Loop until a valid piece is selected
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Calculate the position for the promotion GUI
        Vector2 position = place_center(
            (Rectangle) {board_position.x, board_position.y, BOARD_SIZE * SQUARE_SIZE, BOARD_SIZE * SQUARE_SIZE},
            SQUARE_SIZE * 4, 
            SQUARE_SIZE
        );

        // Draw the promotion options
        for (size_t i = 0; i < 4; i++) {
            Vector2 option_position = Vector2Add(position, (Vector2) {SQUARE_SIZE * i, 0});
            DrawRectangleV(option_position, (Vector2) {SQUARE_SIZE, SQUARE_SIZE}, RAYWHITE);
            DrawRectangleLinesEx((Rectangle){option_position.x, option_position.y, SQUARE_SIZE, SQUARE_SIZE}, 2.0, BLACK);

            char piece;
            switch (i) {
                case 0: piece = 'Q'; break; // Queen
                case 1: piece = 'R'; break; // Rook
                case 2: piece = 'B'; break; // Bishop
                case 3: piece = 'N'; break; // Knight
            }
            if (turn == 0) piece = tolower(piece);

            DrawTextureV(textures[piece_to_index(piece)], option_position, WHITE);
        }

        // Check if the left mouse button is pressed
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 click = (Vector2) {GetMouseX(), GetMouseY()};
            Vector2 local_click = (Vector2) {click.x - position.x, click.y - position.y};

            int clicked_x = (int)(local_click.x / SQUARE_SIZE);
            int clicked_y = (int)(local_click.y / SQUARE_SIZE);

            if (clicked_y == 0 && clicked_x >= 0 && clicked_x < 4) {
                EndDrawing();
                switch (clicked_x) {
                    case 0: return 'Q'; // Queen
                    case 1: return 'R'; // Rook
                    case 2: return 'B'; // Bishop
                    case 3: return 'N'; // Knight
                }
            }
        }

        EndDrawing();
    }
}

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

    square_t from, to;
    char promotion = '\0';

    while (!WindowShouldClose()) {
        display_size = (Vector2){ GetScreenWidth(), GetScreenHeight() };
        Rectangle display = (Rectangle){.x = 0, .y = 0, VARGS(display_size)};
        Vector2 board_position = place_center(display, SQUARE_SIZE * BOARD_SIZE, SQUARE_SIZE * BOARD_SIZE);

        handle_exports(&board, &game);

        BeginDrawing();
        ClearBackground(GetColor(0x282828FF));

        draw_board(board_position, SQUARE_SIZE);

        // Highlight valid moves
        if (!VCMP(selected_square, VEMPTY)) {
            for (size_t i = 0; i < count; i++) {
                highlight_square(board_position, V(valid[i]->x, 7 - valid[i]->y), ((valid[i]->x + valid[i]->y) % 2) ? COLOR_HIGHLIGHT_WHITE : COLOR_HIGHLIGHT_BLACK);
            }
        }

        draw_pieces(board.grid, textures, board_position);

        if (board.result == 0) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (VCMP(selected_square, VEMPTY)) {
                    selected_square = square_clicked(board_position);
                    if (!VCMP(selected_square, VEMPTY)) {
                        square_t square;
                        square_from_coords(&square, 7 - selected_square.y, selected_square.x);
                        valid = valid_moves(&board, square, &count);
                    }
                } else {
                    move_square = square_clicked(board_position);
                    if (!VCMP(move_square, VEMPTY)) {
                        square_from_coords(&from, 7 - selected_square.y, selected_square.x);
                        square_from_coords(&to, 7 - move_square.y, move_square.x);

                        if (pawn_is_promoting(&board, from, to)) {
                            promotion = promotion_gui(board_position, textures, board.turn);
                        } else promotion = '\0';

                        san_move_t san;
                        move_to_san(&board, from, to, promotion, &san);
                        if (move(&board, from, to, promotion)) {
                            game_add_move(&game, san);
                            promotion = '\0';
                        } else {
                            selected_square = VEMPTY;
                            squares_free(&valid, count);
                            continue;
                        }

                        selected_square = VEMPTY;
                        move_square = VEMPTY;
                        squares_free(&valid, count);
                    }
                }
            }
        }

        check_result(&board, &game, display);
        EndDrawing();
    }

    CloseWindow();
}
