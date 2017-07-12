/*
 * Copyright 2016 Advanced Micro Devices, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors: AMD
 *
 */
#ifndef __DCE_HWSEQ_H__
#define __DCE_HWSEQ_H__

#include "hw_sequencer.h"

#define HWSEQ_DCEF_REG_LIST_DCE8() \
	.DCFE_CLOCK_CONTROL[0] = mmCRTC0_CRTC_DCFE_CLOCK_CONTROL, \
	.DCFE_CLOCK_CONTROL[1] = mmCRTC1_CRTC_DCFE_CLOCK_CONTROL, \
	.DCFE_CLOCK_CONTROL[2] = mmCRTC2_CRTC_DCFE_CLOCK_CONTROL, \
	.DCFE_CLOCK_CONTROL[3] = mmCRTC3_CRTC_DCFE_CLOCK_CONTROL, \
	.DCFE_CLOCK_CONTROL[4] = mmCRTC4_CRTC_DCFE_CLOCK_CONTROL, \
	.DCFE_CLOCK_CONTROL[5] = mmCRTC5_CRTC_DCFE_CLOCK_CONTROL

#define HWSEQ_DCEF_REG_LIST() \
	SRII(DCFE_CLOCK_CONTROL, DCFE, 0), \
	SRII(DCFE_CLOCK_CONTROL, DCFE, 1), \
	SRII(DCFE_CLOCK_CONTROL, DCFE, 2), \
	SRII(DCFE_CLOCK_CONTROL, DCFE, 3), \
	SRII(DCFE_CLOCK_CONTROL, DCFE, 4), \
	SRII(DCFE_CLOCK_CONTROL, DCFE, 5), \
	SR(DC_MEM_GLOBAL_PWR_REQ_CNTL)

#define HWSEQ_BLND_REG_LIST() \
	SRII(BLND_V_UPDATE_LOCK, BLND, 0), \
	SRII(BLND_V_UPDATE_LOCK, BLND, 1), \
	SRII(BLND_V_UPDATE_LOCK, BLND, 2), \
	SRII(BLND_V_UPDATE_LOCK, BLND, 3), \
	SRII(BLND_V_UPDATE_LOCK, BLND, 4), \
	SRII(BLND_V_UPDATE_LOCK, BLND, 5), \
	SRII(BLND_CONTROL, BLND, 0), \
	SRII(BLND_CONTROL, BLND, 1), \
	SRII(BLND_CONTROL, BLND, 2), \
	SRII(BLND_CONTROL, BLND, 3), \
	SRII(BLND_CONTROL, BLND, 4), \
	SRII(BLND_CONTROL, BLND, 5)

#define HWSEQ_PIXEL_RATE_REG_LIST(blk) \
	SRII(PIXEL_RATE_CNTL, blk, 0), \
	SRII(PIXEL_RATE_CNTL, blk, 1), \
	SRII(PIXEL_RATE_CNTL, blk, 2), \
	SRII(PIXEL_RATE_CNTL, blk, 3), \
	SRII(PIXEL_RATE_CNTL, blk, 4), \
	SRII(PIXEL_RATE_CNTL, blk, 5)

#define HWSEQ_PHYPLL_REG_LIST(blk) \
	SRII(PHYPLL_PIXEL_RATE_CNTL, blk, 0), \
	SRII(PHYPLL_PIXEL_RATE_CNTL, blk, 1), \
	SRII(PHYPLL_PIXEL_RATE_CNTL, blk, 2), \
	SRII(PHYPLL_PIXEL_RATE_CNTL, blk, 3), \
	SRII(PHYPLL_PIXEL_RATE_CNTL, blk, 4), \
	SRII(PHYPLL_PIXEL_RATE_CNTL, blk, 5)

