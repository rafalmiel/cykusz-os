#ifndef USB_H
#define USB_H

#include <core/common.h>

typedef enum {
	usb_result_ok = 0,
	usb_error_general = -1,
	usb_error_argument = -2,
	usb_error_retry = -3,
	usb_error_device = -4,
	usb_error_incompatible = -5,
	usb_error_compiler = -6,
	usb_error_memory = -7,
	usb_error_timeout = -8,
	usb_error_disconnect = -9,
} usb_result_t;

typedef enum {
	usb_speed_high = 0,
	usb_speed_full = 1,
	usb_speed_low = 2,
} usb_speed_t;

typedef enum {
	usb_transfer_control = 0,
	usb_transfer_isochronous = 1,
	usb_transfer_bulk = 2,
	usb_transfer_interrupt = 3,
} usb_transfer_t;

typedef enum {
	usb_direction_host_to_device = 0,
	usb_direction_out = 0,
	usb_direction_device_to_host = 1,
	usb_direction_in = 1,
} usb_direction_t;

typedef enum {
	usb_packet_size_bits8,
	usb_packet_size_bits16,
	usb_packet_size_bits32,
	usb_packet_size_bits64,
} usb_packet_size_t;

static inline usb_packet_size_t usb_size_from_number(u32 size)
{
	if (size <= 8)
		return usb_packet_size_bits8;
	else if (size <= 16)
		return usb_packet_size_bits16;
	else if (size <= 32)
		return usb_packet_size_bits32;
	else
		return usb_packet_size_bits64;
}

static inline u32 usb_size_to_number(usb_packet_size_t size)
{
	switch (size) {
	case usb_packet_size_bits8:
		return 8;
	case usb_packet_size_bits16:
		return 16;
	case usb_packet_size_bits32:
		return 32;
	case usb_packet_size_bits64:
		return 64;
	}

	kprint("usb_size_to_number error\n");
	return 0;
}

#endif // USB_H
