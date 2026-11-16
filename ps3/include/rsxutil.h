#ifndef RSXUTIL_H
#define RSXUTIL_H
#include <ppu-types.h>
#include <rsx/rsx.h>
#define HOST_SIZE (32*1024*1024)
#define CB_SIZE ( RSX_CONTEXT_SIZE )
extern gcmContextData *context;
extern u32 curr_fb, display_width, display_height, color_pitch, color_offset[2];
void init_screen(void *host_addr, u32 size);
void setRenderTarget(u32 index);
void flip(void);
#endif
