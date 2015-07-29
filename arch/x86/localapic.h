#ifndef LOCALAPIC_H
#define LOCALAPIC_H

#include <core/common.h>

void lapic_set_base(u32 *base);

void init_lapic(void);

void lapic_end_of_interrupt(void);

#endif // LOCALAPIC_H

