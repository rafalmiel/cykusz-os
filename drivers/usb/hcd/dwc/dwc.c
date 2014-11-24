#include <core/io.h>
#include <core/kheap.h>

#include <drivers/usb/hcd/dwc/core.h>
#include <drivers/usb/hcd/dwc/dwc.h>
#include <drivers/usb/descriptors.h>
#include <drivers/usb/pipe.h>
#include <drivers/usb/device.h>
#include <drivers/usb/devicerequest.h>

#define USB_REQUEST_TIMEOUT 5000

dwc_otg_core_regs_t *dwc_core = 0;
dwc_otg_host_regs_t *dwc_host = 0;
dwc_otg_power_reg_t *dwc_power = 0;

extern u32 usb_root_hub_dev_number;

u8 *data_buffer;

void micro_delay(u32 delay)
{
	volatile u32* time_stamp = (u32*)0xD0003004;

	u32 stop = *time_stamp + delay;

	while (*time_stamp < stop)
		__asm__ volatile ("nop");
}

static void usb_on()
{
	volatile u32* mailbox;
	u32 result;

	mailbox = (u32*)(0xD0000000 + 0x0000B880);
	while (mailbox[6] & 0x80000000);
	mailbox[8] = 0x80;
	do {
		while (mailbox[6] & 0x40000000);
	} while (((result = mailbox[0]) & 0xf) != 0);
	(result == 0x80) ? kprint("USB OK\n") : kprint("USB ERROR\n");
}

void hcd_transmit_channel(u8 channel, void *buffer)
{
	dwc_otg_host_hcchar_reg_t ch;
	dwc_host->channels[channel].hcsplt.data.complete_split = 0;

	if (((u32)buffer & 3) != 0)
		kprint("warning: buffer not DWORD aligned\n");

	dwc_host->channels[channel].hcdma = buffer;

	ch.raw = dwc_host->channels[channel].hcchar.raw;

	ch.data.packets_per_frame = 1;
	ch.data.enable = 1;
	ch.data.disable = 0;

	dwc_host->channels[channel].hcchar.raw = ch.raw;
}

usb_result_t hcd_prepare_channel(struct usb_device *device, u8 channel,
				 u32 length, dwc_otg_packet_id_t packet_id,
				 struct usb_pipe_address *pipe)
{
	dwc_otg_host_hcchar_reg_t ch;
	dwc_otg_host_hcsplt_reg_t spl;
	dwc_otg_host_hctsiz_reg_t tsiz;

	if (channel > dwc_core->ghwcfg2.data.host_channels_count) {
		kprint("Channel ");
		kprint_hexnl(channel);
		kprint(" is not available on this host\n");
		return usb_error_argument;
	}

	dwc_host->channels[channel].hcint.raw = 0xffffffff;

	ch.raw = 0;
	ch.data.device_address = pipe->device;
	ch.data.endpoint_number = pipe->endpoint;
	ch.data.endpoint_direction = pipe->direction;
	ch.data.low_speed = (pipe->speed == usb_speed_low) ? 1 : 0;
	ch.data.type = pipe->type;
	ch.data.maximum_packet_size = usb_size_to_number(pipe->max_size);
	ch.data.enable = 0;
	ch.data.disable = 0;

	dwc_host->channels[channel].hcchar.raw = ch.raw;

	spl.raw = 0;
	if (pipe->speed != usb_speed_high) {
		spl.data.split_enable = 1;
		spl.data.hub_address = device->parent->number;
		spl.data.port_address = device->port_number;
	}

	dwc_host->channels[channel].hcsplt.raw = spl.raw;

	tsiz.raw = 0;
	tsiz.data.transfer_size = length;

	if (pipe->speed == usb_speed_low) {
		tsiz.data.packet_count = (length + 7) / 8;
	} else {
		tsiz.data.packet_count = (length
					  + ch.data.maximum_packet_size - 1)
				/ ch.data.maximum_packet_size;
	}

	if (tsiz.data.packet_count == 0) {
		tsiz.data.packet_count = 1;
	}

	tsiz.data.packet_id = packet_id;

