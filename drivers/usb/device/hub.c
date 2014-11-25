#include <core/kheap.h>

#include <drivers/usb/hcd/dwc/dwc.h>

#include <drivers/usb/device/hub.h>
#include <drivers/usb/device.h>

#define USB_CONTROL_MESSAGE_TIMEOUT 10

void usb_hub_load()
{
	usb_interface_attach_class[usb_interface_class_hub] = usb_hub_attach;
}

static usb_result_t usb_hub_read_descriptor(struct usb_device *device)
{
	struct usb_descriptor_header header;
	usb_result_t result;
	struct usb_hub_device *hubdev = (struct usb_hub_device*)device->driver_data;

	if ((result = usb_get_descriptor(device, usb_descriptor_type_hub,
					 0, 0, &header, sizeof(header),
					 sizeof(header), 0x20))
			!= usb_result_ok) {
		kprint("HUB: Failed to read hub descriptor\n");
	}

	if (hubdev->descriptor == 0) {
		hubdev->descriptor = (struct usb_hub_descriptor*)kmalloc(header.descriptor_length);

		if (hubdev->descriptor == 0) {
			kprint("HUB: Not enough memory to read hub descriptor\n");

			return usb_error_memory;
		}
	}

	if ((result = usb_get_descriptor(device, usb_descriptor_type_hub,
					 0, 0, hubdev->descriptor,
					 header.descriptor_length,
					 header.descriptor_length, 0x20))
			!= usb_result_ok) {
		kprint("HUB: Failed to read hub descriptor\n");
		return result;
	}

	return usb_result_ok;
}

static usb_result_t usb_hub_get_status(struct usb_device *device) {
	usb_result_t result;
	struct usb_hub_device *hubdev = (struct usb_hub_device*)device->driver_data;

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
		&hubdev->status,
		sizeof (struct usb_hub_full_status),
		&(struct usb_device_request) {
			.request = usb_device_request_get_status,
			.type = 0xa0,
			.length = sizeof(struct usb_hub_full_status),
		},
		USB_CONTROL_MESSAGE_TIMEOUT)) != usb_result_ok) {

		return result;
	}

	if (device->last_transfer < sizeof(struct usb_hub_full_status)) {
		kprint("HUB: Failed to read hub status\n");
		return usb_error_device;
	}

	return usb_result_ok;
}

static usb_result_t usb_hub_port_get_status(struct usb_device *device, u8 port)
{
	usb_result_t result;
	struct usb_hub_device *hubdev = (struct usb_hub_device*)device->driver_data;

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
		&(hubdev->port_status[port]),
		sizeof (struct usb_hub_port_status),
		&(struct usb_device_request) {
			.request = usb_device_request_get_status,
			.type = 0xa3,
			.index = port + 1,
			.length = sizeof(struct usb_hub_port_status),
		},
		USB_CONTROL_MESSAGE_TIMEOUT)) != usb_result_ok) {

		return result;
	}

	if (device->last_transfer < sizeof(struct usb_hub_port_full_status)) {
		kprint("HUB: Failed to read hub port status\n");
		return usb_error_device;
	}

	return usb_result_ok;
}

static usb_result_t usb_hub_change_feature(struct usb_device *device,
					   enum usb_hub_feature feature,
					   u32 set)
{
	usb_result_t result;

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
			.request = (set) ? usb_device_request_set_feature
					 : usb_device_request_clear_feature,
			.type = 0x20,
			.value = (u16)feature,
		},
		USB_CONTROL_MESSAGE_TIMEOUT)) != usb_result_ok) {

		return result;
	}

	return usb_result_ok;
}

static usb_result_t usb_hub_change_port_feature(struct usb_device *device,
						enum usb_hub_feature feature,
						u8 port,
						u32 set)
{
	usb_result_t result;

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
			.request = (set) ? usb_device_request_set_feature
					 : usb_device_request_clear_feature,
			.type = 0x23,
			.value = (u16)feature,
			.index = port + 1,
		},
		USB_CONTROL_MESSAGE_TIMEOUT)) != usb_result_ok) {

		return result;
	}

	return usb_result_ok;
}