#define HWSEQ_DCE11_REG_LIST_BASE() \
	SR(DC_MEM_GLOBAL_PWR_REQ_CNTL), \
	SR(DCFEV_CLOCK_CONTROL), \
	SRII(DCFE_CLOCK_CONTROL, DCFE, 0), \
	SRII(DCFE_CLOCK_CONTROL, DCFE, 1), \
	SRII(CRTC_H_BLANK_START_END, CRTC, 0),\
	SRII(CRTC_H_BLANK_START_END, CRTC, 1),\
	SRII(BLND_V_UPDATE_LOCK, BLND, 0),\
	SRII(BLND_V_UPDATE_LOCK, BLND, 1),\
	SRII(BLND_CONTROL, BLND, 0),\
	SRII(BLND_CONTROL, BLND, 1),\
	SR(BLNDV_CONTROL),\
	HWSEQ_PIXEL_RATE_REG_LIST(CRTC)

#define HWSEQ_DCE8_REG_LIST() \
	HWSEQ_DCEF_REG_LIST_DCE8(), \
	HWSEQ_BLND_REG_LIST(), \
	HWSEQ_PIXEL_RATE_REG_LIST(CRTC)

#define HWSEQ_DCE10_REG_LIST() \
	HWSEQ_DCEF_REG_LIST(), \
	HWSEQ_BLND_REG_LIST(), \
	HWSEQ_PIXEL_RATE_REG_LIST(CRTC)

#define HWSEQ_ST_REG_LIST() \
	HWSEQ_DCE11_REG_LIST_BASE(), \
	.DCFE_CLOCK_CONTROL[2] = mmDCFEV_CLOCK_CONTROL, \
	.CRTC_H_BLANK_START_END[2] = mmCRTCV_H_BLANK_START_END, \
	.BLND_V_UPDATE_LOCK[2] = mmBLNDV_V_UPDATE_LOCK, \
	.BLND_CONTROL[2] = mmBLNDV_CONTROL,

#define HWSEQ_CZ_REG_LIST() \
	HWSEQ_DCE11_REG_LIST_BASE(), \
	SRII(DCFE_CLOCK_CONTROL, DCFE, 2), \
	SRII(CRTC_H_BLANK_START_END, CRTC, 2), \
	SRII(BLND_V_UPDATE_LOCK, BLND, 2), \
	SRII(BLND_CONTROL, BLND, 2), \
	.DCFE_CLOCK_CONTROL[3] = mmDCFEV_CLOCK_CONTROL, \
	.CRTC_H_BLANK_START_END[3] = mmCRTCV_H_BLANK_START_END, \
	.BLND_V_UPDATE_LOCK[3] = mmBLNDV_V_UPDATE_LOCK, \
	.BLND_CONTROL[3] = mmBLNDV_CONTROL

#define HWSEQ_DCE112_REG_LIST() \
	HWSEQ_DCE10_REG_LIST(), \
	HWSEQ_PIXEL_RATE_REG_LIST(CRTC), \
	HWSEQ_PHYPLL_REG_LIST(CRTC)

