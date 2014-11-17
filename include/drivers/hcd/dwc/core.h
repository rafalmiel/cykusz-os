#ifndef USB_HC_DWC_CORE_H
#define USB_HC_DWC_CORE_H

#include <core/common.h>

#define DWC_OTG_CHANNEL_COUNT 16

typedef union dwc_otg_core_ghwcfg1_reg {

	u32 raw;

	struct {
		volatile const enum endpoint_dir {
			endpoint_dir_bidir = 0,
			endpoint_dir_in = 1,
			endpoint_dir_out = 2,
			endpoint_dir_reserved = 3,
		} endpoint_dir0 : 2;
		volatile const enum endpoint_dir endpoint_dir1 : 2;
		volatile const enum endpoint_dir endpoint_dir2 : 2;
		volatile const enum endpoint_dir endpoint_dir3 : 2;
		volatile const enum endpoint_dir endpoint_dir4 : 2;
		volatile const enum endpoint_dir endpoint_dir5 : 2;
		volatile const enum endpoint_dir endpoint_dir6 : 2;
		volatile const enum endpoint_dir endpoint_dir7 : 2;
		volatile const enum endpoint_dir endpoint_dir8 : 2;
		volatile const enum endpoint_dir endpoint_dir9 : 2;
		volatile const enum endpoint_dir endpoint_dir10 : 2;
		volatile const enum endpoint_dir endpoint_dir11 : 2;
		volatile const enum endpoint_dir endpoint_dir12 : 2;
		volatile const enum endpoint_dir endpoint_dir13 : 2;
		volatile const enum endpoint_dir endpoint_dir14 : 2;
		volatile const enum endpoint_dir endpoint_dir15 : 2;
	} data;

} dwc_otg_core_ghwcfg1_reg_t;

typedef union dwc_otg_core_ghwcfg2_reg {

	u32 raw;

	struct {
		volatile const enum {
			otgmode_hnp_srp_cap = 0,
			otgmode_srp_cap = 1,
			otgmode_no_hnp_srp_cap = 2,
			otgmode_srp_cap_dev = 3,
			otgmode_no_otg_dev = 4,
			otgmode_srp_cap_host = 5,
			otgmode_no_otg_host = 6,
		} otgmode : 3;
		volatile u32 otgarch : 2;
		volatile u32 single_point : 1;
		volatile const enum {
			high_speed_not_supported = 0,
			high_speed_ulpi
		} high_speed_phy : 2;
		volatile const enum {
			full_speed_ulpi = 2
		} full_speed_phy : 2;
		volatile const u32 endpoints_count : 4;
		volatile const u32 host_channels_count : 4;
		volatile const u32 periodic_endpoints : 1;
		volatile const u32 dynamic_fifo : 1;
		volatile const u32 multiproc_interrupt : 1;
		volatile const u32 __reserved21 : 1;
		volatile const u32 non_periodic_queue_depth : 2;
		volatile const u32 host_periodic_queue_depth : 2;
		volatile const u32 token_depth : 5;
		volatile const u32 __reserved31 : 1;
	} data;

} dwc_otg_core_ghwcfg2_reg_t;

typedef union dwc_otg_core_ghwcfg3_reg {

	u32 raw;

	struct {
		volatile const u32 transfer_size_width : 4;
		volatile const u32 packet_size_width : 3;
		volatile const u32 otgen : 1;
		volatile const u32 i2cintsel : 1;
		volatile const u32 vendor_control_interface : 1;
		volatile const u32 optional_feature : 1;
		volatile const u32 sync_reset_type : 1;
		volatile const u32 adp_support : 1;
		volatile const u32 otg_enable_hsic : 1;
		volatile const u32 battery_charger_support : 1;
		volatile const u32 lpm_mode : 1;
		volatile const u32 dfifo_depth : 16;
	} data;

} dwc_otg_core_ghwcfg3_reg_t;