static usb_result_t usb_hub_power_on(struct usb_device *device)
{
	struct usb_hub_device *data;
	struct usb_hub_descriptor *hub_desc;

	data = (struct usb_hub_device*)device->driver_data;
	hub_desc = data->descriptor;

	kprint("Powering on hub\n");

	for (u32 i = 0; i < data->max_children; ++i) {
		if (usb_hub_change_port_feature(device,
						usb_hub_port_feature_power,
						i, 1)
				!= usb_result_ok) {
			kprint("HUB: Could not power port: ");
			kprint_hexnl(i);
		}
	}

	micro_delay(hub_desc->power_good_delay * 2000);

	return usb_result_ok;
}

usb_result_t usb_hub_port_reset(struct usb_device *device, u8 port)
{
	usb_result_t result;
	struct usb_hub_device *data;
	struct usb_hub_port_full_status *port_status;
	u32 retry, timeout;

	data = (struct usb_hub_device*)device->driver_data;
	port_status = &data->port_status[port];

	kprint("HUB: Hub reset. Port ");
	kprint_hexnl(port + 1);

	for (retry = 0; retry < 3; ++retry) {
		if ((result = usb_hub_change_port_feature(device,
							  usb_hub_port_feature_reset,
							  port, 1))
				!= usb_result_ok) {
			kprint("HUB: Failed to reset. Port");
			kprint_hexnl(port + 1);

			return result;
		}

		timeout = 0;

		do {
			micro_delay(20000);

			if ((result = usb_hub_port_get_status(device, port))
					!= usb_result_ok) {

				kprint("HUB: Failed to get status (4) for port");
				kprint_hexnl(port + 1);
				return result;
			}
			timeout++;
		} while(!port_status->change.reset_changed
			&& !port_status->status.enabled && timeout < 10);

		if (timeout == 10)
			continue;

		if (port_status->change.connected_changed
		    || !port_status->status.connected)
			return usb_error_device;

		if (port_status->status.enabled)
			break;
	}

	if (retry == 3) {
		kprint("HUB: Cannot enable port ");
		kprint_hex(port + 1);
		kprint(" Please verify the hardware is working\n");

		return usb_error_device;
	}

	if ((result = usb_hub_change_port_feature(device,
						  usb_hub_port_feature_reset_change,
						  port, 0))
			!= usb_result_ok) {
		kprint("HUB: Failed to clear reset on port ");
		kprint_hexnl(port + 1);
	}

	return usb_result_ok;
}

static usb_result_t usb_hub_port_connection_changed(struct usb_device *device,
						    u8 port)
{
	usb_result_t result;
	struct usb_hub_device *data;
	struct usb_hub_port_full_status *port_status;

	data = (struct usb_hub_device*)device->driver_data;
	port_status = &data->port_status[port];

	if ((result = usb_hub_port_get_status(device, port))
			!= usb_result_ok) {
		kprint("HUB: Failed to get status (2) for port ");
		kprint_hexnl(port + 1);

		return result;
	}

	if ((result = usb_hub_change_port_feature(device,
						  usb_hub_port_feature_connection_change,
						  port, 0))
			!= usb_result_ok) {
		kprint("HUB: Failed to clear change on port ");
		kprint_hexnl(port + 1);
	}

	if ((!port_status->status.connected && !port_status->status.enabled)
	    || data->children[port] != 0) {
		kprint("HUB: Disconnected port ");
		kprint_hexnl(port + 1);
		usb_deallocate_device(data->children[port]);
		data->children[port] = 0;
		if (!port_status->status.connected)
			return usb_result_ok;
	}

	if ((result = usb_hub_port_reset(device, port)) != usb_result_ok) {
		kprint("HUB: Could not reset port for new device ");
		kprint_hexnl(port + 1);
		return result;
	}

	if ((result = usb_allocate_device(&data->children[port]))
			!= usb_result_ok) {
		kprint("HUB: Could not allocate new device entry for port ");
		kprint_hexnl(port + 1);
		return result;
	}

	if ((result = usb_hub_port_get_status(device, port))
			!= usb_result_ok) {
		kprint("HUB: Hub failed to get status (3) for port ");
		kprint_hexnl(port + 1);
		return result;
	}

	if (port_status->status.high_speed_attached) {
		data->children[port]->speed = usb_speed_high;
	} else if (port_status->status.low_speed_attached) {
		data->children[port]->speed = usb_speed_low;
	} else {
		data->children[port]->speed = usb_speed_full;
	}

	data->children[port]->parent = device;
	data->children[port]->port_number = port;

	if ((result = usb_attach_device(data->children[port]))
			!= usb_result_ok) {
		kprint("HUB: Could not connect to new device on port ");
		kprint_hexnl(port + 1);
		usb_deallocate_device(data->children[port]);
		data->children[port] = 0;
		if (usb_hub_change_port_feature(device,
						usb_hub_port_feature_enable,
						port, 0) != usb_result_ok) {
			kprint("HUB: Failed to disable device on port ");
			kprint_hexnl(port + 1);
		}

		return result;
	}

	return usb_result_ok;
}

