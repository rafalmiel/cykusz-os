#ifndef USB_HUB_H
#define USB_HUB_H

#include <drivers/usb/descriptors.h>
#include <drivers/usb/device.h>
#include <drivers/usb/usb.h>

enum usb_hub_port_control
{
	usb_hub_port_control_global = 0,
	usb_hub_port_control_individual = 1,
};

struct usb_hub_descriptor
{
	u8 descriptor_length;
	enum usb_descriptor_type descriptor_type : 8;
	u8 port_count;
	struct
	{
		enum usb_hub_port_control power_switch_mode : 2;
		u32 compound : 1;
		enum usb_hub_port_control overcurrent_protection : 2;
		u32 think_time : 2;
		u32 indicators : 1;
		u32 __reserved8_15 : 8;
	} __attribute__((packed)) attributes;

	u8 power_good_delay;
	u8 maximum_hub_power;
	u8 data[];
} __attribute__((packed));

struct usb_hub_status
{
	u32 local_power : 1;
	u32 overcurrent : 1;
	u32 __reserved2_15 : 14;
} __attribute__((packed));

struct usb_hub_status_change
{
	u32 local_power_changed : 1;
	u32 overcurrent_changed : 1;
	u32 __reserved2_15 : 14;
} __attribute__((packed));

struct usb_hub_full_status
{
	struct usb_hub_status status;
	struct usb_hub_status_change change;
} __attribute__((packed));

struct usb_hub_port_status
{
	u32 connected : 1;
	u32 enabled : 1;
	u32 suspended : 1;
	u32 overcurrent : 1;
	u32 reset : 1;
	u32 __reserved5_7 : 3;
	u32 power : 1;
	u32 low_speed_attached : 1;
	u32 high_speed_attached : 1;
	u32 test_mode : 1;
	u32 indicator_control : 1;
	u32 __reserved13_15 : 3;
} __attribute__((packed));

struct usb_hub_port_status_change
{
	u32 connected_changed : 1;
	u32 enabled_changed : 1;
	u32 suspended_changed : 1;
	u32 overcurrent_changed : 1;
	u32 reset_changed : 1;
	u32 __reserved5_15 : 11;
} __attribute__((packed));

struct usb_hub_port_full_status
{
	struct usb_hub_port_status status;
	struct usb_hub_port_status_change change;
} __attribute__((packed));

enum usb_hub_port_feature
{
	usb_hub_port_feature_connection = 0,
	usb_hub_port_feature_enable = 1,
	usb_hub_port_feature_suspend = 2,
	usb_hub_port_feature_overcurrent = 3,
	usb_hub_port_feature_reset = 4,
	usb_hub_port_feature_power = 8,
	usb_hub_port_feature_low_speed = 9,
	usb_hub_port_feature_high_speed = 10,
	usb_hub_port_feature_connection_change = 16,
	usb_hub_port_feature_enable_change = 17,
	usb_hub_port_feature_suspend_change = 18,
	usb_hub_port_feature_overcurrent_change = 19,
	usb_hub_port_feature_reset_change = 20,
};

struct usb_hub_device
{
	struct usb_driver_data_header header;
	struct usb_hub_full_status status;
	struct usb_hub_descriptor *descriptor;
	u32 max_children;
	struct usb_hub_port_full_status port_status[USB_MAX_CHILDREN_PER_DEVICE];
	struct usb_device *children[USB_MAX_CHILDREN_PER_DEVICE];
};

enum usb_hub_feature
{
	usb_hub_feature_power = 0,
	usb_hub_feature_overcurrent = 1
};

void usb_hub_load();

usb_result_t usb_hub_attach(struct usb_device *device, u32 interface_number);

#endif // USB_HUB_H
