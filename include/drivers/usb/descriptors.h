#ifndef USB_DESCRIPTORS_H
#define USB_DESCRIPTORS_H

#include <drivers/usb/types.h>

enum usb_descriptor_type
{
	usb_descriptor_type_device = 1,
	usb_descriptor_type_configuration = 2,
	usb_descriptor_type_string = 3,
	usb_descriptor_type_interface = 4,
	usb_descriptor_type_endpoint = 5,
	usb_descriptor_type_device_qualifier = 6,
	usb_descriptor_type_other_speed_config = 7,
	usb_descriptor_type_interface_power = 8,
	usb_descriptor_type_hid = 33,
	usb_descriptor_type_hid_report = 34,
	usb_descriptor_type_hid_physical = 35,
	usb_descriptor_type_hub = 41,
};

enum usb_device_class
{
	usb_device_class_in_interface = 0x00,
	usb_device_class_communications = 0x2,
	usb_device_class_hub = 0x9,
	usb_device_class_diagnostic = 0xdc,
	usb_device_class_miscellaneous = 0xef,
	usb_device_class_vendor_specific = 0xff
};

enum usb_interface_class
{
	usb_interface_class_reserved = 0x0,
	usb_interface_class_audio = 0x1,
	usb_interface_class_communications = 0x2,
	usb_interface_class_hid = 0x3,
	usb_interface_class_physical = 0x5,
	usb_interface_class_image = 0x6,
	usb_interface_class_printer = 0x7,
	usb_interface_class_mass_storage = 0x8,
	usb_interface_class_hub = 0x9,
	usb_interface_class_cdc_data = 0xa,
	usb_interface_class_smart_card = 0xb,
	usb_interface_class_content_security = 0xd,
	usb_interface_class_video = 0xe,
	usb_interface_class_personal_healthcare = 0xf,
	usb_interface_class_audiovideo = 0x10,
	usb_interface_class_diagnostic_device = 0xdc,
	usb_interface_class_wireless_controller = 0xe0,
	usb_interface_class_miscellaneous = 0xef,
	usb_interface_class_application_specific = 0xfe,
	usb_interface_class_vendor_specific = 0xff,
};

struct usb_descriptor_header
{
	u8 descriptor_length;
	enum usb_descriptor_type descriptor_type : 8;
} __attribute__((__packed__));

struct usb_device_descriptor
{
	u8 descriptor_length;
	enum usb_descriptor_type descriptor_type : 8;
	u16 usb_version;
	enum usb_device_class dev_class : 8;
	u8 subclass;
	u8 protocol;
	u8 max_packet_size0;
	u16 vendor_id;
	u16 product_id;
	u16 version;
	u8 manufacturer;
	u8 product;
	u8 serial_number;
	u8 configuration_count;
} __attribute__((__packed__));

struct usb_device_qualifier_descriptor
{
	u8 descriptor_length;
	enum usb_descriptor_type descriptor_type : 8;
	u16 usb_version;
	enum usb_device_class dev_class : 8;
	u8 sub_class;
	u8 protocol;
	u8 max_packet_size0;
	u8 configuration_count;
	u8 reserved9;
} __attribute__((__packed__));

struct usb_configuration_descriptor
{
	u8 descriptor_length;
	enum usb_descriptor_type descriptor_type : 8;
	u16 total_length;
	u8 interface_count;
	u8 configuration_value;
	u8 string_index;
	struct
	{
		u32 __reserved0_4 : 5;
		u32 remote_wakeup : 1;
		u32 self_powered : 1;
		u32 __reserved7 : 1;
	} __attribute__ ((__packed__)) attributes;
	u8 maximum_power;
} __attribute__ ((__packed__));

struct usb_interface_descriptor
{
	u8 descriptor_length;
	enum usb_descriptor_type descriptor_type : 8;
	u8 number;
	u8 alternate_setting;
	u8 endpoint_count;
	enum usb_interface_class int_class : 8;
	u8 subclass;
	u8 protocol;
	u8 string_index;
} __attribute__ ((__packed__));

struct usb_endpoint_descriptor
{
	u8 descriptor_length;
	enum usb_descriptor_type descriptor_type : 8;
	struct
	{
		u32 number : 4;
		u32 __reserved4_6 : 3;
		usb_direction_t direction : 1;
	} __attribute__ ((__packed__)) endpoint_address;

	struct
	{
		usb_transfer_t type;
		enum
		{
			usb_endpoint_desc_no_sync = 0,
			usb_endpoint_desc_async = 1,
			usb_endpoint_desc_adaptive = 2,
			usb_endpoint_desc_synchronous = 3,
		} synchronisation : 2;
		enum
		{
			usb_endpoint_desc_usage_data = 0,
			usb_endpoint_desc_usage_feedback = 1,
			usb_endpoint_desc_usage_implicit_feedback_data = 2
		} usage : 2;
		u32 __reserved6_7 : 2;
	} __attribute__ ((__packed__)) attributes;
	struct
	{
		u32 max_size : 11;
		enum
		{
			usb_endpoint_desc_tx_none = 0,
			usb_endpoint_desc_tx_extra1 = 1,
			usb_endpoint_desc_tx_extra2 = 2
		} transactions : 2;
		u32 __reserved13_15 : 3;
	} __attribute__ ((__packed__)) packet;
	u8 interval;
} __attribute__ ((__packed__));

struct usb_string_descriptor {
	u8 descriptor_length;
	enum usb_descriptor_type descriptor_type : 8;
	u16 data[];
} __attribute__ ((__packed__));

#endif // USB_DESCRIPTORS_H
