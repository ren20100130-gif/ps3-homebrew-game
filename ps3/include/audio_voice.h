#ifndef AUDIO_VOICE_H
#define AUDIO_VOICE_H

#include <ppu-types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Samples are interleaved stereo float PCM, 48 kHz, with values in [-1, 1]. */
int audio_voice_init(void);
void audio_voice_play(const float *stereo_samples, u32 frame_count);
void audio_voice_update(void);
void audio_voice_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif
