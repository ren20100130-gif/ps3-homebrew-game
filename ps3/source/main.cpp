/*
 * PS3 runtime entry point based on the public PSL1GHT blitting sample.
 * PSL1GHT pad data reports changed input; each reported press is handled once.
 * The current renderer is intentionally simple: colored screen feedback.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>
#include <ppu-types.h>
#include <sys/process.h>
#include <io/pad.h>
#include <rsx/rsx.h>
#include <sysutil/sysutil.h>
#include "rsxutil.h"
#include "game_logic.h"

SYS_PROCESS_PARAM(1001, 0x100000);

static volatile int running = 1;
static GameState game;
extern "C" {
static void program_exit_callback(void) {
    sysUtilUnregisterCallback(SYSUTIL_EVENT_SLOT0);
    gcmSetWaitFlip(context);
    rsxFinish(context, 1);
}
static void sysutil_exit_callback(u64 status, u64 param, void *usrdata) {
    (void)param; (void)usrdata;
    if (status == SYSUTIL_EXIT_GAME) running = 0;
}
}

static GameAction read_button(const padData *data) {
    if (data->BTN_START) return ACTION_START;
    if (data->BTN_CIRCLE) return ACTION_CIRCLE;
    if (data->BTN_CROSS) return ACTION_CROSS;
    if (data->BTN_SQUARE) return ACTION_SQUARE;
    if (data->BTN_TRIANGLE) return ACTION_TRIANGLE;
    return ACTION_NONE;
}

int main(int argc, const char *argv[]) {
    (void)argc; (void)argv;
    padInfo padinfo;
    padData paddata;
    void *host_addr = memalign(1024 * 1024, HOST_SIZE);
    if (!host_addr) return 1;

    game_init(&game);
    init_screen(host_addr, HOST_SIZE);
    ioPadInit(7);
    atexit(program_exit_callback);
    sysUtilRegisterCallback(0, sysutil_exit_callback, NULL);

    setRenderTarget(curr_fb);
    while (running) {
        GameAction action = ACTION_NONE;
        sysUtilCheckCallback();
        ioPadGetInfo(&padinfo);
        for (int i = 0; i < MAX_PADS; ++i) {
            if (!padinfo.status[i]) continue;
            if (ioPadGetData(i, &paddata) == 0)
                action = read_button(&paddata);
            break;
        }
        game_update(&game, action);

        /* Temporary visual feedback until the text renderer is added. */
        u32 clear = 0x101820;
        switch (game.last_action) {
            case ACTION_CIRCLE: clear = 0x402040; break;
            case ACTION_CROSS: clear = 0x204050; break;
            case ACTION_SQUARE: clear = 0x305020; break;
            case ACTION_TRIANGLE: clear = 0x503020; break;
            default: break;
        }
        rsxSetClearColor(context, clear);
        rsxSetClearDepthStencil(context, 0xffff);
        rsxClearSurface(context, GCM_CLEAR_R | GCM_CLEAR_G |
                         GCM_CLEAR_B | GCM_CLEAR_A |
                         GCM_CLEAR_S | GCM_CLEAR_Z);
        flip();
    }
    free(host_addr);
    return 0;
}