	dwc_host->channels[channel].hctsiz.raw = tsiz.raw;

	return usb_result_ok;
}

usb_result_t hcd_channel_interrupt_to_error(struct usb_device *device,
					    dwc_otg_host_int_reg_t ints,
					    u32 is_complete)
{
	usb_result_t result = usb_result_ok;

	if (ints.data.abh_error) {
		device->error = usb_transfer_error_abh;
		kprint("AHB error in transfer\n");
		return usb_error_device;
	}
	if (ints.data.stall) {
		device->error = usb_transfer_error_stall;
		kprint("Stall error in transfer\n");
		return usb_error_device;
	}
	if (ints.data.negative_acknowledgement) {
		device->error = usb_transfer_error_no_acknowledge;
		kprint("Transfer was not acknowledged\n");
		result = usb_error_timeout;
	}
	if (ints.data.not_yet) {
		device->error = usb_transfer_error_notyet;
		kprint("Not yet error in transfer\n");
		return usb_error_device;
	}
	if (ints.data.babble_error) {
		device->error = usb_transfer_error_babble;
		kprint("Babble error in transfer");
		return usb_error_device;
	}
	if (ints.data.frame_overrun) {
		device->error = usb_transfer_error_buffer;
		kprint("Frame overrun in transfer\n");
		return usb_error_device;
	}
	if (ints.data.data_toggle_error) {
		device->error = usb_transfer_error_bit;
		kprint("Data toggle error in transfer\n");
		return usb_error_device;
	}
	if (ints.data.transaction_error) {
		device->error = usb_transfer_error_connection;
		kprint("Transaction error in transfer\n");
		return usb_error_device;
	}
	if (!ints.data.transfer_complete && is_complete) {
		kprint("Transfer did not complete\n");
		result = usb_error_timeout;
	}
	return result;
}

usb_result_t hcd_channel_send_wait_one(struct usb_device *device,
				       struct usb_pipe_address *pipe,
				       u8 channel, void *buffer, u32 buffer_len,
				       u32 buffer_offset,
				       struct usb_device_request *request)
{
	(void)pipe;
	(void)buffer_len;
	(void)request;

	usb_result_t result;
	u32 timeout, tries, global_tries, actual_tries;

	for (global_tries = 0, actual_tries = 0;
	     global_tries < 3 && actual_tries < 10;
	     ++global_tries, ++actual_tries) {
		dwc_host->channels[channel].hcint.raw = 0xffffffff;

		hcd_transmit_channel(channel, (u8*)buffer + buffer_offset);

		timeout = 0;

		do {
			if (timeout++ == USB_REQUEST_TIMEOUT) {
				device->error = usb_transfer_error_connection;
				return usb_error_timeout;
			}
			if (!dwc_host->channels[channel].hcint.data.halt)
				micro_delay(10);
			else
				break;
		} while(1);

		if (dwc_host->channels[channel].hcsplt.data.split_enable) {
			if (dwc_host->channels[channel].hcint.data.acknowledgement) {
				for (tries = 0; tries < 3; ++tries) {
					dwc_host->channels[channel].hcint.raw = 0xffffffff;

					dwc_host->channels[channel].hcsplt.data.complete_split = 1;

					dwc_host->channels[channel].hcchar.data.enable = 1;
					dwc_host->channels[channel].hcchar.data.disable = 0;

					timeout = 0;
					do {
						if (timeout++ == USB_REQUEST_TIMEOUT) {
							device->error = usb_transfer_error_connection;
							return usb_error_timeout;
						}

						if (!dwc_host->channels[channel].hcint.data.halt)
							micro_delay(100);
						else
							break;
					} while (1);
					if (!dwc_host->channels[channel].hcint.data.not_yet)
						break;
				}

				if (tries == 3) {
					micro_delay(25000);
					continue;
				} else if (dwc_host->channels[channel].hcint.data.negative_acknowledgement) {
					global_tries--;
					micro_delay(25000);
					continue;
				} else if (dwc_host->channels[channel].hcint.data.transaction_error) {
					micro_delay(25000);
					continue;
				}

				if ((result = hcd_channel_interrupt_to_error(device,
									     dwc_host->channels[channel].hcint,
									     0))
						!= usb_result_ok) {
					kprint("Request split completion failed\n");
					return result;
				}
			} else if (dwc_host->channels[channel].hcint.data.negative_acknowledgement) {
				global_tries--;
				micro_delay(25000);
				continue;
			} else if (dwc_host->channels[channel].hcint.data.transaction_error) {
				micro_delay(25000);
				continue;
			}
		} else {
			if ((result = hcd_channel_interrupt_to_error(device,
								     dwc_host->channels[channel].hcint,
								     !dwc_host->channels[channel].hcsplt.data.split_enable))
					!= usb_result_ok) {
				kprint("Request failed\n");
				return usb_error_retry;
			}
		}

		break;
	}

