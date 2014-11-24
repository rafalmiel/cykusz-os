#ifndef USB_DEVICE_H
#define USB_DEVICE_H

#include <drivers/usb/descriptors.h>
#include <drivers/usb/usb.h>

#define USB_MAX_CHILDREN_PER_DEVICE 10

#define USB_MAX_INTERFACES_PER_DEVICE 8

#define USB_MAX_ENDPOINTS_PER_DEVICE 16

#define USB_INTERFACE_CLASS_ATTACH_COUNT 16

struct usb_device;

extern usb_result_t (*usb_interface_attach_class[USB_INTERFACE_CLASS_ATTACH_COUNT])
			(struct usb_device *device, u32 interface_number);

enum usb_device_status
{
	usb_device_status_attached = 0,
	usb_device_status_powered = 1,
	usb_device_status_default = 2,
	usb_device_status_addressed = 3,
	usb_device_status_configured = 4
};

enum usb_transfer_error
{
	usb_transfer_no_error = 0,
	usb_transfer_error_stall = (1 << 1),
	usb_transfer_error_buffer = (1 << 2),
	usb_transfer_error_babble = (1 << 3),
	usb_transfer_error_no_acknowledge = (1 << 4),
	usb_transfer_error_crc = (1 << 5),
	usb_transfer_error_bit = (1 << 6),
	usb_transfer_error_connection = (1 << 7),
	usb_transfer_error_abh = (1 << 8),
	usb_transfer_error_notyet = (1 << 9),
	usb_transfer_error_processing = (1 << 31)
};

struct usb_driver_data_header
{
	u32 device_driver;
	u32 data_size;
};

struct usb_device
{
	u32 number;

	usb_speed_t speed;
	enum usb_device_status status;

	volatile u8 configuration_index;
	u8 port_number;

	volatile enum usb_transfer_error error __attribute__((aligned(4)));

	void (*device_detached)(struct usb_device *device)
						__attribute__((aligned(4)));

	void (*device_deallocate)(struct usb_device *device)
						__attribute__((aligned(4)));

	void (*device_check_for_change)(struct usb_device *device)
						__attribute__((aligned(4)));

	void (*device_child_detached)(struct usb_device *device,
				      struct usb_device *child)
						__attribute__((aligned(4)));

	void (*device_child_reset)(struct usb_device *device,
				   struct usb_device *child)
						__attribute__((aligned(4)));

	void (*device_check_connection)(struct usb_device *device,
					struct usb_device *child)
						__attribute__((aligned(4)));

	volatile struct usb_device_descriptor descriptor __attribute__((aligned(4)));
	volatile struct usb_configuration_descriptor configuration __attribute__((aligned(4)));
	volatile struct usb_interface_descriptor interfaces[USB_MAX_INTERFACES_PER_DEVICE] __attribute__((aligned(4)));
	volatile struct usb_endpoint_descriptor endpoints[USB_MAX_INTERFACES_PER_DEVICE][USB_MAX_ENDPOINTS_PER_DEVICE] __attribute__((aligned(4)));
	struct usb_device *parent __attribute__((aligned(4)));
	volatile void *full_configuration;
	volatile struct usb_driver_data_header *driver_data;
	volatile u32 last_transfer;
};

#endif // DEVICE_H
