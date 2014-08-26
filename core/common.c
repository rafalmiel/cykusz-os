#include "common.h"

size_t strlen(const char* str)
{
	size_t ret = 0;
	while (str[ret] != 0)
		ret++;
	return ret;
}


void *memset(void *ptr, int value, u32 size)
{
	u8 *t = (u8*)ptr;
	for(; size > 0; --size) *t++ = value;
	return ptr;
}


void *memcpy(void *destination, const void *source, u32 num)
{
	const u8 *sp = (const u8 *)source;
	u8 *dp = (u8 *)destination;
	for(; num != 0; num--) *dp++ = *sp++;
	return destination;
}
