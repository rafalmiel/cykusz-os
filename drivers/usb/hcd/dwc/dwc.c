#include <core/io.h>

#include <drivers/hcd/dwc/core.h>
#include <drivers/hcd/dwc/dwc.h>

dwc_otg_core_regs_t *core = 0;

void init_dwc(u32 reg_base)
{
	core = (dwc_otg_core_regs_t *)reg_base;

	kprint("Synopsis ID: ");
	kprint_hexnl(core->gsnpsid);
	kprint("User ID: ");
	kprint_hexnl(core->guid);
	//kprint("Dma remainder mode: ");
	core->gahbcfg.data.dma_enable = 1;
	core->gahbcfg.data.dma_remainder_mode = 1;
	core->gahbcfg.data.__reserved24_31 = 1;

//	kprint_hexnl(core->gahbcfg.data.dma_enable);
//	kprint_hexnl(core->gahbcfg.data.dma_remainder_mode);
//	kprint_hexnl(core->gahbcfg.data.__reserved24_31);
//	kprint_hexnl(sizeof core->gahbcfg);

	kprint("Endpoints count: ");
	kprint_hexnl(core->ghwcfg2.data.endpoints_count);

	kprint("Host channels count: ");
	kprint_hexnl(core->ghwcfg2.data.host_channels_count);

	kprint("Host periodic queue depth: ");
	kprint_hexnl(core->ghwcfg2.data.host_periodic_queue_depth);

	kprint("IN endpoint count: ");
	kprint_hexnl(core->ghwcfg4.data.in_endpoints_count);

	kprint("dfifo depth: ");
	kprint_hexnl(core->ghwcfg2.data.otgmode);
}
