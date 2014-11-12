#ifndef FRAME_H
#define FRAME_H

#include <core/common.h>
#include <asm/paging.h>

void init_frames(u32 *frames_bitmap, u32 frames_size, u32 kernel_end);

void frame_alloc(page_t *page);

void frame_alloc_at(u32 phys_addr, page_t *page);

void frame_free(page_t *page);

#endif // FRAME_H
