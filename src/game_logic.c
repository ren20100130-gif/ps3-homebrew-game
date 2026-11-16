#include "game_logic.h"

void game_init(GameState *state) {
    if (!state) return;
    state->last_action = ACTION_NONE;
    state->combo = 0;
    state->display_text = "READY?";
    state->voice_text = "スタート！";
}

void game_update(GameState *state, GameAction action) {
    if (!state) return;

    if (action == ACTION_START) {
        game_init(state);
        return;
    }

    state->last_action = action;
    switch (action) {
        case ACTION_CIRCLE:
            state->display_text = "チンチン";
            state->voice_text = "チンチン！";
            break;
        case ACTION_CROSS:
            state->display_text = "ニガニガ";
            state->voice_text = "ニガニガ！";
            break;
        case ACTION_SQUARE:
            state->display_text = "poop";
            state->voice_text = "poop！";
            break;
        case ACTION_TRIANGLE:
            state->display_text = "ゲイ";
            state->voice_text = "ゲイ！";
            break;
        default:
            return;
    }
    state->combo++;
}
