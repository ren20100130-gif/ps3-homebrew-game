#include <string.h>
#include <audio/audio.h>
#include <sys/event_queue.h>
#include "audio_voice.h"

static u32 g_port = 0;
static audioPortConfig g_config;
static sys_event_queue_t g_queue;
static sys_ipc_key_t g_queue_key;
static int g_audio_ready = 0;
static int g_port_open = 0;
static int g_queue_open = 0;
static int g_port_started = 0;

static const float *g_clip = 0;
static u32 g_clip_frames = 0;
static u32 g_clip_position = 0;

static float *audio_block(u32 index) {
    float *base = (float *)(u64)g_config.audioDataStart;
    return base + ((u64)index * g_config.channelCount * AUDIO_BLOCK_SAMPLES);
}

static void fill_block(u32 index) {
    float *out = audio_block(index);
    for (u32 i = 0; i < AUDIO_BLOCK_SAMPLES; ++i) {
        float left = 0.0f;
        float right = 0.0f;
        if (g_clip && g_clip_position < g_clip_frames) {
            left = g_clip[g_clip_position * 2];
            right = g_clip[g_clip_position * 2 + 1];
            ++g_clip_position;
        } else {
            g_clip = 0;
        }
        out[i * 2] = left;
        out[i * 2 + 1] = right;
    }
}

extern "C" int audio_voice_init(void) {
    if (g_audio_ready) return 0;
    memset(&g_config, 0, sizeof(g_config));

    if (audioInit() != 0) return -1;
    g_audio_ready = 1;

    audioPortParam params;
    params.numChannels = AUDIO_PORT_2CH;
    params.numBlocks = AUDIO_BLOCK_8;
    params.attrib = AUDIO_PORT_INITLEVEL;
    params.level = 1.0f;

    if (audioPortOpen(&params, &g_port) != 0) goto fail;
    g_port_open = 1;
    if (audioGetPortConfig(g_port, &g_config) != 0) goto fail;

    if (audioCreateNotifyEventQueue(&g_queue, &g_queue_key) != 0) goto fail;
    g_queue_open = 1;
    if (audioSetNotifyEventQueue(g_queue_key) != 0) goto fail;
    sysEventQueueDrain(g_queue);

    g_clip = 0;
    g_clip_frames = 0;
    g_clip_position = 0;
    for (u32 i = 0; i < g_config.numBlocks; ++i) fill_block(i);

    if (audioPortStart(g_port) != 0) goto fail;
    g_port_started = 1;
    return 0;

fail:
    audio_voice_shutdown();
    return -1;
}

extern "C" void audio_voice_play(const float *stereo_samples, u32 frame_count) {
    if (!g_port_started || !stereo_samples || frame_count == 0) return;
    g_clip = stereo_samples;
    g_clip_frames = frame_count;
    g_clip_position = 0;
}

extern "C" void audio_voice_update(void) {
    if (!g_port_started) return;

    /*
     * One event is generated per consumed block. Drain every pending event so
     * a slow video frame cannot leave the audio queue permanently behind.
     */
    sys_event_t event;
    while (sysEventQueueReceive(g_queue, &event, 0) == 0) {
        if (audioGetPortConfig(g_port, &g_config) != 0) break;
        if (g_config.numBlocks == 0) break;

        const u32 write_index =
            (g_config.readIndex + 1) % (u32)g_config.numBlocks;
        fill_block(write_index);
    }
}

extern "C" void audio_voice_shutdown(void) {
    if (g_port_started) {
        audioPortStop(g_port);
        g_port_started = 0;
    }
    if (g_queue_open) {
        audioRemoveNotifyEventQueue(g_queue_key);
        sysEventQueueDestroy(g_queue, 0);
        g_queue_open = 0;
    }
    if (g_port_open) {
        audioPortClose(g_port);
        g_port_open = 0;
    }
    if (g_audio_ready) {
        audioQuit();
        g_audio_ready = 0;
    }
    g_clip = 0;
    g_clip_frames = 0;
    g_clip_position = 0;
}
