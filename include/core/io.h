#ifndef IO_H
#define IO_H

#include <stdint.h>

#include <core/common.h>

void init_output();

void kprint(const char *str);

void kprint_char(char c);
void kprint_int(u32 val);
void kprint_hex(u32 val);
void kprint_intnl(u32 val);
void kprint_hexnl(u32 val);

#endif // IO_H
