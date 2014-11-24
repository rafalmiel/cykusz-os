#include <core/kheap.h>

#include <drivers/usb/usb.h>
#include <drivers/usb/device.h>

#include <drivers/usb/device/hid/hid.h>
#include <drivers/usb/device/hub.h>

#include <drivers/usb/hcd/dwc/dwc.h>

#define USB_CONTROL_MESSAGE_TIMEOUT 10
#define USB_MAX_DEVICES 32

static struct usb_device *usb_devices[USB_MAX_DEVICES];

usb_result_t (*usb_interface_attach_class[USB_INTERFACE_CLASS_ATTACH_COUNT])
		(struct usb_device *device, u32 interface_number);


usb_result_t usb_attach_root_hub();

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
	usb_attach_root_hub();
}

usb_result_t usb_control_message(struct usb_device *device,
				 struct usb_pipe_address pipe,
				 void *buffer, u32 buffer_length,
				 struct usb_device_request *request,
				 u32 timeout)
{
	usb_result_t result;

	if (((u32)buffer & 0x3) != 0)
		kprint("Warning message buffer not word aligned\n");

	result = hcd_submit_control_message(device, pipe, buffer, buffer_length,
					    request);

	if (result != usb_result_ok) {
		kprint("Failed to send message\n");
		return result;
	}

	while (timeout-- > 0 && (device->error & usb_transfer_error_processing)) {
		micro_delay(1000);
	}

	if ((device->error & usb_transfer_error_processing)) {
		kprint("Message timeout reached\n");
		return usb_error_timeout;
	}

	if (device->error & ~usb_transfer_error_processing) {
		if (device->parent && device->parent->device_check_connection) {
			result = device->parent->device_check_connection(device->parent,
									 device);
			if (result != usb_result_ok) {
				return usb_error_disconnect;
			}
		}

		result = usb_error_device;
	}

	return result;
}

usb_result_t usb_get_descriptor(struct usb_device *device,
				enum usb_descriptor_type type,
				u8 index, u16 lang_id, void *buffer, u32 length,
				u32 minimum_length, u8 recipient)
{
	usb_result_t result;

	if ((result = usb_control_message(
		device,
		(struct usb_pipe_address) {
			.type = usb_transfer_control,
			.speed = device->speed,
			.endpoint = 0,
			.device = device->number,
			.direction = usb_direction_in,
			.max_size = usb_size_from_number(device->descriptor.max_packet_size0),
		},
		buffer,
		length,
		&(struct usb_device_request) {
			.request = usb_device_request_get_descriptor,
			.type = 0x80 | recipient,
			.value = (u16)type << 8 | index,
			.index = lang_id,
			.length = length,
		},
		USB_CONTROL_MESSAGE_TIMEOUT)) != usb_result_ok) {
		kprint("Failed to get descriptor for device\n");
		return result;
	}

	if (device->last_transfer < minimum_length) {
		kprint("Unexpectedly short descriptor\n");
		return usb_error_device;
	}

	return usb_result_ok;
}

static usb_result_t usb_get_string(struct usb_device *device, u8 string_index,
				   u16 lang_id, void *buffer, u32 length)
{
	usb_result_t result;

	for (u32 i = 0; i < 3; ++i) {
		result = usb_get_descriptor(device, usb_descriptor_type_string,
					    string_index, lang_id, buffer,
					    length, length, 0);

		if (result == usb_result_ok)
			break;
	}

	return result;
}

static usb_result_t usb_read_string_lang(struct usb_device *device,
					 u8 string_index, u16 lang_id,
					 void *buffer, u32 length)
{
	usb_result_t result;

	result = usb_get_string(device, string_index, lang_id, buffer,
				USB_MIN(2, length, u32));

	if (result == usb_result_ok && device->last_transfer != length)
		result = usb_get_string(device, string_index, lang_id, buffer,
					USB_MIN(((u8*)buffer)[0], length, u32));

	return result;
}

static usb_result_t usb_read_string(struct usb_device *device, u8 string_index,
				    char *buffer, u32 length)
{
	usb_result_t result;
	u32 i; u8 descriptor_length;

	if (buffer == 0 || string_index == 0)
		return usb_error_argument;

	u16 lang_ids[2];
	struct usb_string_descriptor *descriptor;

	result = usb_read_string_lang(device, 0, 0, &lang_ids, 4);

	if (result != usb_result_ok) {
		kprint("Error getting language list\n");
		return result;
	} else if (device->last_transfer < 4) {
		kprint("Unexpectedly short language list\n");
		return usb_error_device;
	}

	descriptor = (struct usb_string_descriptor*)buffer;

	if (descriptor == 0)
		return usb_error_memory;

	if ((result = usb_read_string_lang(device, string_index, lang_ids[1],
					   descriptor, length)) != usb_result_ok)
		return result;

	descriptor_length = descriptor->descriptor_length;
	for (i = 0; i < length && i < (u32)(descriptor_length - 2) >> 1; ++i) {
		if (descriptor->data[i] > 0xff)
			buffer[i] = '?';
		else
			buffer[i] = descriptor->data[i];
	}

	if (i < length)
		buffer[i++] = '\0';

	return result;
}

static usb_result_t usb_read_device_descriptor(struct usb_device *device)
{
	usb_result_t result;

	if (device->speed == usb_speed_low) {
		device->descriptor.max_packet_size0 = 8;

		if ((result = usb_get_descriptor(device,
						 usb_descriptor_type_device,
						 0, 0,
						 (void*)&device->descriptor,
						 sizeof(device->descriptor),
						 8, 0)) != usb_result_ok) {
			return usb_result_ok;
		}

		if (device->last_transfer == sizeof(struct usb_device_descriptor))
			return result;

		return usb_get_descriptor(device, usb_descriptor_type_device,
					  0, 0,
					  (void*)&device->descriptor,
					  sizeof(device->descriptor),
					  sizeof(device->descriptor), 0);
	} else if (device->speed == usb_speed_full) {
		device->descriptor.max_packet_size0 = 64;

		if ((result = usb_get_descriptor(device,
						 usb_descriptor_type_device,
						 0, 0,
						 (void*)&device->descriptor,
						 sizeof(device->descriptor),
						 8, 0)) != usb_result_ok) {
			return usb_result_ok;
		}

		if (device->last_transfer == sizeof(struct usb_device_descriptor))
			return result;

		return usb_get_descriptor(device, usb_descriptor_type_device,
					  0, 0,
					  (void*)&device->descriptor,
					  sizeof(device->descriptor),
					  sizeof(device->descriptor), 0);
	} else {
		device->descriptor.max_packet_size0 = 64;
		return usb_get_descriptor(device, usb_descriptor_type_device,
					  0, 0,
					  (void*)&device->descriptor,
					  sizeof(device->descriptor),
					  sizeof(device->descriptor), 0);
	}
}

static usb_result_t usb_set_address(struct usb_device *device, u8 address)
{
	usb_result_t result;

	if (device->status != usb_device_status_default) {
		kprint("Illegal attempt to address device\n");
		return usb_error_device;
	}

	if ((result = usb_control_message(
		device,
		(struct usb_pipe_address) {
			.type = usb_transfer_control,
			.speed = device->speed,
			.endpoint = 0,
			.device = 0,
			.direction = usb_direction_out,
			.max_size = usb_size_from_number(device->descriptor.max_packet_size0),
		},
		0,
		0,
		&(struct usb_device_request) {
			.request = usb_device_request_set_address,
			.type = 0,
			.value = address,
		},
		USB_CONTROL_MESSAGE_TIMEOUT)) != usb_result_ok) {

		return result;
	}

	micro_delay(10000);

	device->number = address;
	device->status = usb_device_status_addressed;

	return usb_result_ok;
}

static usb_result_t usb_set_configuration(struct usb_device *device, u8 conf)
{
	usb_result_t result;

	kprint("Device status: ");
	kprint_hexnl(device->status);

	if (device->status != usb_device_status_addressed) {
		kprint("Illegal attempt to configure device\n");
		return usb_error_device;
	}

	if ((result = usb_control_message(
		device,
		(struct usb_pipe_address) {
			.type = usb_transfer_control,
			.speed = device->speed,
			.endpoint = 0,
			.device = device->number,
			.direction = usb_direction_out,
			.max_size = usb_size_from_number(device->descriptor.max_packet_size0),
		},
		0,
		0,
		&(struct usb_device_request) {
			.request = usb_device_request_set_configuration,
			.type = 0,
			.value = conf,
		},
		USB_CONTROL_MESSAGE_TIMEOUT)) != usb_result_ok) {

		return result;
	}

	device->configuration_index = conf;
	device->status = usb_device_status_configured;

	return usb_result_ok;
}