#if defined(CONFIG_DRM_AMD_DC_DCN1_0)
#define HWSEQ_DCN_REG_LIST()\
	HWSEQ_PIXEL_RATE_REG_LIST(OTG), \
	HWSEQ_PHYPLL_REG_LIST(OTG), \
	SRII(OTG_GLOBAL_SYNC_STATUS, OTG, 0), \
	SRII(OTG_GLOBAL_SYNC_STATUS, OTG, 1), \
	SRII(OTG_GLOBAL_SYNC_STATUS, OTG, 2), \
	SRII(OTG_GLOBAL_SYNC_STATUS, OTG, 3), \
	SRII(DCHUBP_CNTL, HUBP, 0), \
	SRII(DCHUBP_CNTL, HUBP, 1), \
	SRII(DCHUBP_CNTL, HUBP, 2), \
	SRII(DCHUBP_CNTL, HUBP, 3), \
	SRII(HUBP_CLK_CNTL, HUBP, 0), \
	SRII(HUBP_CLK_CNTL, HUBP, 1), \
	SRII(HUBP_CLK_CNTL, HUBP, 2), \
	SRII(HUBP_CLK_CNTL, HUBP, 3), \
	SRII(DPP_CONTROL, DPP_TOP, 0), \
	SRII(DPP_CONTROL, DPP_TOP, 1), \
	SRII(DPP_CONTROL, DPP_TOP, 2), \
	SRII(DPP_CONTROL, DPP_TOP, 3), \
	SRII(OPP_PIPE_CONTROL, OPP_PIPE, 0), \
	SRII(OPP_PIPE_CONTROL, OPP_PIPE, 1), \
	SRII(OPP_PIPE_CONTROL, OPP_PIPE, 2), \
	SRII(OPP_PIPE_CONTROL, OPP_PIPE, 3), \
	SR(REFCLK_CNTL), \
	SR(DCHUBBUB_GLOBAL_TIMER_CNTL), \
	SR(DC_IP_REQUEST_CNTL), \
	SR(DOMAIN0_PG_CONFIG), \
	SR(DOMAIN1_PG_CONFIG), \
	SR(DOMAIN2_PG_CONFIG), \
	SR(DOMAIN3_PG_CONFIG), \
	SR(DOMAIN4_PG_CONFIG), \
	SR(DOMAIN5_PG_CONFIG), \
	SR(DOMAIN6_PG_CONFIG), \
	SR(DOMAIN7_PG_CONFIG), \
	SR(DOMAIN0_PG_STATUS), \
	SR(DOMAIN1_PG_STATUS), \
	SR(DOMAIN2_PG_STATUS), \
	SR(DOMAIN3_PG_STATUS), \
	SR(DOMAIN4_PG_STATUS), \
	SR(DOMAIN5_PG_STATUS), \
	SR(DOMAIN6_PG_STATUS), \
	SR(DOMAIN7_PG_STATUS), \
	SR(DIO_MEM_PWR_CTRL), \
	SR(DCCG_GATE_DISABLE_CNTL), \
	SR(DCCG_GATE_DISABLE_CNTL2), \
	SR(DCFCLK_CNTL)
#endif

#if defined(CONFIG_DRM_AMD_DC_DCN1_0)
#define HWSEQ_DCN1_REG_LIST()\
	HWSEQ_DCN_REG_LIST()
#endif


struct dce_hwseq_registers {
	uint32_t DCFE_CLOCK_CONTROL[6];
	uint32_t DCFEV_CLOCK_CONTROL;
	uint32_t DC_MEM_GLOBAL_PWR_REQ_CNTL;
	uint32_t BLND_V_UPDATE_LOCK[6];
	uint32_t BLND_CONTROL[6];
	uint32_t BLNDV_CONTROL;
	uint32_t CRTC_H_BLANK_START_END[6];
	uint32_t PIXEL_RATE_CNTL[6];
	uint32_t PHYPLL_PIXEL_RATE_CNTL[6];

#if defined(CONFIG_DRM_AMD_DC_DCN1_0)
	uint32_t OTG_GLOBAL_SYNC_STATUS[4];
	uint32_t DCHUBP_CNTL[4];
	uint32_t HUBP_CLK_CNTL[4];
	uint32_t DPP_CONTROL[4];
	uint32_t OPP_PIPE_CONTROL[4];
	uint32_t REFCLK_CNTL;
	uint32_t DCHUBBUB_GLOBAL_TIMER_CNTL;
	uint32_t DC_IP_REQUEST_CNTL;
	uint32_t DOMAIN0_PG_CONFIG;
	uint32_t DOMAIN1_PG_CONFIG;
	uint32_t DOMAIN2_PG_CONFIG;
	uint32_t DOMAIN3_PG_CONFIG;
	uint32_t DOMAIN4_PG_CONFIG;
	uint32_t DOMAIN5_PG_CONFIG;
	uint32_t DOMAIN6_PG_CONFIG;
	uint32_t DOMAIN7_PG_CONFIG;
	uint32_t DOMAIN0_PG_STATUS;
	uint32_t DOMAIN1_PG_STATUS;
	uint32_t DOMAIN2_PG_STATUS;
	uint32_t DOMAIN3_PG_STATUS;
	uint32_t DOMAIN4_PG_STATUS;
	uint32_t DOMAIN5_PG_STATUS;
	uint32_t DOMAIN6_PG_STATUS;
	uint32_t DOMAIN7_PG_STATUS;
	uint32_t DIO_MEM_PWR_CTRL;
	uint32_t DCCG_GATE_DISABLE_CNTL;
	uint32_t DCCG_GATE_DISABLE_CNTL2;
	uint32_t DCFCLK_CNTL;
#endif
};
 /* set field name */
