#ifndef USB_PIPE_H
#define USB_PIPE_H

#include <drivers/usb/usb.h>

struct usb_pipe_address
{
	usb_packet_size_t max_size : 2;
	usb_speed_t speed : 2;
	u32 endpoint : 4;
	u32 device : 8;
	usb_transfer_t type : 2;
	usb_direction_t direction : 1;
	u32 __reserved19_31 : 13;
} __attribute__((__packed__));

#endif // USB_PIPE_H
