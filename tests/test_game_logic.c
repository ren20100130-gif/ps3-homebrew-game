#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "game_logic.h"

static void check_action(GameAction action, const char *expected) {
    GameState state;
    game_init(&state);
    game_update(&state, action);
    assert(state.last_action == action);
    assert(strcmp(state.display_text, expected) == 0);
    assert(state.combo == 1);
}

int main(void) {
    GameState state;

    check_action(ACTION_CIRCLE, "チンチン");
    check_action(ACTION_CROSS, "ニガニガ");
    check_action(ACTION_SQUARE, "poop");
    check_action(ACTION_TRIANGLE, "ゲイ");

    game_init(&state);
    game_update(&state, ACTION_CIRCLE);
    game_update(&state, ACTION_SQUARE);
    assert(state.combo == 2);
    game_update(&state, ACTION_START);
    assert(state.combo == 0);
    assert(state.last_action == ACTION_NONE);
    assert(strcmp(state.display_text, "READY?") == 0);

    puts("All game logic tests passed.");
    return 0;
}
