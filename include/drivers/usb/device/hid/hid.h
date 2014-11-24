#ifndef USB_HID_H
#define USB_HID_H

#include <drivers/usb/device.h>

void usb_hid_load();

usb_result_t usb_hid_attach(struct usb_device *device, u32 interface_number);

#endif // USB_HID_H