	if (global_tries == 3 || actual_tries == 10) {
		kprint("Request failed 3 times\n");
		if ((result = hcd_channel_interrupt_to_error(device,
							     dwc_host->channels[channel].hcint,
							     !dwc_host->channels[channel].hcsplt.data.split_enable))
				!= usb_result_ok) {
			kprint("Request failed\n");
			return result;
		}
		device->error = usb_transfer_error_connection;
		return usb_error_timeout;
	}

	return usb_result_ok;
}

usb_result_t hcd_channel_send_wait(struct usb_device *device,
				   struct usb_pipe_address *pipe, u8 channel,
				   void *buffer, u32 buffer_length,
				   struct usb_device_request *request,
				   dwc_otg_packet_id_t packet_id)
{
	usb_result_t result;
	u32 packets, transfer, tries;

	tries = 0;
retry:
	if (tries++ == 3) {
		kprint("Failed to send after 3 attempts\n");
		return usb_error_timeout;
	}

	if ((result = hcd_prepare_channel(device, channel, buffer_length,
					  packet_id, pipe)) != usb_result_ok) {
		device->error = usb_transfer_error_connection;
		kprint("Cound not prepare data channel\n");
		return result;
	}

	transfer = 0;

	do {
		packets = dwc_host->channels[channel].hctsiz.data.packet_count;

		if ((result = hcd_channel_send_wait_one(device, pipe, channel,
							buffer, buffer_length,
							transfer,
							request)) != usb_result_ok) {
			if (result == usb_error_retry)
				goto retry;

			return result;
		}

		transfer = buffer_length - dwc_host->channels[channel].hctsiz.data.transfer_size;

		if (packets == dwc_host->channels[channel].hctsiz.data.packet_count)
			break;
	} while (dwc_host->channels[channel].hctsiz.data.packet_count > 0);

	if (packets == dwc_host->channels[channel].hctsiz.data.packet_count) {
		device->error = usb_transfer_error_connection;
		kprint("Transfer got stuck\n");
		return usb_error_device;
	}

	if (tries > 1) {
		kprint("Transfer succeded on attempt: ");
		kprint_hexnl(tries);
	}

	return usb_result_ok;
}

usb_result_t hcd_submit_control_message(struct usb_device *device,
					struct usb_pipe_address pipe,
					void *buffer, u32 buffer_length,
					struct usb_device_request *request)
{
	usb_result_t result;
	struct usb_pipe_address temp_pipe;

	if (pipe.device == usb_root_hub_dev_number) {
		return usb_process_root_hub_message(device, pipe, buffer,
						    buffer_length, request);
	}

	device->error = usb_transfer_error_processing;
	device->last_transfer = 0;

	temp_pipe.speed = pipe.speed;
	temp_pipe.device = pipe.device;
	temp_pipe.endpoint = pipe.endpoint;
	temp_pipe.max_size = pipe.max_size;
	temp_pipe.type = usb_transfer_control;
	temp_pipe.direction = usb_direction_out;

	if ((result = hcd_channel_send_wait(device, &temp_pipe, 0, data_buffer,
					    buffer_length, request,
					    dwc_otg_packet_id_setup))
			!= usb_result_ok) {
		kprint("Could not send setup\n");
		return usb_result_ok;
	}

