#ifndef MAILBOX_H
#define MAILBOX_H

#include <core/common.h>

void mailbox_write(u8 channel, u32 data28);
u32 mailbox_read(u8 channel);

#endif // MAILBOX_H