static void usb_hub_detached(struct usb_device *device)
{
	struct usb_hub_device *data;

	if (device->driver_data) {
		data = (struct usb_hub_device*)device->driver_data;

		for (u32 i = 0; i < data->max_children; ++i) {
			if (data->children[i]
			    && data->children[i]->device_detached) {
				data->children[i]->device_detached(data->children[i]);
			}
		}
	}
}

static void usb_hub_deallocate(struct usb_device *device)
{
	struct usb_hub_device *data;

	if (device->driver_data != 0) {
		data = (struct usb_hub_device*)device->driver_data;

		for (u32 i = 0; i < data->max_children; ++i) {
			if (data->children[i] != 0) {
				usb_deallocate_device(data->children[i]);
				data->children[i] = 0;
			}
		}

		if (data->descriptor != 0)
			kfree((u32)data->descriptor);
		kfree((u32)device->driver_data);
	}

	device->device_deallocate = 0;
	device->device_detached = 0;
	device->device_check_for_change = 0;
	device->device_child_detached = 0;
	device->device_child_reset = 0;
	device->device_check_connection = 0;
}

static void usb_hub_check_for_change(struct usb_device *device)
{
	struct usb_hub_device *data;

	data = (struct usb_hub_device*)device->driver_data;

	for (u32 i = 0; i < data->max_children; ++i) {
		if (usb_hub_check_connection(device, i) != usb_result_ok)
			continue;

		if (data->children[i]
		    && data->children[i]->device_check_for_change)
			data->children[i]->device_check_for_change(data->children[i]);
	}
}

static void usb_hub_child_detached(struct usb_device *device,
				   struct usb_device *child)
{
	struct usb_hub_device *data;

	data = (struct usb_hub_device*)device->driver_data;

	if (child->parent == device /*&& child->port_number >= 0*/
	    && child->port_number < data->max_children
	    && data->children[child->port_number] == child)
		data->children[child->port_number] = 0;
}

static usb_result_t usb_hub_child_reset(struct usb_device *device,
					struct usb_device *child)
{
	struct usb_hub_device *data;

	data = (struct usb_hub_device*)device->driver_data;

	if (child->parent == device /*&& child->port_number >= 0*/
	    && child->port_number < data->max_children
	    && data->children[child->port_number] == child) {
		return usb_hub_port_reset(device, child->port_number);
	} else {
		return usb_error_device;
	}
}

static usb_result_t usb_hub_check_connection_device(struct usb_device *device,
						    struct usb_device *child)
{
	struct usb_hub_device *data;
	usb_result_t result;

	data = (struct usb_hub_device*)device->driver_data;

	if (child->parent == device /*&& child->port_number >= 0*/
	    && child->port_number < data->max_children
	    && data->children[child->port_number] == child) {
		if ((result = usb_hub_check_connection(device,
						       child->port_number))
				!= usb_result_ok) {
			return result;
		}

		return data->children[child->port_number] == child ?
				usb_result_ok : usb_error_disconnect;
	} else {
		return usb_error_argument;
	}
}

