#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stddef.h>

typedef int8_t s8;
typedef uint8_t u8;
typedef int16_t s16;
typedef uint16_t u16;
typedef int32_t s32;
typedef uint32_t u32;

size_t strlen(const char* str);
s32 strcmp(const char *s1, const char *s2);
char *strcpy(char *dest, const char *src);
char *strcat(char *dest, const char *src);
void *memset(void *ptr, int value, u32 size);
void *memcpy (void *destination, const void *source, u32 num);

static inline u32 align(u32 addr, u32 to)
{
	if (addr & (to - 1)) {
		addr &= ~(to - 1);
		addr += to;
	}

	return addr;
}

static inline u32 align_4K(u32 addr)
{
	return align(addr, 0x1000);
}

static inline u32 align_32B(u32 addr)
{
	return align(addr, 0x20);
}

#endif // COMMON_H
