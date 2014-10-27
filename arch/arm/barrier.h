#ifndef BARRIER_H
#define BARRIER_H

/*
 * Data memory barrier
 * No memory access after the DMB can run until all memory accesses before it
 * have completed
 */
#define dmb() asm volatile \
	("mcr p15, #0, r0, c7, c10, #5")


/*
 * Data synchronisation barrier
 * No instruction after the DSB can run until all instructions before it have
 * completed
 */
#define dsb() asm volatile \
	("mcr p15, #0, r0, c7, c10, #4")


/*
 * Clean and invalidate entire cache
 * Flush pending writes to main memory
 * Remove all data in data cache
 */
#define flushcache() asm volatile \
	("mcr p15, #0, r0, c7, c14, #0")

#endif // BARRIER_H
