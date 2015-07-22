#include <core/common.h>

size_t strlen(const char* str)
{
	size_t ret = 0;
	while (str[ret] != 0)
		ret++;
	return ret;
}

s32 strcmp(const char *s1, const char *s2)
{
	while(*s1 && (*s1==*s2))
		s1++,s2++;
	return *(const u8*)s1-*(const u8*)s2;
}

char *strcpy(char *dest, const char *src)
{
	char *ret = dest;
	while ((*dest++ = *src++))
		;
	return ret;
}

char *strcat(char *dest, const char *src)
{
	char *ret = dest;
	while (*dest)
		dest++;
	while ((*dest++ = *src++))
		;
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


s32 memcmp(const void *s1, const void *s2, u32 n)
{
	u32 i, a, b;
	for (i = 0; i < n; ++i) {
		a = *(u8*)s1;
		b = *(u8*)s2;
		if (a != b) {
			return a - b;
		}

		(u8*)s1++;
		(u8*)s2++;
	}

	return 0;
}
