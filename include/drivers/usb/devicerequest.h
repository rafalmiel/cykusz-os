#ifndef USB_DEVICEREQUEST_H
#define USB_DEVICEREQUEST_H

#include <drivers/usb/usb.h>

typedef enum usb_device_request_request
{
	// USB requests
	usb_device_request_get_status = 0,
	usb_device_request_clear_feature = 1,
	usb_device_request_set_feature = 3,
	usb_device_request_set_address = 5,
	usb_device_request_get_descriptor = 6,
	usb_device_request_set_descriptor = 7,
	usb_device_request_get_configuration = 8,
	usb_device_request_set_configuration = 9,
	usb_device_request_get_interface = 10,
	usb_device_request_set_interface = 11,
	usb_device_request_synch_frame = 12,

	// HID requests
	usb_device_request_get_report = 1,
	usb_device_request_get_idle = 2,
	usb_device_request_get_protocol = 3,
	usb_device_request_set_report = 9,
	usb_device_request_set_idle = 10,
	usb_device_request_set_protocol = 11
} usb_device_request_request_t;

struct usb_device_request
{
	u8 type;
	usb_device_request_request_t request : 8;
	u16 value;
	u16 index;
	u16 length;
} __attribute__((__packed__));

#endif // USB_DEVICEREQUEST_H
