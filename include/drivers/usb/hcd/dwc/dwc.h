#ifndef USB_HC_DWC_DWC_H
#define USB_HC_DWC_DWC_H

#include <core/common.h>

#include "core.h"

extern dwc_otg_core_regs_t *dwc_core;
extern dwc_otg_host_regs_t *dwc_host;
extern dwc_otg_power_reg_t *dwc_power;

void micro_delay(u32 delay);

void init_dwc(u32 reg_base);

#endif // DWC_H