typedef union dwc_otg_core_ghwcfg4_reg {

	u32 raw;

	struct {
		volatile const u32 periodic_in_endpoint_count : 4;
		volatile const u32 partial_power_down : 1;
		volatile const u32 min_abh_frequency : 1;
		volatile const u32 hibernation : 1;
		volatile const u32 __reserved7_13 : 7;
		volatile const u32 phy_data_width : 2;
		volatile const u32 mode_control_endpoint_count : 4;
		volatile const u32 iddg_filter_enabled : 1;
		volatile const u32 vbus_valid_filter_enabled : 1;
		volatile const u32 valid_filter_a_enabled : 1;
		volatile const u32 valid_filter_b_enabled : 1;
		volatile const u32 session_end_filter_enabled : 1;
		volatile const u32 dedicated_fifo_mode_enabled : 1;
		volatile const u32 in_endpoints_count : 4;
		volatile const u32 dma_configuration : 1;
		volatile const u32 dma_dynamic_configuration : 1;
	} data;

} dwc_otg_core_ghwcfg4_reg_t;


typedef union dwc_otg_core_gahbcfg_reg {

	u32 raw;

	struct {
		volatile u32 glob_int_mask : 1;
		volatile enum {
			axi_burst_length_4 = 0,
			axi_burst_length_3 = 1,
			axi_burst_length_2 = 2,
			axi_burst_length_1 = 3,
		} axi_burst_length : 2;
		volatile u32 __reserved3 : 1;
		volatile u32 wait_for_axi_writes : 1;
		volatile u32 dma_enable : 1;
		volatile u32 __reserved6 : 1;
		volatile enum empty_lvl {
			empty_lvl_half = 0,
			empty_lvl_full = 1,
		} transfer_empty_lvl : 1;
		volatile enum empty_lvl periodic_transfer_empty_lvl : 1;
		volatile u32 __reserved9_20 : 12;
		volatile u32 remmemsupp : 1;
		volatile u32 notialldmawrit : 1;
		volatile u32 dma_remainder_mode : 1;
		volatile u32 __reserved24_31 : 8;
	} data;

} dwc_otg_core_gahbcfg_reg_t;

typedef union dwc_otg_core_gusbcfg_reg {

	u32 raw;

	struct {
		volatile u32 toutcal : 3;
		volatile const u32 phy_interface : 1;
		volatile const enum {
			umode_ulpi = 0,
			umode_utmi = 1
		} umode : 1;
		volatile const u32 fsintf : 1;
		volatile const u32 physel : 1;
		volatile u32 ddrsel : 1;
		volatile u32 srp_capable : 1;
		volatile u32 hnp_capable : 1;
		volatile u32 usbtrdtim : 4;
		volatile u32 __reserved14 : 1;
		volatile u32 phy_lpm_clk_sel : 1;
		volatile u32 otgutmifssel : 1;
		volatile u32 ulpi_fsls : 1;
		volatile u32 ulpi_auto_res : 1;
		volatile u32 ulpi_clk_sus_m : 1;
		volatile u32 ulpi_drive_external_vbus : 1;
		volatile u32 ulpi_int_vbus_indicator : 1;
		volatile u32 ts_dline_pulse_enable : 1;
		volatile u32 indicator_complement : 1;
		volatile u32 indicator_pass_through : 1;
		volatile u32 ulpi_int_prot_dis : 1;
		volatile u32 ic_usb_capable : 1;
		volatile u32 ic_traffic_pull_remove : 1;
		volatile u32 tx_end_delay : 1;
		volatile u32 force_host_mode : 1;
		volatile u32 force_dev_mode : 1;
		volatile u32 corrupt_ttx_pkr : 1;
	} data;

} dwc_otg_core_gusbcfg_reg_t;