usb_result_t usb_hub_check_connection(struct usb_device *device, u8 port)
{
	usb_result_t result;
	struct usb_hub_device *data;
	struct usb_hub_port_full_status *port_status;

	data = (struct usb_hub_device*)device->driver_data;

	if ((result = usb_hub_port_get_status(device, port)) != usb_result_ok) {
		if (result != usb_error_disconnect) {
			kprint("HUB: Failed to get hub port status (1) for port ");
			kprint_hexnl(port + 1);
		}

		return result;
	}

	port_status = &data->port_status[port];

	if (port_status->change.connected_changed) {
		kprint("Connection changed\n");
		usb_hub_port_connection_changed(device, port);
	}

	if (port_status->change.enabled_changed) {
		kprint("Enabled changed\n");
		if (usb_hub_change_port_feature(device,
						usb_hub_port_feature_enable_change,
						port, 0) != usb_result_ok) {
			kprint("HUB: Failed to clear enable change for port ");
			kprint_hexnl(port + 1);
		}

		if (!port_status->status.enabled
		    && port_status->status.connected
		    && data->children[port] != 0) {
			kprint("Port "); kprint_hex(port + 1);
			kprint(" has been disabled, but is connected. This can"
			       " be caused by interference. Reenabling\n");

			usb_hub_port_connection_changed(device, port);
		}
	}

	if (port_status->status.suspended) {
		if (usb_hub_change_port_feature(device,
						usb_hub_port_feature_suspend,
						port, 0) != usb_result_ok) {
			kprint("HUB: Failed to clear suspended port ");
			kprint_hexnl(port + 1);
		}
	}

	if (port_status->change.overcurrent_changed) {
		if (usb_hub_change_port_feature(device,
						usb_hub_port_feature_overcurrent_change,
						port, 0) != usb_result_ok) {
			kprint("HUB: Failed to clear over current port ");
			kprint_hexnl(port + 1);
		}

		usb_hub_power_on(device);
	}

	if (port_status->change.reset_changed) {
		if (usb_hub_change_port_feature(device,
						usb_hub_port_feature_reset_change,
						port, 0) != usb_result_ok) {
			kprint("HUB: Failed to clear reset port ");
			kprint_hexnl(port + 1);
		}
	}

	return usb_result_ok;
}

usb_result_t usb_hub_attach(struct usb_device *device, u32 interface_number)
{
	kprint("Attaching hub driver\n");
	usb_result_t result;
	struct usb_hub_device *data;
	struct usb_hub_descriptor *hub_desc;
	struct usb_hub_full_status *status;

	if (device->interfaces[interface_number].endpoint_count != 1) {
		kprint("HUB: Cannot enumerate hub with multiple endpoints\n");
		return usb_error_incompatible;
	}
	if (device->endpoints[interface_number][0].endpoint_address.direction == usb_direction_out) {
		kprint("HUB: Cannot enumerate hub with only one output endpoint\n");
		return usb_error_incompatible;
	}
	if (device->endpoints[interface_number][0].attributes.type != usb_transfer_interrupt) {
		kprint("HUB: Cannot enumerate hub without interrupt endpoint\n");
		return usb_error_incompatible;
	}

	device->device_deallocate = usb_hub_deallocate;
	device->device_detached = usb_hub_detached;
	device->device_check_for_change = usb_hub_check_for_change;
	device->device_child_detached = usb_hub_child_detached;
	device->device_child_reset = usb_hub_child_reset;
	device->device_check_connection = usb_hub_check_connection_device;

	device->driver_data = (struct usb_driver_data_header*)kmalloc(sizeof(struct usb_hub_device));

	if (device->driver_data == 0) {
		kprint("HUB: Cannot allocate hub data. Out of memory\n");
		return usb_error_memory;
	}

	data = (struct usb_hub_device*)device->driver_data;
	device->driver_data->data_size = sizeof(struct usb_hub_device);
	device->driver_data->device_driver = USB_DEVICE_DRIVER_HUB;

	for (u32 i = 0; i < USB_MAX_CHILDREN_PER_DEVICE; ++i) {
		data->children[i] = 0;
	}

	if ((result = usb_hub_read_descriptor(device)) != usb_result_ok) {
		kprint("HUB: Failed to read hub descriptor\n");
		return result;
	}

	hub_desc = data->descriptor;

	data->max_children = hub_desc->port_count;

	//.....

	if ((result = usb_hub_get_status(device)) != usb_result_ok) {
		kprint("Failed to get hub status\n");
		return result;
	}

	status = &data->status;

	if ((result = usb_hub_power_on(device)) != usb_result_ok) {
		kprint("HUB: Hub failed to power on\n");
		usb_hub_deallocate(device);
		return result;
	}

	if ((result = usb_hub_get_status(device)) != usb_result_ok) {
		kprint("Failed to get hub status\n");
		usb_hub_deallocate(device);
		return result;
	}

	kprint_hexnl(status->status.local_power);
	kprint_hexnl(status->status.overcurrent);

	for (u8 port = 0; port < data->max_children; ++port) {
		kprint("Checking port connection\n");
		usb_hub_check_connection(device, port);
	}

	return usb_result_ok;
}
