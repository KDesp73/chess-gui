#include "gui.h"
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

void gui_run(ui_config_t config, const char* pgn)
{    
    Texture2D textures[12];
    load_textures(textures, "staunty");

    Vector2 display_size = { GetScreenWidth(), GetScreenHeight() };

    game_t game;
    pgn_import(&game, pgn);
    board_t board;
    board_init_fen(&board, NULL);

    size_t move_index = 0;

    while (!WindowShouldClose()) {
        display_size = (Vector2){ GetScreenWidth(), GetScreenHeight() };
        Rectangle display = (Rectangle){.x = 0, .y = 0, VARGS(display_size)};
        Vector2 board_position = place_center(display, SQUARE_SIZE * BOARD_SIZE, SQUARE_SIZE * BOARD_SIZE);

        handle_exports(&board, &game);
        if(IsKeyPressed(KEY_ENTER)){
            san_move_t san = game.moves[move_index];
            if(game.move_count > move_index) {
                move_index++;
            
                square_t from, to;
                char promotion;
                san_to_move(&board, san, &from, &to, &promotion);
                move(&board, from, to, promotion);
            }
        }


        // Drawing
        BeginDrawing();
        ClearBackground(GetColor(0x282828FF));

        draw_board(board_position, SQUARE_SIZE);
        draw_pieces(board.grid, textures, board_position);

        check_result(&board, &game, display);

        EndDrawing();
    }

    CloseWindow();
   
}