typedef union dwc_otg_host_hcfg_reg {

	u32 raw;

	struct {
		volatile enum {
			clock_rate_30_60MHz = 0,
			clock_rate_48MHz = 1,
			clock_rate_6MHz = 2,
		} clock_rate : 2;
		volatile u32 fsls_only : 1;
		volatile u32 __reserved3_6 : 4;
		volatile u32 en_32khz_susp : 1;
		volatile u32 res_val_period : 8;
		volatile u32 __reserved16_22 : 7;
		volatile u32 enable_dma_descriptor : 1;
		volatile u32 frame_list_entries : 2;
		volatile u32 periodic_schedule_enable : 1;
		volatile const u32 periodic_schedule_status : 1;
		volatile u32 __reserved28_30 : 4;
		volatile u32 mode_chg_time : 1;
	} data;

} dwc_otg_host_hcfg_reg_t;

typedef union dwc_otg_host_hfir_reg {

	u32 raw;

	struct {
		volatile u32 interval : 16;
		volatile u32 dynamic_frame_reload : 1;
		volatile u32 __reserved17_31 : 15;
	} data;

} dwc_otg_host_hfir_reg_t;

typedef union dwc_otg_host_hfnum_reg {

	u32 raw;

	struct {
		volatile const u32 frame_number : 16;
		volatile const u32 frame_remaining : 16;
	} data;

} dwc_otg_host_hfnum_reg_t;

typedef union dwc_otg_host_hptxsts_reg {

	u32 raw;

	struct {
		volatile const u32 space_available : 16;
		volatile const u32 queue_space_available : 8;
		volatile const u32 terminate : 1;
		volatile const enum {
			token_type_zero_length = 0,
			token_type_ping = 1,
			token_type_disable = 2
		} token_type : 2;
		volatile const u32 channel : 4;
		volatile const u32 odd : 1;
	} data;

} dwc_otg_host_hptxsts_reg_t;

typedef enum {
	usb_speed_high = 0,
	usb_speed_full = 1,
	usb_speed_low = 2,
} usb_speed_t;

typedef enum {
	usb_transfer_control = 0,
	usb_transfer_isochronous = 1,
	usb_transfer_bulk = 2,
	usb_transfer_interrupt = 3,
} usb_transfer_t;

typedef enum {
	usb_direction_host_to_device = 0,
	usb_direction_out = 0,
	usb_direction_device_to_host = 1,
	usb_direction_in = 1,
} usb_direction_t;

typedef union dwc_otg_host_hprt_reg {

	u32 raw;

	struct {
		volatile const u32 connect : 1;
		volatile const u32 connect_detected : 1;
		volatile const u32 enable : 1;
		volatile const u32 enable_changed : 1;
		volatile const u32 over_current : 1;
		volatile const u32 over_current_changed : 1;
		volatile u32 resume : 1;
		volatile const u32 suspend : 1;
		volatile u32 reset : 1;
		volatile u32 __reserved9 : 1;
		volatile const u32 port_line_status : 2;
		volatile u32 power : 1;
		volatile u32 test_control : 4;
		volatile const usb_speed_t speed : 2;
		volatile u32 __reserved19_31 : 13;
	} data;

} dwc_otg_host_hprt_reg_t;

typedef union dwc_otg_host_hcchar_reg {

	u32 raw;

	struct {
		volatile u32 maximum_packet_size : 11;
		volatile u32 endpoint_number : 4;
		volatile usb_direction_t endpoint_direction : 1;
		volatile u32 _reserved16 : 1;
		volatile u32 low_speed : 1;
		usb_transfer_t type : 2;
		volatile u32 packets_per_frame : 2;
		volatile u32 device_address : 7;
		volatile u32 odd_frame  : 1;
		volatile const u32 disable : 1;
		volatile const u32 enable : 1;
	} data;

} dwc_otg_host_hcchar_reg_t;