static usb_result_t usb_configure(struct usb_device *device, u8 configuration)
{
	usb_result_t result;
	void *full_descriptor;
	struct usb_descriptor_header *header;
	struct usb_interface_descriptor *interface;
	struct usb_endpoint_descriptor *endpoint;
	u32 last_interface, last_endpoint;
	u32 is_alternate;

	if (device->status != usb_device_status_addressed) {
		kprint("Illegal attempt to configure unadressed device\n");
		return usb_error_device;
	}

	if ((result = usb_get_descriptor(device, usb_descriptor_type_configuration,
					 configuration, 0,
					 (void*)&device->configuration,
					 sizeof(device->configuration),
					 sizeof(device->configuration),
					 0)) != usb_result_ok) {
		kprint("Failed to retrieve configuration descriptor\n");
		return result;
	}

	kprint("Configuration total length: ");
	kprint_hexnl(device->configuration.total_length);

	if ((full_descriptor = (void*)kmalloc(device->configuration.total_length)) == 0) {
		kprint("Failed to allocate space for descriptor.\n");
		return usb_error_memory;
	}

	if ((result = usb_get_descriptor(device, usb_descriptor_type_configuration,
					 configuration, 0,
					 full_descriptor,
					 device->configuration.total_length,
					 device->configuration.total_length,
					 0)) != usb_result_ok) {
		kprint("Failed to retrieve full configuration descriptor\n");
		goto deallocate;
	}

	device->configuration_index = configuration;
	configuration = device->configuration.configuration_value;

	header = full_descriptor;
	last_interface = USB_MAX_INTERFACES_PER_DEVICE;
	last_endpoint = USB_MAX_ENDPOINTS_PER_DEVICE;
	is_alternate = 0;

	for (header = (struct usb_descriptor_header*)((u8*)header + header->descriptor_length);
	     (u32)header - (u32)full_descriptor < device->configuration.total_length;
	     header = (struct usb_descriptor_header*)((u8*)header + header->descriptor_length)) {
		switch (header->descriptor_type) {
		case usb_descriptor_type_interface:
			kprint("Bind interface\n");
			interface = (struct usb_interface_descriptor*)header;
			if (last_interface != interface->number) {
				memcpy((void*)&device->interfaces[last_interface = interface->number],
				       (void*)interface,
				       sizeof(struct usb_interface_descriptor));
				last_endpoint = 0;
				is_alternate = 0;
			} else {
				is_alternate = 1;
			}
			break;
		case usb_descriptor_type_endpoint:
			kprint("Bind endpoint\n");
			if (is_alternate)
				break;
			if (last_interface == USB_MAX_INTERFACES_PER_DEVICE
			    || last_endpoint >= device->interfaces[last_interface].endpoint_count) {
				kprint("Unexpected endpoint descriptor in interface\n");
				break;
			}
			endpoint = (struct usb_endpoint_descriptor*)header;
			memcpy((void*)&device->endpoints[last_interface][last_endpoint++],
			       (void*)endpoint,
			       sizeof(struct usb_endpoint_descriptor));
			break;
		default:
			kprint("Bind default");
			if (header->descriptor_length == 0)
				goto header_loop_break;

			break;
		}
	}

header_loop_break:

	if ((result = usb_set_configuration(device, configuration)) != usb_result_ok) {
		goto deallocate;
	}

	device->full_configuration = full_descriptor;

	return usb_result_ok;

deallocate:
	kfree((u32)full_descriptor);
	return result;
}

