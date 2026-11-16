/*
 * PS3 integration template.
 *
 * This file intentionally avoids guessed PSL1GHT API calls. The exact
 * video, pad, audio, and exit-callback APIs must match the selected
 * ps3dev/PSL1GHT revision.
 */
#include "game_logic.h"

int main(void) {
    GameState game;
    game_init(&game);

    /*
     * TODO:
     * - Initialize PS3 video output.
     * - Initialize and poll DualShock 3 input.
     * - Map ○×□△ and START to GameAction.
     * - Render game.display_text and game.combo.
     * - Play game.voice_text using bundled audio assets.
     * - Register a clean XMB exit callback.
     *
     * This template is not yet a runnable PS3 application.
     */
    return (int)game.combo;
}
