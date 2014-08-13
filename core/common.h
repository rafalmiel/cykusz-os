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

static inline void outb(u16 port, u8 val)
{
	asm volatile ( "outb %0, %1"
		       :
		       : "a"(val), "Nd"(port)
		     );
}

static inline u8 inb(u16 port)
{
	u8 ret;
	asm volatile ( "inb %1, %0"
		       : "=a"(ret)
		       : "Nd"(port)
		     );
	return ret;
}

static inline void outw(u16 port, u16 val)
{
	asm volatile ( "outw %0, %1"
		       :
		       : "a"(val), "Nd"(port)
		     );
}

static inline u16 inw(u16 port)
{
	u16 ret;
	asm volatile ( "inw %1, %0"
		       : "=a"(ret)
		       : "Nd"(port)
		     );
	return ret;
}

static inline void outl(u16 port, u32 val)
{
	asm volatile ( "outl %0, %1"
		       :
		       : "a"(val), "Nd"(port)
		     );
}

static inline u32 inl(u16 port)
{
	u32 ret;
	asm volatile ( "inl %1, %0"
		       : "=a"(ret)
		       : "Nd"(port)
		     );
	return ret;
}

#endif // COMMON_H
