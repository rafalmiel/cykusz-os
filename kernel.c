#if !defined(__cplusplus)
#include <stdbool.h>
#endif
#include <stddef.h>
#include <stdint.h>

#include <core/io.h>
#include <core/timer.h>

#if defined(__cplusplus)
extern "C"
#endif

void kernel_main(/*struct multiboot *multiboot*/)
{
	init_timer(0x200);
}