	if (buffer != 0) {
		if (pipe.direction == usb_direction_out) {
			memcpy(data_buffer, buffer, buffer_length);
		}

		temp_pipe.speed = pipe.speed;
		temp_pipe.device = pipe.device;
		temp_pipe.endpoint = pipe.endpoint;
		temp_pipe.max_size = pipe.max_size;
		temp_pipe.type = usb_transfer_control;
		temp_pipe.direction = pipe.direction;

		if ((result = hcd_channel_send_wait(device, &temp_pipe,
						    0, data_buffer,
						    buffer_length,
						    request,
						    dwc_otg_packet_id_data1))
				!= usb_result_ok) {
			kprint("Cound not send data\n");
			return usb_result_ok;
		}

		if (pipe.direction == usb_direction_in) {
			if (dwc_host->channels[0].hctsiz.data.transfer_size
					< buffer_length) {
				device->last_transfer = buffer_length -
						dwc_host->channels[0].hctsiz.data.transfer_size;
			} else {
				kprint("Weird transfer\n");
				device->last_transfer = buffer_length;
			}
			memcpy(buffer, data_buffer, device->last_transfer);
		} else {
			device->last_transfer = buffer_length;
		}
	}

	temp_pipe.speed = pipe.speed;
	temp_pipe.device = pipe.device;
	temp_pipe.endpoint = pipe.endpoint;
	temp_pipe.max_size = pipe.max_size;
	temp_pipe.type = usb_transfer_control;
	temp_pipe.direction = ((buffer_length == 0)
			       || pipe.direction == usb_direction_out)
			? usb_direction_in
			: usb_direction_out;

	if ((result = hcd_channel_send_wait(device, &temp_pipe, 0,
					    data_buffer, 0, request,
					    dwc_otg_packet_id_data1))
			!= usb_result_ok) {
		kprint("Could not send STATUS\n");
		return usb_result_ok;
	}

	if (dwc_host->channels[0].hctsiz.data.transfer_size != 0) {
		kprint("Warning non zero status transfer!\n");
	}

	device->error = usb_transfer_no_error;

	return usb_result_ok;
}

int hcd_reset()
{
	u32 count = 0;

	do {
		if (count++ >= 0x100000) {
			kprint("HCD: Device hang 1!\n");
			return -1;
		}
	} while (dwc_core->grstctl.data.abh_master_idle == 0);

	dwc_core->grstctl.data.core_soft = 1;

	count = 0;

	do {
		if (count++ >= 0x10000000) {
			kprint("HCD: Device hang 2!\n");
			return -1;
		}
	} while (dwc_core->grstctl.data.core_soft == 1
		 || dwc_core->grstctl.data.abh_master_idle == 0);

	return 0;
}

int hcd_transmit_fifo_flush()
{
	dwc_core->grstctl.data.transmit_fifo_flush_number = transmit_fifo_flush_all;
	dwc_core->grstctl.data.transmit_fifo_flush = 1;

	do {

	} while (dwc_core->grstctl.data.transmit_fifo_flush == 1);

	return 0;
}

int hcd_receive_fifo_flush()
{
	dwc_core->grstctl.data.receive_fifo_flush = 1;

	do {

	} while (dwc_core->grstctl.data.receive_fifo_flush == 1);

	return 0;
}

