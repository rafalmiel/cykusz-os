#ifndef USB_H
#define USB_H

#include <core/common.h>
#include <core/io.h>

#include <drivers/usb/types.h>

#include <drivers/usb/pipe.h>
#include <drivers/usb/device.h>
#include <drivers/usb/devicerequest.h>

void usb_initialise(u32 reg_base);

usb_result_t usb_attach_device(struct usb_device *device);

usb_result_t usb_control_message(struct usb_device *device,
				 struct usb_pipe_address pipe,
				 void *buffer, u32 buffer_length,
				 struct usb_device_request *request,
				 u32 timeout);

usb_result_t usb_get_descriptor(struct usb_device *device,
				enum usb_descriptor_type type,
				u8 index, u16 lang_id, void *buffer, u32 length,
				u32 minimum_length, u8 recipient);

usb_result_t usb_allocate_device(struct usb_device **device);

void usb_deallocate_device(struct usb_device *device);

void usb_check_for_change(void);

#endif // USB_H
