#include <drivers/usb/device/hub.h>
#include <drivers/usb/device.h>

void usb_hub_load()
{
	usb_interface_attach_class[usb_interface_class_hub] = usb_hub_attach;
}


usb_result_t usb_hub_attach(struct usb_device *device, u32 interface_number)
{
	(void)device;
	(void)interface_number;
	return usb_result_ok;
}
