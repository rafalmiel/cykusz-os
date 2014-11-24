
#include <core/common.h>
#include <drivers/usb/descriptors.h>
#include <drivers/usb/pipe.h>
#include <drivers/usb/devicerequest.h>
#include <drivers/usb/device/hub.h>

#include <drivers/usb/hcd/dwc/dwc.h>

struct usb_device_descriptor dev_descriptor = {
	.descriptor_length = 0x12,
	.descriptor_type = usb_descriptor_type_device,
	.usb_version = 0x0200,
	.dev_class = usb_device_class_hub,
	.subclass = 0,
	.protocol = 0,
	.max_packet_size0 = 8,
	.vendor_id = 0,
	.product_id = 0,
	.version = 0x100,
	.manufacturer = 0,
	.product = 1,
	.serial_number = 0,
	.configuration_count = 1,
};

struct {
	struct usb_configuration_descriptor configuration;
	struct usb_interface_descriptor interface;
	struct usb_endpoint_descriptor endpoint;
} __attribute__((packed)) config_descriptor = {
	.configuration = {
		.descriptor_length = 9,
		.descriptor_type = usb_descriptor_type_configuration,
		.total_length = 0x19,
		.interface_count = 1,
		.configuration_value = 1,
		.string_index = 0,
		.attributes = {
			.remote_wakeup = 0,
			.self_powered = 1,
			.__reserved7 = 1,
		},
		.maximum_power = 0,
	},
	.interface = {
		.descriptor_length = 9,
		.descriptor_type = usb_descriptor_type_interface,
		.number = 0,
		.alternate_setting = 0,
		.endpoint_count = 1,
		.int_class = usb_interface_class_hub,
		.subclass = 0,
		.protocol = 0,
		.string_index = 0,
	},
	.endpoint = {
		.descriptor_length = 9,
		.descriptor_type = usb_descriptor_type_endpoint,
		.endpoint_address = {
			.number = 1,
			.direction = usb_direction_in,
		},
		.attributes = {
			.type = usb_transfer_interrupt,
		},
		.packet = {
			.max_size= 8,
		},
		.interval = 0xff,
	}
};

struct usb_string_descriptor string0 =
{
	.descriptor_length = 4,
	.descriptor_type = usb_descriptor_type_string,
	.data = {
		0x0409,
	},
};

struct usb_string_descriptor string1 =
{
	.descriptor_length = sizeof(L"USB 2.0 Root Hub") + 2,
	.descriptor_type = usb_descriptor_type_string,
	.data = L"USB 2.0 Root Hub",
};

struct usb_hub_descriptor hub_descriptor = {
	.descriptor_length = 0x9,
	.descriptor_type = usb_descriptor_type_hub,
	.port_count = 1,
	.attributes = {
		.power_switch_mode = usb_hub_port_control_global,
		.compound = 0,
		.overcurrent_protection = usb_hub_port_control_global,
		.think_time = 0,
		.indicators = 0,
	},
	.power_good_delay = 0,
	.maximum_hub_power = 0,
	.data = { 0x01, 0xff, },
};

u32 usb_root_hub_dev_number = 0;

usb_result_t usb_process_root_hub_message(struct usb_device *device,
					  struct usb_pipe_address pipe,
					  void *buffer, u32 buffer_length,
					  struct usb_device_request *request)
{
	u32 reply_length;
	usb_result_t result;

	result = usb_result_ok;
	device->error = usb_transfer_error_processing;

	if (pipe.type == usb_transfer_interrupt) {
		kprint("RootHub does not support IRQ pipes\n");
		device->error = usb_transfer_error_stall;
		return usb_result_ok;
	}

	reply_length = 0;