#define HWS_SF(blk_name, reg_name, field_name, post_fix)\
	.field_name = blk_name ## reg_name ## __ ## field_name ## post_fix

#define HWS_SF1(blk_name, reg_name, field_name, post_fix)\
	.field_name = blk_name ## reg_name ## __ ## blk_name ## field_name ## post_fix


#define HWSEQ_DCEF_MASK_SH_LIST(mask_sh, blk)\
	HWS_SF(blk, CLOCK_CONTROL, DCFE_CLOCK_ENABLE, mask_sh),\
	SF(DC_MEM_GLOBAL_PWR_REQ_CNTL, DC_MEM_GLOBAL_PWR_REQ_DIS, mask_sh)

#define HWSEQ_BLND_MASK_SH_LIST(mask_sh, blk)\
	HWS_SF(blk, V_UPDATE_LOCK, BLND_DCP_GRPH_V_UPDATE_LOCK, mask_sh),\
	HWS_SF(blk, V_UPDATE_LOCK, BLND_SCL_V_UPDATE_LOCK, mask_sh),\
	HWS_SF(blk, V_UPDATE_LOCK, BLND_DCP_GRPH_SURF_V_UPDATE_LOCK, mask_sh),\
	HWS_SF(blk, V_UPDATE_LOCK, BLND_BLND_V_UPDATE_LOCK, mask_sh),\
	HWS_SF(blk, V_UPDATE_LOCK, BLND_V_UPDATE_LOCK_MODE, mask_sh),\
	HWS_SF(blk, CONTROL, BLND_FEEDTHROUGH_EN, mask_sh),\
	HWS_SF(blk, CONTROL, BLND_ALPHA_MODE, mask_sh),\
	HWS_SF(blk, CONTROL, BLND_MODE, mask_sh),\
	HWS_SF(blk, CONTROL, BLND_MULTIPLIED_MODE, mask_sh)

#define HWSEQ_PIXEL_RATE_MASK_SH_LIST(mask_sh, blk)\
	HWS_SF1(blk, PIXEL_RATE_CNTL, PIXEL_RATE_SOURCE, mask_sh),\
	HWS_SF(blk, PIXEL_RATE_CNTL, DP_DTO0_ENABLE, mask_sh)

#define HWSEQ_PHYPLL_MASK_SH_LIST(mask_sh, blk)\
	HWS_SF1(blk, PHYPLL_PIXEL_RATE_CNTL, PHYPLL_PIXEL_RATE_SOURCE, mask_sh),\
	HWS_SF1(blk, PHYPLL_PIXEL_RATE_CNTL, PIXEL_RATE_PLL_SOURCE, mask_sh)

#define HWSEQ_DCE8_MASK_SH_LIST(mask_sh)\
	.DCFE_CLOCK_ENABLE = CRTC_DCFE_CLOCK_CONTROL__CRTC_DCFE_CLOCK_ENABLE ## mask_sh, \
	HWS_SF(BLND_, V_UPDATE_LOCK, BLND_DCP_GRPH_V_UPDATE_LOCK, mask_sh),\
	HWS_SF(BLND_, V_UPDATE_LOCK, BLND_SCL_V_UPDATE_LOCK, mask_sh),\
	HWS_SF(BLND_, V_UPDATE_LOCK, BLND_DCP_GRPH_SURF_V_UPDATE_LOCK, mask_sh),\
	HWS_SF(BLND_, CONTROL, BLND_MODE, mask_sh),\
	HWSEQ_PIXEL_RATE_MASK_SH_LIST(mask_sh, CRTC0_)

