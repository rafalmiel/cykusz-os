#ifndef FRAME_H
#define FRAME_H

#include <core/common.h>
#include "paging.h"

void initialise_frames();

void alloc_frame(page_t *page);

void free_frame(page_t *page);

#endif // FRAME_H