usb_result_t usb_attach_device(struct usb_device *device)
{
	usb_result_t result;
	u8 address;
	char *buffer;

	address = device->number;
	device->number = 0;

	if ((result = usb_read_device_descriptor(device)) != usb_result_ok) {
		kprint("Failed to read device descriptor\n");
		device->number = address;
		return result;
	}

	device->status = usb_device_status_default;

	if (device->parent && device->parent->device_child_reset) {
		if ((result = device->parent->device_child_reset(device->parent,
								 device))
				!= usb_result_ok) {
			kprint("Failed to reset port again for new device\n");
			device->number = address;
			return result;
		}
	}

	if ((result = usb_set_address(device, address)) != usb_result_ok) {
		kprint("Failed to assign address\n");
		device->number = address;
		return result;
	}

	device->number = address;

	if ((result = usb_read_device_descriptor(device)) != usb_result_ok) {
		kprint("Failed to reread device descriptor\n");
		device->number = address;
		return result;
	}

	buffer = 0;

	if (device->descriptor.product != 0) {
		if (buffer == 0)
			buffer = (char*)kmalloc(0x100);
		if (buffer != 0) {
			result = usb_read_string(device,
						 device->descriptor.product,
						 buffer, 0x100);
			if (result == usb_result_ok) {
				kprint("Product: ");
				kprint(buffer); kprint("\n");
			}
		}
	}

	if (device->descriptor.manufacturer != 0) {
		if (buffer == 0)
			buffer = (char*)kmalloc(0x100);
		if (buffer != 0) {
			result = usb_read_string(device,
						 device->descriptor.manufacturer,
						 buffer, 0x100);
			if (result == usb_result_ok) {
				kprint("Manufacturer: ");
				kprint(buffer); kprint("\n");
			}
		}
	}

	if (device->descriptor.serial_number != 0) {
		if (buffer == 0)
			buffer = (char*)kmalloc(0x100);
		if (buffer != 0) {
			result = usb_read_string(device,
						 device->descriptor.serial_number,
						 buffer, 0x100);
			if (result == usb_result_ok) {
				kprint("Serial Number: ");
				kprint(buffer); kprint("\n");
			}
		}
	}

	if (buffer != 0)
		kfree((u32)buffer);

	kprint("Configuring device\n");

	if ((result = usb_configure(device, 0)) != usb_result_ok) {
		kprint("Failed to configure device\n");
		return usb_result_ok;
	}

	if (device->configuration.string_index != 0) {
		if (buffer == 0)
			buffer = (char*)kmalloc(0x100);
		if (buffer != 0) {
			result = usb_read_string(device,
						 device->configuration.string_index,
						 buffer, 0x100);
			if (result == usb_result_ok) {
				kprint("Configuration: ");
				kprint(buffer); kprint("\n");
			}
		}
	}

	kprint("starting attached interface!\n");
	kprint_hexnl(device->interfaces[0].int_class);

	if (device->interfaces[0].int_class < USB_INTERFACE_CLASS_ATTACH_COUNT
	    && usb_interface_attach_class[device->interfaces[0].int_class]) {
		kprint("starting attached interface!\n");
		if ((result = usb_interface_attach_class[device->interfaces[0].int_class](device, 0))
				!= usb_result_ok) {
			kprint("Could not start the driver\n");
		}
	}

	return usb_result_ok;
}

usb_result_t usb_allocate_device(struct usb_device **device)
{
	*device = (struct usb_device*)kmalloc(sizeof(struct usb_device));

	if (*device == 0) {
		return usb_error_memory;
	}

	for (u32 number = 0; number < USB_MAX_DEVICES; ++number) {
		if (usb_devices[number] == 0) {
			usb_devices[number] = *device;
			(*device)->number = number + 1;
			break;
		}
	}

	(*device)->status = usb_device_status_attached;
	(*device)->error = usb_transfer_no_error;
	(*device)->port_number = 0;
	(*device)->driver_data = 0;
	(*device)->full_configuration = 0;
	(*device)->configuration_index = 0xff;
	(*device)->device_deallocate = 0;
	(*device)->device_detached = 0;
	(*device)->device_check_connection = 0;
	(*device)->device_check_for_change = 0;
	(*device)->device_child_detached = 0;
	(*device)->device_child_reset = 0;

	return usb_result_ok;
}


void usb_deallocate_device(struct usb_device *device)
{
	if (device->device_detached)
		device->device_detached(device);

	if (device->device_deallocate)
		device->device_deallocate(device);

	if (device->parent && device->parent->device_child_detached)
		device->parent->device_child_detached(device->parent, device);

	if (device->status == usb_device_status_addressed
	    || device->status == usb_device_status_configured)
		if (device->number > 0 && device->number <= USB_MAX_DEVICES
		    && usb_devices[device->number - 1] == device)
			usb_devices[device->number - 1] = 0;

	if (device->full_configuration)
		kfree((u32)device->full_configuration);

	kfree((u32)device);
}

usb_result_t usb_attach_root_hub(void)
{
	usb_result_t result;
	struct usb_device *root_hub;

	if (usb_devices[0])
		usb_deallocate_device(usb_devices[0]);

	if ((result = usb_allocate_device(&root_hub)) != usb_result_ok)
		return result;

	usb_devices[0]->status = usb_device_status_powered;

	return usb_attach_device(usb_devices[0]);
}

void usb_check_for_change(void)
{
	if (usb_devices[0]
	    && usb_devices[0]->device_check_for_change) {
		usb_devices[0]->device_check_for_change(usb_devices[0]);
	}
}