static void dwc_core_start(void)
{
	data_buffer = (u8*)kmalloc_a(1024);

	dwc_core->gintmsk.raw = 0;
	dwc_core->gahbcfg.data.glob_int_mask = 0;

	usb_on();

	dwc_core->gusbcfg.data.ulpi_drive_external_vbus = 0;
	dwc_core->gusbcfg.data.ts_dline_pulse_enable = 0;

	if (hcd_reset() == 0) {
		kprint("HCD reset success!\n");
	} else {
		kprint("HCD reset failed!\n");
		return;
	}

	if (dwc_core->ghwcfg2.data.high_speed_phy == high_speed_ulpi
	    && dwc_core->ghwcfg2.data.full_speed_phy == full_speed_dedicated) {
		kprint("Ulpi FSLS configuration: enabled\n");
		dwc_core->gusbcfg.data.ulpi_fsls = 1;
		dwc_core->gusbcfg.data.ulpi_clk_sus_m = 1;
	} else {
		kprint("Ulpi FSLS configuration: disabled\n");
		dwc_core->gusbcfg.data.ulpi_fsls = 0;
		dwc_core->gusbcfg.data.ulpi_clk_sus_m = 0;
	}

	dwc_core->gahbcfg.data.dma_enable = 1;

	kprint("Dma remainder mode: ");
	kprint_hexnl(dwc_core->gahbcfg.data.dma_remainder_mode);
	dwc_core->gahbcfg.data.dma_remainder_mode = dma_remainder_mode_incremental;
	kprint("Operating mode: ");
	kprint_hexnl(dwc_core->ghwcfg2.data.otgmode);
	dwc_core->gusbcfg.data.hnp_capable = 1;
	dwc_core->gusbcfg.data.srp_capable = 1;

	kprint("HCD Core started!\n");
}

static void dwc_host_start(void)
{
	dwc_power->raw = 0;

	dwc_host->hcfg.data.clock_rate = clock_rate_30_60MHz;
	dwc_host->hcfg.data.fsls_only = 1;

	dwc_core->grxfsiz = 0x1000;

	dwc_core->gnptxfsiz.data.depth = 0x1000;
	dwc_core->gnptxfsiz.data.start_address = 0x1000;

	dwc_core->hptxfsiz.data.depth = 0x1000;
	dwc_core->hptxfsiz.data.start_address = 0x2000;

	dwc_core->gotgctl.data.host_set_hnp_enable = 1;

	hcd_transmit_fifo_flush();
	hcd_receive_fifo_flush();

	kprint("Fifos flushed\n");

	if (!dwc_host->hcfg.data.enable_dma_descriptor) {
		for (u32 channel = 0;
		     channel < dwc_core->ghwcfg2.data.host_channels_count;
		     ++channel) {
			dwc_host->channels[channel].hcchar.data.enable = 0;
			dwc_host->channels[channel].hcchar.data.disable = 1;
			dwc_host->channels[channel].hcchar.data.endpoint_direction =
					usb_direction_in;
		}

		for (u32 channel = 0;
		     channel < dwc_core->ghwcfg2.data.host_channels_count;
		     ++channel) {
			dwc_host->channels[channel].hcchar.data.enable = 1;
			dwc_host->channels[channel].hcchar.data.disable = 1;
			dwc_host->channels[channel].hcchar.data.endpoint_direction =
					usb_direction_in;

			u32 timeout = 0;

			do {
				if (timeout++ > 0x400000) {
					kprint("Unable to clear halt on channel ");
					kprint_hexnl(channel);
					break;
				}
			} while (dwc_host->channels[channel].hcchar.data.enable);
		}

//		for (u32 channel = 0;
//		     channel < dwc_core->ghwcfg2.data.host_channels_count;
//		     ++channel) {
//			kprint("Channel ");
//			kprint_hex(channel);
//			kprint(" status: ");
//			kprint_hex(dwc_host->channels[channel].hcchar.data.enable);
//			kprint(" ");
//			kprint_hexnl(dwc_host->channels[channel].hcchar.data.disable);
//		}
	}

	if (!dwc_host->hprt.data.power) {
		kprint("Powering up port\n");
		dwc_host->hprt.data.power = 1;
	}

	dwc_host->hprt.data.reset = 1;

	micro_delay(50000);

	dwc_host->hprt.data.reset = 0;

	kprint("HCD succesfully started: ");
	kprint_hexnl(dwc_host->hprt.data.power);
}

static void dwc_start(void)
{
	dwc_core_start();
	dwc_host_start();
}

void init_dwc(u32 reg_base)
{
	dwc_core = (dwc_otg_core_regs_t *)reg_base;
	dwc_host = (dwc_otg_host_regs_t *)(reg_base + 0x400);
	dwc_power = (dwc_otg_power_reg_t *)(reg_base + 0xE00);

	dwc_start();
}