#define HWSEQ_DCE10_MASK_SH_LIST(mask_sh)\
	HWSEQ_DCEF_MASK_SH_LIST(mask_sh, DCFE_),\
	HWSEQ_BLND_MASK_SH_LIST(mask_sh, BLND_),\
	HWSEQ_PIXEL_RATE_MASK_SH_LIST(mask_sh, CRTC0_)

#define HWSEQ_DCE11_MASK_SH_LIST(mask_sh)\
	HWSEQ_DCE10_MASK_SH_LIST(mask_sh),\
	SF(DCFEV_CLOCK_CONTROL, DCFEV_CLOCK_ENABLE, mask_sh),\
	HWSEQ_PIXEL_RATE_MASK_SH_LIST(mask_sh, CRTC0_)

#define HWSEQ_DCE112_MASK_SH_LIST(mask_sh)\
	HWSEQ_DCE10_MASK_SH_LIST(mask_sh),\
	HWSEQ_PHYPLL_MASK_SH_LIST(mask_sh, CRTC0_)

#define HWSEQ_DCE12_MASK_SH_LIST(mask_sh)\
	HWSEQ_DCEF_MASK_SH_LIST(mask_sh, DCFE0_DCFE_),\
	HWSEQ_BLND_MASK_SH_LIST(mask_sh, BLND0_BLND_),\
	HWSEQ_PIXEL_RATE_MASK_SH_LIST(mask_sh, CRTC0_),\
	HWSEQ_PHYPLL_MASK_SH_LIST(mask_sh, CRTC0_)

