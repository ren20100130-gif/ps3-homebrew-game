#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

typedef enum {
    ACTION_NONE = 0,
    ACTION_CIRCLE,
    ACTION_CROSS,
    ACTION_SQUARE,
    ACTION_TRIANGLE,
    ACTION_START
} GameAction;

typedef struct {
    GameAction last_action;
    unsigned int combo;
    const char *display_text;
    const char *voice_text;
} GameState;

void game_init(GameState *state);
void game_update(GameState *state, GameAction action);

#endif
