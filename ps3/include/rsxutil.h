#ifndef __RSXUTIL_H__
#define __RSXUTIL_H__
#include <ppu-types.h>
#include <rsx/rsx.h>
#define CB_SIZE 0x100000
#define HOST_SIZE (32*1024*1024)
extern gcmContextData *context;
extern u32 display_width, display_height, curr_fb;
extern u32 color_pitch, color_offset[2];
extern u32 *color_buffer[2];
void setRenderTarget(u32 index);
s32 init_screen(void *host_addr, u32 size);
void waitflip(void);
void flip(void);
#endif