#if defined(CONFIG_DRM_AMD_DC_DCN1_0)
#define HWSEQ_DCN_MASK_SH_LIST(mask_sh)\
	HWSEQ_PIXEL_RATE_MASK_SH_LIST(mask_sh, OTG0_),\
	HWS_SF1(OTG0_, PHYPLL_PIXEL_RATE_CNTL, PHYPLL_PIXEL_RATE_SOURCE, mask_sh), \
	HWS_SF(OTG0_, OTG_GLOBAL_SYNC_STATUS, VUPDATE_NO_LOCK_EVENT_CLEAR, mask_sh), \
	HWS_SF(OTG0_, OTG_GLOBAL_SYNC_STATUS, VUPDATE_NO_LOCK_EVENT_OCCURRED, mask_sh), \
	HWS_SF(HUBP0_, DCHUBP_CNTL, HUBP_NO_OUTSTANDING_REQ, mask_sh), \
	HWS_SF(HUBP0_, DCHUBP_CNTL, HUBP_VTG_SEL, mask_sh), \
	HWS_SF(HUBP0_, HUBP_CLK_CNTL, HUBP_CLOCK_ENABLE, mask_sh), \
	HWS_SF(DPP_TOP0_, DPP_CONTROL, DPP_CLOCK_ENABLE, mask_sh), \
	HWS_SF(OPP_PIPE0_, OPP_PIPE_CONTROL, OPP_PIPE_CLOCK_EN, mask_sh),\
	HWS_SF(, DCHUBBUB_GLOBAL_TIMER_CNTL, DCHUBBUB_GLOBAL_TIMER_ENABLE, mask_sh), \
	HWS_SF(, DC_IP_REQUEST_CNTL, IP_REQUEST_EN, mask_sh), \
	HWS_SF(, DOMAIN0_PG_CONFIG, DOMAIN0_POWER_FORCEON, mask_sh), \
	HWS_SF(, DOMAIN0_PG_CONFIG, DOMAIN0_POWER_GATE, mask_sh), \
	HWS_SF(, DOMAIN1_PG_CONFIG, DOMAIN1_POWER_FORCEON, mask_sh), \
	HWS_SF(, DOMAIN1_PG_CONFIG, DOMAIN1_POWER_GATE, mask_sh), \
	HWS_SF(, DOMAIN2_PG_CONFIG, DOMAIN2_POWER_FORCEON, mask_sh), \
	HWS_SF(, DOMAIN2_PG_CONFIG, DOMAIN2_POWER_GATE, mask_sh), \
	HWS_SF(, DOMAIN3_PG_CONFIG, DOMAIN3_POWER_FORCEON, mask_sh), \
	HWS_SF(, DOMAIN3_PG_CONFIG, DOMAIN3_POWER_GATE, mask_sh), \
	HWS_SF(, DOMAIN4_PG_CONFIG, DOMAIN4_POWER_FORCEON, mask_sh), \
	HWS_SF(, DOMAIN4_PG_CONFIG, DOMAIN4_POWER_GATE, mask_sh), \
	HWS_SF(, DOMAIN5_PG_CONFIG, DOMAIN5_POWER_FORCEON, mask_sh), \
	HWS_SF(, DOMAIN5_PG_CONFIG, DOMAIN5_POWER_GATE, mask_sh), \
	HWS_SF(, DOMAIN6_PG_CONFIG, DOMAIN6_POWER_FORCEON, mask_sh), \
	HWS_SF(, DOMAIN6_PG_CONFIG, DOMAIN6_POWER_GATE, mask_sh), \
	HWS_SF(, DOMAIN7_PG_CONFIG, DOMAIN7_POWER_FORCEON, mask_sh), \
	HWS_SF(, DOMAIN7_PG_CONFIG, DOMAIN7_POWER_GATE, mask_sh), \
	HWS_SF(, DOMAIN0_PG_STATUS, DOMAIN0_PGFSM_PWR_STATUS, mask_sh), \
	HWS_SF(, DOMAIN1_PG_STATUS, DOMAIN1_PGFSM_PWR_STATUS, mask_sh), \
	HWS_SF(, DOMAIN2_PG_STATUS, DOMAIN2_PGFSM_PWR_STATUS, mask_sh), \
	HWS_SF(, DOMAIN3_PG_STATUS, DOMAIN3_PGFSM_PWR_STATUS, mask_sh), \
	HWS_SF(, DOMAIN4_PG_STATUS, DOMAIN4_PGFSM_PWR_STATUS, mask_sh), \
	HWS_SF(, DOMAIN5_PG_STATUS, DOMAIN5_PGFSM_PWR_STATUS, mask_sh), \
	HWS_SF(, DOMAIN6_PG_STATUS, DOMAIN6_PGFSM_PWR_STATUS, mask_sh), \
	HWS_SF(, DOMAIN7_PG_STATUS, DOMAIN7_PGFSM_PWR_STATUS, mask_sh), \
	HWS_SF(, DCFCLK_CNTL, DCFCLK_GATE_DIS, mask_sh)
#endif

#if defined(CONFIG_DRM_AMD_DC_DCN1_0)
#define HWSEQ_DCN1_MASK_SH_LIST(mask_sh)\
	HWSEQ_DCN_MASK_SH_LIST(mask_sh), \
	HWS_SF1(OTG0_, PHYPLL_PIXEL_RATE_CNTL, PIXEL_RATE_PLL_SOURCE, mask_sh), \
	HWS_SF(DPP_TOP0_, DPP_CONTROL, DPPCLK_RATE_CONTROL, mask_sh)
#endif

#define HWSEQ_REG_FIELD_LIST(type) \
	type DCFE_CLOCK_ENABLE; \
	type DCFEV_CLOCK_ENABLE; \
	type DC_MEM_GLOBAL_PWR_REQ_DIS; \
	type BLND_DCP_GRPH_V_UPDATE_LOCK; \
	type BLND_SCL_V_UPDATE_LOCK; \
	type BLND_DCP_GRPH_SURF_V_UPDATE_LOCK; \
	type BLND_BLND_V_UPDATE_LOCK; \
	type BLND_V_UPDATE_LOCK_MODE; \
	type BLND_FEEDTHROUGH_EN; \
	type BLND_ALPHA_MODE; \
	type BLND_MODE; \
	type BLND_MULTIPLIED_MODE; \
	type DP_DTO0_ENABLE; \
	type PIXEL_RATE_SOURCE; \
	type PHYPLL_PIXEL_RATE_SOURCE; \
	type PIXEL_RATE_PLL_SOURCE; \

