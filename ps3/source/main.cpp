/*
 * PS3 runtime entry point based on the public PSL1GHT blitting sample.
 * Text is drawn into the framebuffer with a tiny built-in 5x7 ASCII font.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <ppu-types.h>
#include <sys/process.h>
#include <io/pad.h>
#include <rsx/rsx.h>
#include <sysutil/sysutil.h>
#include "rsxutil.h"
#include "game_logic.h"
#include "audio_voice.h"

SYS_PROCESS_PARAM(1001, 0x100000);

static volatile int running = 1;
static GameState game;
static void *g_host_addr = NULL;

extern "C" {
static void program_exit_callback(void) {
    audio_voice_shutdown();
    sysUtilUnregisterCallback(SYSUTIL_EVENT_SLOT0);
    if (context) {
        gcmSetWaitFlip(context);
        rsxFinish(context, 1);
    }
    if (g_host_addr) {
        free(g_host_addr);
        g_host_addr = NULL;
    }
}
static void sysutil_exit_callback(u64 status, u64 param, void *usrdata) {
    (void)param; (void)usrdata;
    if (status == SYSUTIL_EXIT_GAME) running = 0;
}
}

/* 5x7 glyphs for the ASCII characters used by the UI. */
static const u8 font[26][7] = {
 {14,17,17,31,17,17,17},{30,17,17,30,17,17,30},
 {14,17,16,16,16,17,14},{30,17,17,17,17,17,30},
 {31,16,16,30,16,16,31},{31,16,16,30,16,16,16},
 {14,17,16,23,17,17,15},{17,17,17,31,17,17,17},
 {14,4,4,4,4,4,14},{7,2,2,2,18,18,12},
 {17,18,20,24,20,18,17},{16,16,16,16,16,16,31},
 {17,27,21,21,17,17,17},{17,25,21,19,17,17,17},
 {14,17,17,17,17,17,14},{30,17,17,30,16,16,16},
 {14,17,17,17,21,18,13},{30,17,17,30,20,18,17},
 {15,16,16,14,1,1,30},{31,4,4,4,4,4,4},
 {17,17,17,17,17,17,14},{17,17,17,17,17,10,4},
 {17,17,17,21,21,21,10},{17,17,10,4,10,17,17},
 {17,17,10,4,4,4,4},{31,1,2,4,8,16,31}
};

static const u8 digits[10][7] = {
 {14,17,19,21,25,17,14},{4,12,4,4,4,4,14},
 {14,17,1,2,4,8,31},{30,1,1,14,1,1,30},
 {2,6,10,18,31,2,2},{31,16,16,30,1,1,30},
 {14,16,16,30,17,17,14},{31,1,2,4,8,8,8},
 {14,17,17,14,17,17,14},{14,17,17,15,1,1,14}
};

static const u8 *glyph(char c) {
    if (c >= 'A' && c <= 'Z') return font[c-'A'];
    if (c >= '0' && c <= '9') return digits[c-'0'];
    return NULL;
}

static void draw_text(const char *text, int x, int y, int scale, u32 color) {
    const int start_x = x;
    u32 *fb = color_buffer[curr_fb];
    for (; *text; ++text) {
        if (*text == '\n') { x = start_x; y += 9 * scale; continue; }
        const u8 *g = glyph(*text);
        if (g) {
            for (int row=0; row<7; ++row) {
                for (int col=0; col<5; ++col) {
                    if (!(g[row] & (1u << (4-col)))) continue;
                    for (int sy=0; sy<scale; ++sy) {
                        for (int sx=0; sx<scale; ++sx) {
                            const int px=x+col*scale+sx, py=y+row*scale+sy;
                            if (px>=0 && py>=0 && px<(int)display_width && py<(int)display_height)
                                fb[py*(color_pitch/4)+px]=color;
                        }
                    }
                }
            }
        }
        x += 6 * scale;
    }
}

static const char *action_label(GameAction action) {
    switch (action) {
        case ACTION_CIRCLE: return "CHINCHIN";
        case ACTION_CROSS: return "NIGANIGA";
        case ACTION_SQUARE: return "POOP";
        case ACTION_TRIANGLE: return "GEI";
        default: return "READY";
    }
}

int main(int argc, const char *argv[]) {
    (void)argc; (void)argv;
    padInfo padinfo = {};
    padData paddata = {};
    g_host_addr = memalign(1024 * 1024, HOST_SIZE);
    if (!g_host_addr) return 1;

    game_init(&game);
    atexit(program_exit_callback);
    /* Stop cleanly if RSX/video setup fails; never draw into uninitialized buffers. */
    if (init_screen(g_host_addr, HOST_SIZE) != 0) return 1;
    /* Keep rendering even if pad initialization fails; input is simply disabled. */
    const s32 pad_init_result = ioPadInit(7);
    /* Audio is optional: keep the game running if audio initialization fails. */
    audio_voice_init();
    sysUtilRegisterCallback(0, sysutil_exit_callback, NULL);

    setRenderTarget(curr_fb);
    while (running) {
        GameAction action = ACTION_NONE;
        sysUtilCheckCallback();
        audio_voice_update();
        if (pad_init_result == 0 && ioPadGetInfo(&padinfo) == 0) {
            /* ioPadInit(7) supports seven ports; padInfo itself has a larger array. */
            const u32 pad_count = padinfo.max < MAX_PORT_NUM ? padinfo.max : MAX_PORT_NUM;
            for (u32 i = 0; i < pad_count; ++i) {
                if (!padinfo.status[i]) continue;
                memset(&paddata, 0, sizeof(paddata));
                if (ioPadGetData(i, &paddata) != 0) continue;
                /* PSL1GHT reports only changed input; len==0 means no new press. */
                if (paddata.len == 0) continue;
                if (paddata.BTN_START) action = ACTION_START;
                else if (paddata.BTN_CIRCLE) action = ACTION_CIRCLE;
                else if (paddata.BTN_CROSS) action = ACTION_CROSS;
                else if (paddata.BTN_SQUARE) action = ACTION_SQUARE;
                else if (paddata.BTN_TRIANGLE) action = ACTION_TRIANGLE;
                break;
            }
        }
        game_update(&game, action);

        u32 clear = 0x101820;
        switch (game.last_action) {
            case ACTION_CIRCLE: clear = 0x402040; break;
            case ACTION_CROSS: clear = 0x204050; break;
            case ACTION_SQUARE: clear = 0x305020; break;
            case ACTION_TRIANGLE: clear = 0x503020; break;
            default: break;
        }
        /* Software-clear the back buffer before drawing the UI. */
        u32 *fb = color_buffer[curr_fb];
        const u32 pitch_pixels = color_pitch / 4;
        for (u32 y=0; y<display_height; ++y)
            for (u32 x=0; x<display_width; ++x)
                fb[y*pitch_pixels+x] = 0xff000000 | clear;

        draw_text("POOP GAME", 40, 40, 5, 0xffffffff);
        draw_text("PRESS A BUTTON", 40, 100, 2, 0xffffffff);
        draw_text(action_label(game.last_action), 40, 150, 4, 0xffff80ff);
        draw_text("O CHINCHIN  X NIGANIGA", 40, 220, 2, 0xffffffff);
        draw_text("SQUARE POOP  TRIANGLE GEI", 40, 250, 2, 0xffffffff);
        draw_text("START RESET", 40, 280, 2, 0xffffffff);

        flip();
    }
    return 0;
}
