
#include <drivers/usb/usb.h>
#include <drivers/usb/device.h>

#include <drivers/usb/device/hid/hid.h>
#include <drivers/usb/device/hub.h>

#include <drivers/usb/hcd/dwc/dwc.h>

#define USB_CONTROL MESSAGE_TIMEOUT 10
#define USB_MAX_DEVICES 32

static struct usb_device *usb_devices[USB_MAX_DEVICES];

usb_result_t (*usb_interface_attach_class[USB_INTERFACE_CLASS_ATTACH_COUNT])
		(struct usb_device *device, u32 interface_number);

void usb_load()
{
	kprint("Loading USB driver\n");
	for (u32 i = 0; i < USB_MAX_DEVICES; ++i)
		usb_devices[i] = 0;
	for (u32 i = 0; i < USB_INTERFACE_CLASS_ATTACH_COUNT; ++i)
		usb_interface_attach_class[i] = 0;
}


void usb_initialise(u32 reg_base)
{
	usb_load();
	usb_hid_load();
	usb_hub_load();

	init_dwc(reg_base);
}
