#ifndef ORDERED_ARRAY_H
#define ORDERED_ARRAY_H

#include <core/common.h>

typedef void* ordarr_type_t;

typedef s8 (*ordarr_lessthan_pred_t)(ordarr_type_t, ordarr_type_t);

typedef struct
{
	ordarr_type_t *array;
	u32 size;
	u32 max_size;
	ordarr_lessthan_pred_t less_than;
} ordarr_t;

s8 ordarr_lessthan_predicate(ordarr_type_t a, ordarr_type_t b);

#ifndef __arch_arm
ordarr_t create_ordarr(u32 max_size,
		       ordarr_lessthan_pred_t less_than);
#endif

ordarr_t place_ordarr(void *addr, u32 max_size,
		      ordarr_lessthan_pred_t less_than);

void destroy_ordarr(ordarr_t *array);

void ordarr_insert(ordarr_type_t item, ordarr_t *array);

ordarr_type_t ordarr_lookup(u32 i, ordarr_t *array);

void ordarr_remove(u32 i, ordarr_t *array);

void ordarr_remove_by_val(ordarr_type_t value, ordarr_t *array);

void ordarr_print(ordarr_t *array, void(*p)(ordarr_type_t el));

#endif // ORDERED_ARRAY_H
