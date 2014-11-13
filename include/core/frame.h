#ifndef FRAME_H
#define FRAME_H

#include <core/common.h>
#include <asm/paging.h>

void init_frames(u32 *frames_bitmap, u32 frames_size, u32 kernel_end);

void frame_alloc(page_t *page, u32 virt);

void frame_alloc_at(page_t *page, u32 phys_addr, u32 virt);

void frame_free(page_t *page, u32 virt);

#endif // FRAME_H
