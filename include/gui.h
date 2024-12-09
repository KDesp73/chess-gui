#ifndef GUI_H
#define GUI_H

#include "raylib.h"
#include <chess/board.h>
#include <chess/notation.h>
#include <chess/ui.h>

void gui_play(ui_config_t config, const char* fen);
void gui_run(ui_config_t config, const char* pgn);

Vector2 text_size(const char* text, int fontSize);
void handle_exports(board_t* board, game_t* game);
void highlight_square(Vector2 board_position, Vector2 square, Color color);
Vector2 square_clicked(Vector2 board_position);
void draw_pieces(char board[8][8], Texture2D textures[12], Vector2 board_position);
int piece_to_index(char piece);
void load_textures(Texture2D textures[12], const char* theme);
void draw_board(Vector2 board_position, float square_size);
Vector2 place_center(Rectangle outer, float innerWidth, float innerHeight);
void check_result(board_t* board, game_t* game, Rectangle display);

#endif // GUI_H