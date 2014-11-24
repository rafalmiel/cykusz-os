#include <drivers/usb/device/hid/hid.h>

void usb_hid_load()
{
	usb_interface_attach_class[usb_interface_class_hid] = usb_hid_attach;
}

usb_result_t usb_hid_attach(struct usb_device *device, u32 interface_number)
{
	kprint("Attaching hid driver\n");
	(void)device;
	(void)interface_number;
	return usb_result_ok;
}
