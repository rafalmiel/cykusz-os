#ifndef UART_H
#define UART_H

#include <core/common.h>

/*
 * Initialize UART0.
 */
void uart_init();

/*
 * Transmit a byte via UART0.
 * uint8_t Byte: byte to send.
 */
void uart_putc(u8 byte);

u8 uart_getc();

/*
 * print a string to the UART one character at a time
 * const char *str: 0-terminated string
 */
void uart_puts(const char *str);

#endif // UART_H
