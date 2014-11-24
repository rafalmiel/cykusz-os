#ifndef USB_HC_DWC_DWC_H
#define USB_HC_DWC_DWC_H

#include <core/common.h>

#include <drivers/usb/device.h>
#include <drivers/usb/pipe.h>
#include <drivers/usb/devicerequest.h>

#include "core.h"

extern dwc_otg_core_regs_t *dwc_core;
extern dwc_otg_host_regs_t *dwc_host;
extern dwc_otg_power_reg_t *dwc_power;

void micro_delay(u32 delay);

void init_dwc(u32 reg_base);

usb_result_t hcd_submit_control_message(struct usb_device *device,
					struct usb_pipe_address pipe,
					void *buffer, u32 buffer_length,
					struct usb_device_request *request);

usb_result_t usb_process_root_hub_message(struct usb_device *device,
					  struct usb_pipe_address pipe,
					  void *buffer, u32 buffer_length,
					  struct usb_device_request *request);

#endif // DWC_H