typedef union dwc_otg_host_hcsplt_reg {

	u32 raw;

	struct {
		volatile u32 port_address : 7;
		volatile u32 hub_address : 7;
		volatile enum {
			transaction_position_middle = 0,
			transaction_position_end = 1,
			transaction_position_begin = 2,
			transaction_position_all = 3,
		} transaction_position : 2;
		volatile u32 complete_split : 1;
		volatile u32 __reserved17_30 : 14;
		volatile u32 split_enable : 1;
	} data;

} dwc_otg_host_hcsplt_reg_t;

typedef union dwc_otg_host_hctsiz_reg {

	u32 raw;

	struct {
		volatile u32 transfer_size : 19;
		volatile u32 packet_count : 10;
		volatile enum packet_id {
			packet_id_data0 = 0,
			packet_id_data1 = 2,
			packet_id_data2 = 1,
			packet_id_mdata = 3,
			packet_id_setup = 3,
		} packet_id : 2;
		volatile u32 do_ping : 1;
	} data;

} dwc_otg_host_hctsiz_reg_t;

typedef union dwc_otg_host_int_reg {

	u32 raw;

	struct {
		u32 transfer_complete : 1;
		u32 halt : 1;
		u32 abh_error : 1;
		u32 stall : 1;
		u32 negative_acknowledgement : 1;
		u32 acknowledgement : 1;
		u32 not_yet : 1;
		u32 transaction_error : 1;
		u32 babble_error : 1;
		u32 frame_overrun : 1;
		u32 data_toggle_error : 1;
		u32 buffer_not_available : 1;
		u32 excessive_transmission : 1;
		u32 frame_list_rollover : 1;
		u32 __reserved14_31 : 18;
	} data;

} dwc_otg_host_int_reg_t;

typedef struct dwc_otg_core_regs {
	/** OTG Control and Status Register		(Offset: 0x0) */
	volatile u32 gotgctl;

	/** OTG Interrupt Register			(Offset 0x4) */
	volatile u32 gotgint;

	/** AHB Configuration Register			(Offset 0x8) */
	volatile dwc_otg_core_gahbcfg_reg_t gahbcfg;

	/** USB Configuration Register			(Offset 0xC */
	volatile dwc_otg_core_gusbcfg_reg_t gusbcfg;

	/** Reset Register				(Offset 0x10) */
	volatile u32 grstctl;

	/** Interrupt Register				(Offset 0x14) */
	volatile u32 gintsts;

	/** Interrupt Mask Register			(Offset 0x18) */
	volatile u32 gintmsk;

	/**  Receive Status Debug Read Register		(Offset 0x1C) */
	volatile u32 grxstsr;

	/** Receive Status Read Pop Register		(Offset 0x20) */
	volatile u32 grxstsp;

	/** Receive FIFO Size Register			(Offset 0x24) */
	volatile u32 grxfsiz;

	/** Non-periodic Transmit FIFO Size Register	(Offset 0x28) */
	volatile u32 gnptxfsiz;

	/** Non-periodic Transmit FIFO Queue Status	(Offset 0x2C) */
	volatile u32 gnptxsts;

	/** Reserved */
	volatile u32 __reserved_0x30_0x34;

	/** PHY Vendor Control Register			(Offset 0x34) */
	volatile u32 gpvndctl;

	/** General Purpose  Input Output Register	(Offset 0x38) */
	volatile u32 ggpio;

	/** User ID Register				(Offset 0x3C) */
	volatile u32 guid;

	/** Synopsys ID Register			(Offset 0x40) */
	volatile u32 gsnpsid;

	/** User HW Config1 Register			(Offset 0x44) */
	volatile dwc_otg_core_ghwcfg1_reg_t ghwcfg1;

	/** User HW Config2 Register			(Offset 0x48) */
	volatile dwc_otg_core_ghwcfg2_reg_t ghwcfg2;

	/** User HW Config3 Register			(Offset 0x4C) */
	volatile dwc_otg_core_ghwcfg3_reg_t ghwcfg3;

	/** User HW Config4 Register			(Offset 0x50) */
	volatile dwc_otg_core_ghwcfg4_reg_t ghwcfg4;

	/** Reserved */
	volatile u32 __reserved_0x54_0x58[2];

	/** DFIFO Software Config Register		(Offset 0x5C) */
	volatile u32 gdfifocfg;

	/** Reserved */
	volatile u32 __reserved_0x60_0x7C[8];

	// BCM2835 specific registers

	/** bcm2835 MDIO interface control		(Offset 0x80 */
	volatile u32 gusbmdiocntl;

	/** bcm2835 Data for MDIO interface		(Offset 0x84 */
	volatile u32 gusbmdiogen;

	/** bcm2835 Vbus and other Miscellaneous ctrls	(Offset 0x88 */
	volatile u32 gusbvbusdrv;

	// BCM2835 specific registers end

	/** Reserved */
	volatile u32 __reserved_0x8C_0xFC[29];

	/** Host Periodic Transmit FIFO Size Register	(Offset 0x100) */
	volatile u32 hptxfsiz;

	/** Device IN Endpoint Transmit FIFO Size Regs	(Offset 0x104) */
	volatile u32 dieptxf[15];
} dwc_otg_core_regs_t;

