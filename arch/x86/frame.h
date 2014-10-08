#ifndef FRAME_H
#define FRAME_H

#include <core/common.h>
#include "paging.h"

void init_frames();

void frame_alloc(page_t *page);

void frame_alloc_at(u32 phys_addr, page_t *page);

void frame_free(page_t *page);

#endif // FRAME_H
