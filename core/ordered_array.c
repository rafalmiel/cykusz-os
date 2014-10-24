#ifndef __arch_arm
#include <arch/x86/kheap.h>
#include <core/io.h>
#endif

#include <core/ordered_array.h>

s8 ordarr_lessthan_predicate(ordarr_type_t a, ordarr_type_t b)
{
	return (a < b) ? 1 : 0;
}

#ifndef __arch_arm
ordarr_t create_ordarr(u32 max_size,
				     ordarr_lessthan_pred_t less_than)
{
	ordarr_t ret;
	ret.array = (void*)kmalloc(max_size * sizeof(ordarr_type_t));
	memset(ret.array, 0, max_size * sizeof(ordarr_type_t));
	ret.size = 0;
	ret.max_size = max_size;
	ret.less_than = less_than;
	return ret;
}
#endif

ordarr_t place_ordarr(void *addr, u32 max_size,
				    ordarr_lessthan_pred_t less_than)
{
	ordarr_t ret;
	ret.array = (ordarr_type_t*)addr;
	memset(ret.array, 0, max_size * sizeof(ordarr_type_t));
	ret.size = 0;
	ret.max_size = max_size;
	ret.less_than = less_than;
	return ret;
}

void destroy_ordarr(ordarr_t *array)
{
#ifndef __arch_arm
	kfree((u32)array);
#else
	(void)array;
#endif
}

void ordarr_insert(ordarr_type_t item, ordarr_t *array)
{
	u32 iterator = 0;
	while (iterator < array->size
	       && array->less_than(array->array[iterator], item))
		++iterator;

	if (iterator == array->size)
		array->array[array->size++] = item;
	else {
		ordarr_type_t tmp = array->array[iterator];
		array->array[iterator] = item;

		while (iterator < array->size) {
			++iterator;
			ordarr_type_t tmp2 = array->array[iterator];
			array->array[iterator] = tmp;
			tmp = tmp2;
		}
		++array->size;
	}
}

ordarr_type_t ordarr_lookup(u32 i, ordarr_t *array)
{
	return array->array[i];
}

void ordarr_remove(u32 i, ordarr_t *array)
{
	while (i < array->size) {
		array->array[i] = array->array[i+1];
		++i;
	}
	--array->size;
}

#ifndef __arch_arm
void ordarr_print(ordarr_t *array, void (*p)(ordarr_type_t))
{
	for (u32 i = 0; i < array->size; ++i)
		p(ordarr_lookup(i, array));
	vga_writestring("--------------------\n");
}
#endif


void ordarr_remove_by_val(ordarr_type_t value, ordarr_t *array)
{
	u32 iterator = 0;
	while ((iterator < array->size)
	       && (ordarr_lookup(iterator, array)) != value)
		++iterator;

	if (iterator < array->size)
		ordarr_remove(iterator, array);
}