#if defined(CONFIG_DRM_AMD_DC_DCN1_0)
#define HWSEQ_DCN_REG_FIELD_LIST(type) \
	type VUPDATE_NO_LOCK_EVENT_CLEAR; \
	type VUPDATE_NO_LOCK_EVENT_OCCURRED; \
	type HUBP_NO_OUTSTANDING_REQ; \
	type HUBP_VTG_SEL; \
	type HUBP_CLOCK_ENABLE; \
	type DPP_CLOCK_ENABLE; \
	type DPPCLK_RATE_CONTROL; \
	type DCHUBBUB_GLOBAL_TIMER_ENABLE; \
	type OPP_PIPE_CLOCK_EN;\
	type IP_REQUEST_EN; \
	type DOMAIN0_POWER_FORCEON; \
	type DOMAIN0_POWER_GATE; \
	type DOMAIN1_POWER_FORCEON; \
	type DOMAIN1_POWER_GATE; \
	type DOMAIN2_POWER_FORCEON; \
	type DOMAIN2_POWER_GATE; \
	type DOMAIN3_POWER_FORCEON; \
	type DOMAIN3_POWER_GATE; \
	type DOMAIN4_POWER_FORCEON; \
	type DOMAIN4_POWER_GATE; \
	type DOMAIN5_POWER_FORCEON; \
	type DOMAIN5_POWER_GATE; \
	type DOMAIN6_POWER_FORCEON; \
	type DOMAIN6_POWER_GATE; \
	type DOMAIN7_POWER_FORCEON; \
	type DOMAIN7_POWER_GATE; \
	type DOMAIN0_PGFSM_PWR_STATUS; \
	type DOMAIN1_PGFSM_PWR_STATUS; \
	type DOMAIN2_PGFSM_PWR_STATUS; \
	type DOMAIN3_PGFSM_PWR_STATUS; \
	type DOMAIN4_PGFSM_PWR_STATUS; \
	type DOMAIN5_PGFSM_PWR_STATUS; \
	type DOMAIN6_PGFSM_PWR_STATUS; \
	type DOMAIN7_PGFSM_PWR_STATUS; \
	type DCFCLK_GATE_DIS;
#endif

struct dce_hwseq_shift {
	HWSEQ_REG_FIELD_LIST(uint8_t)
#if defined(CONFIG_DRM_AMD_DC_DCN1_0)
	HWSEQ_DCN_REG_FIELD_LIST(uint8_t)
#endif
};

struct dce_hwseq_mask {
	HWSEQ_REG_FIELD_LIST(uint32_t)
#if defined(CONFIG_DRM_AMD_DC_DCN1_0)
	HWSEQ_DCN_REG_FIELD_LIST(uint32_t)
#endif
};


enum blnd_mode {
	BLND_MODE_CURRENT_PIPE = 0,/* Data from current pipe only */
	BLND_MODE_OTHER_PIPE, /* Data from other pipe only */
	BLND_MODE_BLENDING,/* Alpha blending - blend 'current' and 'other' */
};

void dce_enable_fe_clock(struct dce_hwseq *hwss,
		unsigned int inst, bool enable);

void dce_pipe_control_lock(struct core_dc *dc,
		struct pipe_ctx *pipe,
		bool lock);

void dce_set_blender_mode(struct dce_hwseq *hws,
	unsigned int blnd_inst, enum blnd_mode mode);

void dce_clock_gating_power_up(struct dce_hwseq *hws,
		bool enable);

void dce_crtc_switch_to_clk_src(struct dce_hwseq *hws,
		struct clock_source *clk_src,
		unsigned int tg_inst);

bool dce_use_lut(const struct core_surface *surface);
#endif   /*__DCE_HWSEQ_H__*/