	switch (request->request) {
	case usb_device_request_get_status:
		switch (request->type) {
		case 0x80:
			*(u16*)buffer = 1;
			reply_length = 2;
			break;
		case 0x81: //interface
		case 0x82: //endpoint
			*(u16*)buffer = 0;
			reply_length = 2;
			break;
		case 0xa0: //class
			*(u32*)buffer = 0;
			reply_length = 4;
			break;
		case 0xa3:
		{
			*(u32*)buffer = 0;
			struct usb_hub_port_full_status *s =
					(struct usb_hub_port_full_status*)buffer;

			s->status.connected = dwc_host->hprt.data.connect;
			s->status.enabled = dwc_host->hprt.data.enable;
			s->status.suspended = dwc_host->hprt.data.suspend;
			s->status.overcurrent = dwc_host->hprt.data.over_current;
			s->status.reset = dwc_host->hprt.data.reset;
			s->status.power = dwc_host->hprt.data.power;

			if (dwc_host->hprt.data.speed == usb_speed_high)
				s->status.high_speed_attached = 1;
			else if (dwc_host->hprt.data.speed == usb_speed_low)
				s->status.low_speed_attached = 1;
			s->status.test_mode = dwc_host->hprt.data.test_control;

			s->change.connected_changed = dwc_host->hprt.data.connect_detected;
			s->change.enabled_changed = dwc_host->hprt.data.enable_changed;
			s->change.overcurrent_changed = dwc_host->hprt.data.over_current_changed;
			s->change.reset_changed = 1;

			reply_length = 4;
		}
			break;
		default:
			device->error = usb_transfer_error_stall;
			break;
		}

		break;

	case usb_device_request_clear_feature:
		reply_length = 0;
		switch (request->type) {
		case 0x2:
		case 0x20:
			break;
		case 0x23:
			switch ((enum usb_hub_port_feature)request->value) {

			case usb_hub_port_feature_enable:
				dwc_host->hprt.data.enable = 1;
				break;

			case usb_hub_port_feature_suspend:
				dwc_power->raw = 0;

				micro_delay(5000);

				dwc_host->hprt.data.resume = 1;

				micro_delay(100000);

				dwc_host->hprt.data.resume = 0;
				dwc_host->hprt.data.suspend = 0;
				break;

			case usb_hub_port_feature_power:
				dwc_host->hprt.data.power = 0;
				break;

			case usb_hub_port_feature_connection_change:
				dwc_host->hprt.data.connect_detected = 1;
				break;

			case usb_hub_port_feature_enable_change:
				dwc_host->hprt.data.enable_changed = 1;
				break;

			case usb_hub_port_feature_overcurrent_change:
				dwc_host->hprt.data.over_current_changed = 1;
				break;
			default:
				break;
			}

			break;
		default:
			result = usb_error_argument;
			break;
		}

		break;

	case usb_device_request_set_feature:
		switch ((enum usb_hub_port_feature)request->value) {
		case usb_hub_port_feature_reset:
			dwc_power->data.enable_sleep_clock_gating = 0;
			dwc_power->data.stop_pclock = 0;

			dwc_power->raw = 0;

			dwc_host->hprt.data.suspend = 0;
			dwc_host->hprt.data.reset = 1;
			dwc_host->hprt.data.power = 1;

			micro_delay(60000);

			dwc_host->hprt.data.reset = 0;
			break;
		case usb_hub_port_feature_power:
			dwc_host->hprt.data.power = 1;
			break;
		default:
			break;
		}

		break;

	case usb_device_request_set_address:
		reply_length = 0;
		usb_root_hub_dev_number = request->value;
		break;

	case usb_device_request_get_descriptor:
		switch (request->type) {
		case 0x80:
			switch ((enum usb_descriptor_type)((request->value >> 8) & 0xff)) {
			case usb_descriptor_type_device:
				memcpy(buffer, &dev_descriptor,
				       reply_length = USB_MIN(sizeof(dev_descriptor),
							      buffer_length,
							      u32));
				break;
			case usb_descriptor_type_configuration:
				kprint("get configuration\n");
				memcpy(buffer, &config_descriptor,
				       reply_length = USB_MIN(sizeof(config_descriptor),
							      buffer_length,
							      u32));
				kprint_hexnl(buffer_length);
				kprint_hexnl(sizeof(config_descriptor));
				break;
			case usb_descriptor_type_string:
				switch (request->value & 0xff) {
				case 0x0:
					memcpy(buffer, &string0,
					       reply_length = USB_MIN(string0.descriptor_length,
								      buffer_length,
								      u32));
					break;
				case 0x1:
					memcpy(buffer, &string1,
					       reply_length = USB_MIN(string1.descriptor_length,
								      buffer_length,
								      u32));
					break;
				default:
					reply_length = 0;
					break;
				}

				break;
			default:
				result = usb_error_argument;
			}

			break;

		case 0xa0:
			memcpy(buffer, &hub_descriptor,
			       reply_length = USB_MIN(hub_descriptor.descriptor_length,
						      buffer_length,
						      u32));
			break;

		default:
			result = usb_error_argument;
			break;
		}

		break;

	case usb_device_request_get_configuration:
		*(u8*)buffer = 0x1;
		break;

	case usb_device_request_set_configuration:
		reply_length = 0;
		break;

	default:
		result = usb_error_argument;
		break;
	}

	if (result == usb_error_argument) {
		device->error |= usb_transfer_error_stall;
	}

	kprint("Reply length: ");
	kprint_hexnl(reply_length);

	device->error &= ~usb_transfer_error_processing;
	device->last_transfer = reply_length;

	return usb_result_ok;
}