typedef struct dwc_otg_host_regs {

	/** Config					(Offset 0x400) */
	volatile dwc_otg_host_hcfg_reg_t hcfg;

	/** Frame interval				(Offset 0x404) */
	volatile dwc_otg_host_hfir_reg_t hfir;

	/** Frame number				(Offset 0x408) */
	volatile dwc_otg_host_hfnum_reg_t hfnum;

	/** Reserved */
	volatile u32 __reserved0x40C;

	/** Fifo status					(Offset 0x410) */
	volatile dwc_otg_host_hptxsts_reg_t hptxsts;

	/** Interrupt					(Offset 0x414) */
	volatile u32 haint;

	/** Interrupt mask				(Offset 0x418 */
	volatile u32 haintmsk;

	/** Frame list					(Offset 0x41C) */
	volatile u32 hflbaddr;

	/** Reserved */
	volatile u32 __reserved0x420_0x43C[0x440 - 0x420];

	/** Port					(Offset 0x440) */
	volatile dwc_otg_host_hprt_reg_t hprt;

	/** Reserved */
	volatile u8 __reserved0x444_0x4FC[0x500 - 0x444];

	/** Channels					(Offset 0x500) */
	volatile struct {

		/** Characteristic Base			(Offset 0x0) */
		volatile dwc_otg_host_hcchar_reg_t hcchar;

		/** Split Control			(Offset 0x4) */
		volatile dwc_otg_host_hcsplt_reg_t hcsplt;

		/** Interrupt				(Offset 0x8) */
		volatile dwc_otg_host_int_reg_t hcint;

		/** Interrupt Mask			(Offset 0xC) */
		volatile dwc_otg_host_int_reg_t hcintmsk;

		/** Transfer Size			(Offset 0x10) */
		volatile dwc_otg_host_hctsiz_reg_t hctsiz;

		/** Dma Address				(Offset 0x14) */
		volatile u32 hcdma;

		/** Dma Buffer				(Offset 0x1C) */
		volatile u32 hcdmab;

	} channels[DWC_OTG_CHANNEL_COUNT];

} dwc_otg_host_regs_t;

typedef union dwc_otg_power_reg {

	u32 raw;

	struct {
		volatile u32 stop_pclock : 1;
		volatile u32 gate_hclock : 1;
		volatile u32 power_clamp : 1;
		volatile u32 power_down_modules : 1;
		volatile u32 phy_suspended : 1;
		volatile u32 enable_sleep_clock_gating : 1;
		volatile const u32 phy_sleeping : 1;
		volatile const u32 deep_sleep : 1;
		volatile u32 __reserved8_31 : 24;
	} data;

} dwc_otg_power_reg_t;

#endif // USB_HC_DWC_CORE_H
