#include <core/io.h>

#include <drivers/hcd/dwc/core.h>
#include <drivers/hcd/dwc/dwc.h>

dwc_otg_core_regs_t *core = 0;
dwc_otg_host_regs_t *host = 0;
dwc_otg_power_reg_t *power = 0;

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

int hcd_reset()
{
	u32 count = 0;

	do {
		if (count++ >= 0x100000) {
			kprint("HCD: Device hang 1!\n");
			return -1;
		}
	} while (core->grstctl.data.abh_master_idle == 0);

	core->grstctl.data.core_soft = 1;

	count = 0;

	do {
		if (count++ >= 0x10000000) {
			kprint("HCD: Device hang 2!\n");
			return -1;
		}
	} while (core->grstctl.data.core_soft == 1
		 || core->grstctl.data.abh_master_idle == 0);

	return 0;
}

int hcd_transmit_fifo_flush()
{
	core->grstctl.data.transmit_fifo_flush_number = transmit_fifo_flush_all;
	core->grstctl.data.transmit_fifo_flush = 1;

	do {

	} while (core->grstctl.data.transmit_fifo_flush == 1);

	return 0;
}

int hcd_receive_fifo_flush()
{
	core->grstctl.data.receive_fifo_flush = 1;

	do {

	} while (core->grstctl.data.receive_fifo_flush == 1);

	return 0;
}

static void dwc_core_start(void)
{
	core->gusbcfg.data.ulpi_drive_external_vbus = 0;
	core->gusbcfg.data.ts_dline_pulse_enable = 0;

	if (hcd_reset() == 0) {
		kprint("HCD reset success!\n");
	} else {
		kprint("HCD reset failed!\n");
		return;
	}

	if (core->ghwcfg2.data.high_speed_phy == high_speed_ulpi
	    && core->ghwcfg2.data.full_speed_phy == full_speed_dedicated) {
		kprint("Ulpi FSLS configuration: enabled\n");
		core->gusbcfg.data.ulpi_fsls = 1;
		core->gusbcfg.data.ulpi_clk_sus_m = 1;
	} else {
		kprint("Ulpi FSLS configuration: disabled\n");
		core->gusbcfg.data.ulpi_fsls = 0;
		core->gusbcfg.data.ulpi_clk_sus_m = 0;
	}

	core->gahbcfg.data.dma_enable = 1;

	kprint("Dma remainder mode: ");
	kprint_hexnl(core->gahbcfg.data.dma_remainder_mode);
	core->gahbcfg.data.dma_remainder_mode = dma_remainder_mode_incremental;
	kprint("Operating mode: ");
	kprint_hexnl(core->ghwcfg2.data.otgmode);
	core->gusbcfg.data.hnp_capable = 1;
	core->gusbcfg.data.srp_capable = 1;

	kprint("HCD Core started!\n");
}

static void dwc_host_start(void)
{
	power->raw = 0;

	host->hcfg.data.clock_rate = clock_rate_30_60MHz;
	host->hcfg.data.fsls_only = 1;

	core->grxfsiz = 0x1000;

	core->gnptxfsiz.data.depth = 0x1000;
	core->gnptxfsiz.data.start_address = 0x1000;

	core->hptxfsiz.data.depth = 0x1000;
	core->hptxfsiz.data.start_address = 0x2000;

	core->gotgctl.data.host_set_hnp_enable = 1;

	hcd_transmit_fifo_flush();
	hcd_receive_fifo_flush();

	kprint("Fifos flushed\n");



	if (!host->hcfg.data.enable_dma_descriptor) {
		for (u32 channel = 0;
		     channel < core->ghwcfg2.data.host_channels_count;
		     ++channel) {
			host->channels[channel].hcchar.data.enable = 0;
			host->channels[channel].hcchar.data.disable = 1;
			host->channels[channel].hcchar.data.endpoint_direction =
					usb_direction_in;
		}

		for (u32 channel = 0;
		     channel < core->ghwcfg2.data.host_channels_count;
		     ++channel) {
			host->channels[channel].hcchar.data.enable = 1;
			host->channels[channel].hcchar.data.disable = 1;
			host->channels[channel].hcchar.data.endpoint_direction =
					usb_direction_in;

			u32 timeout = 0;

			do {
				if (timeout++ > 0x400000) {
					kprint("Unable to clear halt on channel ");
					kprint_hexnl(channel);
					break;
				}
			} while (host->channels[channel].hcchar.data.enable);
		}

//		for (u32 channel = 0;
//		     channel < core->ghwcfg2.data.host_channels_count;
//		     ++channel) {
//			kprint_hex(host->channels[channel].hcchar.data.enable);
//			kprint(" ");
//			kprint_hexnl(host->channels[channel].hcchar.data.disable);
//		}
	}

	if (!host->hprt.data.power) {
		kprint("Powering up port\n");
		host->hprt.data.power = 1;
	}

	host->hprt.data.reset = 1;

	micro_delay(50000);

	host->hprt.data.reset = 0;

	kprint("HCD succesfully started: ");
	kprint_hexnl(host->hprt.data.power);
}

static void dwc_start(void)
{
	dwc_core_start();
	dwc_host_start();
}

void init_dwc(u32 reg_base)
{
	core = (dwc_otg_core_regs_t *)reg_base;
	host = (dwc_otg_host_regs_t *)(reg_base + 0x400);
	power = (dwc_otg_power_reg_t *)(reg_base + 0xE00);

	core->gintmsk.raw = 0;
	core->gahbcfg.data.glob_int_mask = 0;

	usb_on();

	dwc_start();
}
