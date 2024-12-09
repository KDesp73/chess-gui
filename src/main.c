#include "globals.h"
#include "gui.h"
#include "utils.h"
#include "version.h"
#include <chess/notation.h>
#include <chess/ui.h>
#include <chess/zobrist.h>
#include <stdio.h>

#define CLIB_IMPLEMENTATION
#include "extern/clib.h"

int main(int argc, char** argv){
    init_zobrist();

    CliArguments args = clib_cli_make_arguments(5,
        clib_cli_create_argument('h', "help", "Prints this message", no_argument),
        clib_cli_create_argument('v', "version", "Prints library version", no_argument),
        clib_cli_create_argument('F', "fen", "Specify the starting fen", required_argument),
        clib_cli_create_argument('p', "perspective", "Print board from the perspective of {WHITE|BLACK}", required_argument),
        clib_cli_create_argument('P', "pgn", "Specify pgn file to run", required_argument)
    );

    struct option* opts = clib_cli_get_options(args);
    char* fmt = clib_cli_generate_format_string(args);
    char* fen = NULL;
    char* pgn_file = NULL;

    ui_config_t config = FULL_CONFIG;

    int opt;
    while ((opt = getopt_long(argc, argv, fmt, opts, NULL)) != -1) {
        switch (opt) {
        case 'h':
            clib_cli_help(args, "./chess [OPTIONS...]", "Written by KDesp73");
            goto cleanup;
        case 'v':
            printf("chess-gui v%s\n", CHESS_GUI_VERSION);
            goto cleanup;
        case 'F':
            fen = optarg;
            break;
        case 'p':
            if(STREQ("BLACK", optarg)) config.perspective = 0;
            else config.perspective = 1;
            break;
        case 'P':
            pgn_file = optarg;
            if(!clib_file_exists(pgn_file)){
                ERRO("File not found");
                goto cleanup;
            }
            break;
        default:
            ERRO("Invalid option: '%c'", opt);
            goto cleanup;
        }
    }

    SetTraceLogLevel(LOG_WARNING);

    // Initialize Window
    InitWindow(SQUARE_SIZE * 8, SQUARE_SIZE * 8, TITLE);
    SET_FULLSCREEN(0);
    SetTargetFPS(60);

    if(pgn_file != NULL){
        char* pgn = clib_file_read(pgn_file, "r");
        gui_run(config, pgn);
        free(pgn);
    } else {
        gui_play(config, fen);
    }

cleanup:
    free(fmt);
    free(opts);
    clib_cli_clean_arguments(&args);

    return 0;
}
