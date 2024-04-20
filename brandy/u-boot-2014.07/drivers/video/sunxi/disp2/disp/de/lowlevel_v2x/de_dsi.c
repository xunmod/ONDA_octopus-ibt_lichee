#include "de_feat.h"
#include "de_dsi_type.h"
#include "de_dsi.h"

#if defined(SUPPORT_DSI) && defined(DSI_VERSION_40)

static volatile struct __de_dsi_dev_t *dsi_dev[2];
static volatile struct __de_dsi_dphy_dev_t *dphy_dev[1];

u32 dsi_pixel_bits[4] = { 24, 24, 18, 16 };
u32 dsi_lane_den[4] = { 0x1, 0x3, 0x7, 0xf };
u32 tcon_div = 4;

s32 dsi_delay_ms(u32 ms)
{
	disp_delay_ms(ms);

	return 0;
}

s32 dsi_delay_us(u32 us)
{
	disp_delay_us(us);

	return 0;
}

s32 dsi_set_reg_base(u32 sel, uintptr_t base)
{
	dsi_dev[sel] = (struct __de_dsi_dev_t *) base;
	dphy_dev[sel] = (struct __de_dsi_dphy_dev_t *) (base + 0x1000);

	return 0;
}

u32 dsi_get_reg_base(u32 sel)
{
	return (u32) (uintptr_t) dsi_dev[sel];
}

u32 dsi_get_start_delay(u32 sel)
{
	u32 dsi_start_delay =
	    dsi_dev[sel]->dsi_basic_ctl1.bits.video_start_delay;
	u32 vt = dsi_dev[sel]->dsi_basic_size1.bits.vt;
	u32 vsa = dsi_dev[sel]->dsi_basic_size0.bits.vsa;
	u32 vbp = dsi_dev[sel]->dsi_basic_size0.bits.vbp;
	u32 y = dsi_dev[sel]->dsi_basic_size1.bits.vact;
	u32 vfp = vt - vsa - vbp - y;
	u32 start_delay = dsi_start_delay + vfp;

	if (start_delay > dsi_dev[sel]->dsi_basic_size1.bits.vt)
		start_delay -= dsi_dev[sel]->dsi_basic_size1.bits.vt;

	return vt - y - 8;
}

u32 dsi_get_cur_line(u32 sel)
{
	u32 curr_line = dsi_dev[sel]->dsi_debug_video0.bits.video_curr_line;
	u32 vt = dsi_dev[sel]->dsi_basic_size1.bits.vt;
	u32 vsa = dsi_dev[sel]->dsi_basic_size0.bits.vsa;
	u32 vbp = dsi_dev[sel]->dsi_basic_size0.bits.vbp;
	u32 y = dsi_dev[sel]->dsi_basic_size1.bits.vact;
	u32 vfp = vt - vsa - vbp - y;
	curr_line += vfp;
	if (curr_line > vt)
		curr_line -= vt;

	return curr_line;
}

s32 dsi_irq_enable(u32 sel, enum __dsi_irq_id_t id)
{
	dsi_dev[sel]->dsi_gint0.bits.dsi_irq_en |= (1 << id);

	return 0;
}

s32 dsi_irq_disable(u32 sel, enum __dsi_irq_id_t id)
{
	dsi_dev[sel]->dsi_gint0.bits.dsi_irq_en &= ~(1 << id);

	return 0;
}

u32 dsi_irq_query(u32 sel, enum __dsi_irq_id_t id)
{
	u32 en, fl;
	en = dsi_dev[sel]->dsi_gint0.bits.dsi_irq_en;
	fl = dsi_dev[sel]->dsi_gint0.bits.dsi_irq_flag;
	if (en & fl & (1 << id)) {
		dsi_dev[sel]->dsi_gint0.bits.dsi_irq_flag |= (1 << id);
		return 1;
	} else {
		return 0;
	}
}

s32 dsi_inst_busy(u32 sel)
{
	return dsi_dev[sel]->dsi_basic_ctl0.bits.inst_st;
}

s32 dsi_start(u32 sel, enum __dsi_start_t func)
{
	switch (func) {
	case DSI_START_TBA:
		dsi_dev[sel]->dsi_inst_jump_sel.dwval =
		    DSI_INST_ID_TBA << (4 * DSI_INST_ID_LP11)
		    | DSI_INST_ID_END << (4 * DSI_INST_ID_TBA);
		break;
	case DSI_START_HSTX:
		dsi_dev[sel]->dsi_inst_jump_sel.dwval =
		    DSI_INST_ID_HSC << (4 * DSI_INST_ID_LP11)
		    | DSI_INST_ID_NOP << (4 * DSI_INST_ID_HSC)
		    | DSI_INST_ID_HSD << (4 * DSI_INST_ID_NOP)
		    | DSI_INST_ID_DLY << (4 * DSI_INST_ID_HSD)
		    | DSI_INST_ID_NOP << (4 * DSI_INST_ID_DLY)
		    | DSI_INST_ID_END << (4 * DSI_INST_ID_HSCEXIT);
		break;
	case DSI_START_LPTX:
		dsi_dev[sel]->dsi_inst_jump_sel.dwval =
		    DSI_INST_ID_LPDT << (4 * DSI_INST_ID_LP11)
		    | DSI_INST_ID_END << (4 * DSI_INST_ID_LPDT);
		break;
	case DSI_START_LPRX:
		dsi_dev[sel]->dsi_inst_jump_sel.dwval =
		    DSI_INST_ID_LPDT << (4 * DSI_INST_ID_LP11)
		    | DSI_INST_ID_DLY << (4 * DSI_INST_ID_LPDT)
		    | DSI_INST_ID_TBA << (4 * DSI_INST_ID_DLY)
		    | DSI_INST_ID_END << (4 * DSI_INST_ID_TBA);
		break;
	case DSI_START_HSC:
		dsi_dev[sel]->dsi_inst_jump_sel.dwval =
		    DSI_INST_ID_HSC << (4 * DSI_INST_ID_LP11)
		    | DSI_INST_ID_END << (4 * DSI_INST_ID_HSC);
		break;
	case DSI_START_HSD:
		dsi_dev[sel]->dsi_inst_jump_sel.dwval =
		    DSI_INST_ID_NOP << (4 * DSI_INST_ID_LP11)
		    | DSI_INST_ID_HSD << (4 * DSI_INST_ID_NOP)
		    | DSI_INST_ID_DLY << (4 * DSI_INST_ID_HSD)
		    | DSI_INST_ID_NOP << (4 * DSI_INST_ID_DLY)
		    | DSI_INST_ID_END << (4 * DSI_INST_ID_HSCEXIT);
		break;
	default:
		dsi_dev[sel]->dsi_inst_jump_sel.dwval =
		    DSI_INST_ID_END << (4 * DSI_INST_ID_LP11);
		break;
	}
	dsi_dev[sel]->dsi_basic_ctl0.bits.inst_st = 0;
	dsi_dev[sel]->dsi_basic_ctl0.bits.inst_st = 1;
	if (func == DSI_START_HSC)
		dsi_dev[sel]->dsi_inst_func[DSI_INST_ID_LP11].bits.lane_cen = 0;

	return 0;
}

s32 dsi_open(u32 sel, disp_panel_para *panel)
{
	if (panel->lcd_fresh_mode == 0) {
		dsi_irq_enable(sel, DSI_IRQ_VIDEO_VBLK);
		dsi_start(sel, DSI_START_HSD);
		/*
		   while (dphy_dev[sel]->dphy_dbg0.bits.lptx_sta_clk != 5);
		   dphy_dev[sel]->dphy_ana1.bits.reg_vttmode =  1;
		   dphy_dev[sel]->dphy_ana2.bits.enp2s_cpu =
			dsi_lane_den[panel->lcd_dsi_lane-1];
		 */
	}

	return 0;
}

s32 dsi_close(u32 sel)
{
	dsi_irq_disable(sel, DSI_IRQ_VIDEO_VBLK);
	dsi_dev[sel]->dsi_inst_jump_cfg[0].bits.jump_cfg_en = 1;
	dsi_delay_ms(30);
	/*
	while (dphy_dev[sel]->dphy_dbg0.bits.lptx_sta_d0 == 5);
	dphy_dev[sel]->dphy_ana2.bits.enp2s_cpu	= 0;
	dphy_dev[sel]->dphy_ana1.bits.reg_vttmode =	1;
	*/

	return 0;
}

s32 dsi_tri_start(u32 sel)
{
	dsi_start(sel, DSI_START_HSTX);
#if 0
	while (dsi_dev[sel]->dsi_debug_inst.bits.curr_instru_num
	    != DSI_INST_ID_HSD)
		;
	while (dphy_dev[sel]->dphy_dbg0.bits.lptx_sta_d0 != 5)
		;
#endif

	return 0;
}

s32 dsi_dcs_wr(u32 sel, u8 cmd, u8 *para_p, u32 para_num)
{
	volatile u8 *p = (u8 *) dsi_dev[sel]->dsi_cmd_tx;
	u32 count = 0;

	while ((dsi_dev[sel]->dsi_basic_ctl0.bits.inst_st == 1)
	    && (count < 50)) {
		count++;
		dsi_delay_us(100);
	}
	if (count >= 50)
		dsi_dev[sel]->dsi_basic_ctl0.bits.inst_st = 0;

	if (para_num == 0) {

		*(p++) = DSI_DT_DCS_WR_P0;
		*(p++) = cmd;
		*(p++) = 0x00;
	} else if (para_num == 1) {
		*(p++) = DSI_DT_DCS_WR_P1;
		*(p++) = cmd;
		*(p++) = *para_p;
	} else {
		*(p++) = DSI_DT_DCS_LONG_WR;
		*(p++) = (para_num + 1) >> 0 & 0xff;
		*(p++) = (para_num + 1) >> 8 & 0xff;
	}
	*(p++) = dsi_ecc_pro(dsi_dev[sel]->dsi_cmd_tx[0].dwval);

	if (para_num > 1) {
		u16 crc, i;
		*(p++) = cmd;
		for (i = 0; i < para_num; i++)
			*(p++) = *(para_p + i);
		crc =
		    dsi_crc_pro((u8 *) (dsi_dev[sel]->dsi_cmd_tx + 1),
				para_num + 1);
		*(p++) = (crc >> 0) & 0xff;
		*(p++) = (crc >> 8) & 0xff;
		dsi_dev[sel]->dsi_cmd_ctl.bits.tx_size =
		    4 + 1 + para_num + 2 - 1;
	} else {
		dsi_dev[sel]->dsi_cmd_ctl.bits.tx_size = 4 - 1;
	}
	dsi_start(sel, DSI_START_LPTX);

	return 0;
}

s32 dsi_dcs_wr_index(u32 sel, u8 index)
{
	return 0;
}

s32 dsi_dcs_wr_data(u32 sel, u8 data)
{
	return 0;
}

s32 dsi_dcs_wr_0para(u32 sel, u8 cmd)
{
	u8 tmp;

	return dsi_dcs_wr(0, cmd, &tmp, 0);
}

s32 dsi_dcs_wr_1para(u32 sel, u8 cmd, u8 para)
{
	u8 tmp = para;

	return dsi_dcs_wr(0, cmd, &tmp, 1);
}

s32 dsi_dcs_wr_2para(u32 sel, u8 cmd, u8 para1, u8 para2)
{
	u8 tmp[2];

	tmp[0] = para1;
	tmp[1] = para2;
	return dsi_dcs_wr(0, cmd, tmp, 2);
}

s32 dsi_dcs_wr_3para(u32 sel, u8 cmd, u8 para1, u8 para2, u8 para3)
{
	u8 tmp[3];

	tmp[0] = para1;
	tmp[1] = para2;
	tmp[2] = para3;
	return dsi_dcs_wr(0, cmd, tmp, 3);
}

s32 dsi_dcs_wr_4para(u32 sel, u8 cmd, u8 para1, u8 para2, u8 para3, u8 para4)
{
	u8 tmp[4];

	tmp[0] = para1;
	tmp[1] = para2;
	tmp[2] = para3;
	tmp[3] = para4;
	return dsi_dcs_wr(0, cmd, tmp, 4);
}

s32 dsi_dcs_wr_5para(u32 sel, u8 cmd, u8 para1, u8 para2, u8 para3, u8 para4,
		     u8 para5)
{
	u8 tmp[5];

	tmp[0] = para1;
	tmp[1] = para2;
	tmp[2] = para3;
	tmp[3] = para4;
	tmp[4] = para5;
	return dsi_dcs_wr(0, cmd, tmp, 5);
}

s32 dsi_dcs_wr_memory(u32 sel, u32 *p_data, u32 length)
{
	u32 tx_cntr = length;
	u32 tx_num;
	u32 *tx_p = p_data;
	u8 para[256];
	u32 i;
	u32 start = 1;

	while (tx_cntr) {
		if (tx_cntr >= 83)
			tx_num = 83;
		else
			tx_num = tx_cntr;
		tx_cntr -= tx_num;

		for (i = 0; i < tx_num; i++) {
			para[i * 3 + 0] = (*tx_p >> 16) & 0xff;
			para[i * 3 + 1] = (*tx_p >> 8) & 0xff;
			para[i * 3 + 2] = (*tx_p >> 0) & 0xff;
			tx_p++;
		}

		if (start) {
			dsi_dcs_wr(sel, DSI_DCS_WRITE_MEMORY_START, para,
				   tx_num * 3);
			start = 0;
		} else {
			dsi_dcs_wr(sel, DSI_DCS_WRITE_MEMORY_CONTINUE, para,
				   tx_num * 3);
		}
	}
	return 0;
}

s32 dsi_gen_wr(u32 sel, u8 cmd, u8 *para_p, u32 para_num)
{
	volatile u8 *p = (u8 *) dsi_dev[sel]->dsi_cmd_tx;
	while (dsi_inst_busy(sel))
		;
	if (para_num == 0) {
		*(p++) = DSI_DT_GEN_WR_P1;
		*(p++) = cmd;
		*(p++) = 0x00;
	} else if (para_num == 1) {
		*(p++) = DSI_DT_GEN_WR_P2;
		*(p++) = cmd;
		*(p++) = *para_p;
	} else {
		*(p++) = DSI_DT_GEN_LONG_WR;
		*(p++) = (para_num + 1) >> 0 & 0xff;
		*(p++) = (para_num + 1) >> 8 & 0xff;
	}
	*(p++) = dsi_ecc_pro(dsi_dev[sel]->dsi_cmd_tx[0].dwval);

	if (para_num > 1) {
		__u16 crc, i;
		*(p++) = cmd;
		for (i = 0; i < para_num; i++)
			*(p++) = *(para_p + i);
		crc = dsi_crc_pro((u8 *) (dsi_dev[sel]->dsi_cmd_tx + 1),
			para_num + 1);
		*(p++) = (crc >> 0) & 0xff;
		*(p++) = (crc >> 8) & 0xff;
		dsi_dev[sel]->dsi_cmd_ctl.bits.tx_size =
		    4 + 1 + para_num + 2 - 1;
	} else {
		dsi_dev[sel]->dsi_cmd_ctl.bits.tx_size = 4 - 1;
	}
	dsi_start(sel, DSI_START_LPTX);
	return 0;
}

s32 dsi_set_max_ret_size(u32 sel, u32 size)
{
	dsi_dev[sel]->dsi_cmd_tx[0].bits.byte0 = DSI_DT_MAX_RET_SIZE;
	dsi_dev[sel]->dsi_cmd_tx[0].bits.byte1 = (size >> 0) & 0xff;
	dsi_dev[sel]->dsi_cmd_tx[0].bits.byte2 = (size >> 8) & 0xff;
	dsi_dev[sel]->dsi_cmd_tx[0].bits.byte3 =
	    dsi_ecc_pro(dsi_dev[sel]->dsi_cmd_tx[0].dwval);
	dsi_dev[sel]->dsi_cmd_ctl.bits.tx_size = 4 - 1;
	dsi_start(sel, DSI_START_LPTX);
	return 0;
}

s32 dsi_dcs_rd(u32 sel, u8 cmd, u8 *para_p, u32 *num_p)
{
	u32 num, i;
	u32 count = 0;
	dsi_dev[sel]->dsi_cmd_tx[0].bits.byte0 = DSI_DT_DCS_RD_P0;
	dsi_dev[sel]->dsi_cmd_tx[0].bits.byte1 = cmd;
	dsi_dev[sel]->dsi_cmd_tx[0].bits.byte2 = 0x00;
	dsi_dev[sel]->dsi_cmd_tx[0].bits.byte3 =
	    dsi_ecc_pro(dsi_dev[sel]->dsi_cmd_tx[0].dwval);
	dsi_dev[sel]->dsi_cmd_ctl.bits.tx_size = 4 - 1;

	dsi_start(sel, DSI_START_LPRX);

	while ((dsi_dev[sel]->dsi_basic_ctl0.bits.inst_st == 1)
	    && (count < 50)) {
		count++;
		dsi_delay_us(100);
	}
	if (count >= 50)
		dsi_dev[sel]->dsi_basic_ctl0.bits.inst_st = 0;

	if (dsi_dev[sel]->dsi_cmd_ctl.bits.rx_flag) {
		if (dsi_dev[sel]->dsi_cmd_ctl.bits.rx_overflow)
			return -1;
		if (dsi_dev[sel]->dsi_cmd_rx[0].bits.byte0 == DSI_DT_ACK_ERR)
			return -1;

		if (dsi_dev[sel]->dsi_cmd_rx[0].bits.byte0
		    == DSI_DT_DCS_RD_R1) {
			num = 1;
			*(para_p + 0) = dsi_dev[sel]->dsi_cmd_rx[0].bits.byte1;
		} else if (dsi_dev[sel]->dsi_cmd_rx[0].bits.byte0 ==
			   (u8) DSI_DT_DCS_RD_R2) {
			num = 2;
			*(para_p + 0) = dsi_dev[sel]->dsi_cmd_rx[0].bits.byte1;
			*(para_p + 1) = dsi_dev[sel]->dsi_cmd_rx[0].bits.byte2;
		} else if (dsi_dev[sel]->dsi_cmd_rx[0].bits.byte0 ==
			   (u8) DSI_DT_DCS_LONG_RD_R) {
			num = dsi_dev[sel]->dsi_cmd_ctl.bits.rx_size + 1 - 6;
			for (i = 0; i < num; i++) {
				*(para_p + i) =
				    *((u8 *) dsi_dev[sel]->dsi_cmd_rx + 4 + i);
			}
		} else {
			num = 0;
			return -1;
		}
		*num_p = num;
	}

	return 0;
}

s32 dsi_dcs_rd_memory(u32 sel, u32 *p_data, u32 length)
{
	u32 rx_cntr = length;
	u32 rx_curr;
	u32 *rx_p = p_data;
	u32 i;
	u32 start = 1;

	u8 rx_bf[32];
	u32 rx_num;

	dsi_set_max_ret_size(sel, 24);

	while (rx_cntr) {
		if (rx_cntr >= 8) {
			rx_curr = 8;
		} else {
			rx_curr = rx_cntr;
			dsi_set_max_ret_size(sel, rx_curr * 3);
		}
		rx_cntr -= rx_curr;

		if (start) {
			dsi_dcs_rd(sel, DSI_DCS_READ_MEMORY_START, rx_bf,
				   &rx_num);
			start = 0;
		} else {
			dsi_dcs_rd(sel, DSI_DCS_READ_MEMORY_CONTINUE, rx_bf,
				   &rx_num);
		}

		if (rx_num != rx_curr * 3)
			return -1;

		for (i = 0; i < rx_curr; i++) {
			*rx_p &= 0xff000000;
			*rx_p |= 0xff000000;
			*rx_p |= (u32) *(rx_bf + 4 + i * 3 + 0) << 16;
			*rx_p |= (u32) *(rx_bf + 4 + i * 3 + 1) << 8;
			*rx_p |= (u32) *(rx_bf + 4 + i * 3 + 2) << 0;
			rx_p++;
		}
	}
	return 0;
}

static s32 dsi_dphy_cfg(u32 sel, disp_panel_para *panel)
{
	__disp_dsi_dphy_timing_t *dphy_timing_p;
	__disp_dsi_dphy_timing_t dphy_timing_cfg1 = {
		/* lp_clk_div(100ns) hs_prepare(70ns) hs_trail(100ns) */
		14,
		6,
		10,
		/* clk_prepare(70ns) clk_zero(300ns) clk_pre */
		7,
		50,
		3,
		/*
		 * clk_post: 400*6.734 for nop inst  2.5us
		 * clk_trail: 200ns
		 * hs_dly_mode
		 */
		10,
		30,
		0,
		/* hs_dly lptx_ulps_exit hstx_ana1 hstx_ana0 */
		10,
		3,
		3,
		3,
	};

	dphy_dev[sel]->dphy_gctl.bits.module_en = 0;
	dphy_dev[sel]->dphy_gctl.bits.lane_num = panel->lcd_dsi_lane - 1;

	dphy_dev[sel]->dphy_tx_ctl.bits.hstx_clk_cont = 1;

	dphy_timing_p = &dphy_timing_cfg1;

	dphy_dev[sel]->dphy_tx_time0.bits.lpx_tm_set =
	    dphy_timing_p->lp_clk_div;
	dphy_dev[sel]->dphy_tx_time0.bits.hs_pre_set =
	    dphy_timing_p->hs_prepare;
	dphy_dev[sel]->dphy_tx_time0.bits.hs_trail_set =
	    dphy_timing_p->hs_trail;
	dphy_dev[sel]->dphy_tx_time1.bits.ck_prep_set =
	    dphy_timing_p->clk_prepare;
	dphy_dev[sel]->dphy_tx_time1.bits.ck_zero_set = dphy_timing_p->clk_zero;
	dphy_dev[sel]->dphy_tx_time1.bits.ck_pre_set = dphy_timing_p->clk_pre;
	dphy_dev[sel]->dphy_tx_time1.bits.ck_post_set = dphy_timing_p->clk_post;
	dphy_dev[sel]->dphy_tx_time2.bits.ck_trail_set =
	    dphy_timing_p->clk_trail;
	dphy_dev[sel]->dphy_tx_time2.bits.hs_dly_mode = 0;
	dphy_dev[sel]->dphy_tx_time2.bits.hs_dly_set = 0;
	dphy_dev[sel]->dphy_tx_time3.bits.lptx_ulps_exit_set = 0;
	dphy_dev[sel]->dphy_tx_time4.bits.hstx_ana0_set = 3;
	dphy_dev[sel]->dphy_tx_time4.bits.hstx_ana1_set = 3;
	dphy_dev[sel]->dphy_gctl.bits.module_en = 1;
/*
	dphy_dev[sel]->dphy_ana1.bits.reg_vttmode =	0;
	dphy_dev[sel]->dphy_ana0.bits.reg_selsck = 0;
	dphy_dev[sel]->dphy_ana0.bits.reg_pws =	1;
	dphy_dev[sel]->dphy_ana0.bits.reg_sfb =	0;
	dphy_dev[sel]->dphy_ana0.bits.reg_dmpc = 1;
	dphy_dev[sel]->dphy_ana0.bits.reg_dmpd =
	    dsi_lane_den[panel->lcd_dsi_lane-1];
	dphy_dev[sel]->dphy_ana0.bits.reg_slv =	0x7;
	dphy_dev[sel]->dphy_ana0.bits.reg_den =
	    dsi_lane_den[panel->lcd_dsi_lane-1];
	dphy_dev[sel]->dphy_ana1.bits.reg_svtt = 7;
	dphy_dev[sel]->dphy_ana4.bits.reg_ckdv = 0x1;
	dphy_dev[sel]->dphy_ana4.bits.reg_tmsc = 0x1;
	dphy_dev[sel]->dphy_ana4.bits.reg_tmsd = 0x1;
	dphy_dev[sel]->dphy_ana4.bits.reg_txdnsc = 0x1;
	dphy_dev[sel]->dphy_ana4.bits.reg_txdnsd = 0x1;
	dphy_dev[sel]->dphy_ana4.bits.reg_txpusc = 0x1;
	dphy_dev[sel]->dphy_ana4.bits.reg_txpusd = 0x1;
	dphy_dev[sel]->dphy_ana4.bits.reg_dmplvc = 0x1;
	dphy_dev[sel]->dphy_ana4.bits.reg_dmplvd =
	    dsi_lane_den[panel->lcd_dsi_lane-1];

	dphy_dev[sel]->dphy_ana2.bits.enib = 1;
	dsi_delay_us(5);
	dphy_dev[sel]->dphy_ana3.bits.enldor = 1;
	dphy_dev[sel]->dphy_ana3.bits.enldoc = 1;
	dphy_dev[sel]->dphy_ana3.bits.enldod = 1;
	dsi_delay_us(1);
	dphy_dev[sel]->dphy_ana3.bits.envttc = 1;
	dphy_dev[sel]->dphy_ana3.bits.envttd =
	    dsi_lane_den[panel->lcd_dsi_lane-1];
	dsi_delay_us(1);
	dphy_dev[sel]->dphy_ana3.bits.endiv	= 1;
	dsi_delay_us(1);
	dphy_dev[sel]->dphy_ana2.bits.enck_cpu = 1;
	dsi_delay_us(1);

	dphy_dev[sel]->dphy_ana1.bits.reg_vttmode =	1;
	dphy_dev[sel]->dphy_ana2.bits.enp2s_cpu	=
	    dsi_lane_den[panel->lcd_dsi_lane-1];

	dphy_dev[sel]->dphy_dbg1.bits.lptx_set_ck =	0x3;
	dphy_dev[sel]->dphy_dbg1.bits.lptx_set_d0 =	0x3;
	dphy_dev[sel]->dphy_dbg1.bits.lptx_set_d1 =	0x3;
	dphy_dev[sel]->dphy_dbg1.bits.lptx_set_d2 =	0x3;
	dphy_dev[sel]->dphy_dbg1.bits.lptx_set_d3 =	0x3;
	dphy_dev[sel]->dphy_dbg1.bits.lptx_dbg_en =	1;
*/
	return 0;
}

u32 dsi_io_open(u32 sel, disp_panel_para *panel)
{
	dphy_dev[sel]->dphy_ana1.bits.reg_vttmode = 0;
	dphy_dev[sel]->dphy_ana0.bits.reg_selsck = 0;
	dphy_dev[sel]->dphy_ana0.bits.reg_pws = 1;
	dphy_dev[sel]->dphy_ana0.bits.reg_sfb = 0;
	dphy_dev[sel]->dphy_ana1.bits.reg_csmps = 1;
	dphy_dev[sel]->dphy_ana0.bits.reg_dmpc = 1;
	dphy_dev[sel]->dphy_ana0.bits.reg_dmpd =
	    dsi_lane_den[panel->lcd_dsi_lane - 1];
	dphy_dev[sel]->dphy_ana0.bits.reg_slv = 0x7;
	dphy_dev[sel]->dphy_ana0.bits.reg_den =
	    dsi_lane_den[panel->lcd_dsi_lane - 1];
	dphy_dev[sel]->dphy_ana1.bits.reg_svtt = 7;
	dphy_dev[sel]->dphy_ana4.bits.reg_ckdv = 0x1;
	dphy_dev[sel]->dphy_ana4.bits.reg_tmsc = 0x1;
	dphy_dev[sel]->dphy_ana4.bits.reg_tmsd = 0x1;
	dphy_dev[sel]->dphy_ana4.bits.reg_txdnsc = 0x1;
	dphy_dev[sel]->dphy_ana4.bits.reg_txdnsd = 0x1;
	dphy_dev[sel]->dphy_ana4.bits.reg_txpusc = 0x1;
	dphy_dev[sel]->dphy_ana4.bits.reg_txpusd = 0x1;
	dphy_dev[sel]->dphy_ana4.bits.reg_dmplvc = 0x1;
	dphy_dev[sel]->dphy_ana4.bits.reg_dmplvd =
	    dsi_lane_den[panel->lcd_dsi_lane - 1];

	dphy_dev[sel]->dphy_ana2.bits.enib = 1;
	dsi_delay_us(5);
	dphy_dev[sel]->dphy_ana3.bits.enldor = 1;
	dphy_dev[sel]->dphy_ana3.bits.enldoc = 1;
	dphy_dev[sel]->dphy_ana3.bits.enldod = 1;
	dsi_delay_us(1);
	dphy_dev[sel]->dphy_ana3.bits.envttc = 1;
	dphy_dev[sel]->dphy_ana3.bits.envttd =
	    dsi_lane_den[panel->lcd_dsi_lane - 1];
	dsi_delay_us(1);
	dphy_dev[sel]->dphy_ana3.bits.endiv = 1;
	dsi_delay_us(1);
	dphy_dev[sel]->dphy_ana2.bits.enck_cpu = 1;
	dsi_delay_us(1);

	dphy_dev[sel]->dphy_ana1.bits.reg_vttmode = 1;
	dphy_dev[sel]->dphy_ana2.bits.enp2s_cpu =
	    dsi_lane_den[panel->lcd_dsi_lane - 1];

	return 0;
}

u32 dsi_io_close(u32 sel)
{
	dphy_dev[sel]->dphy_ana2.bits.enp2s_cpu = 0;
	dphy_dev[sel]->dphy_ana1.bits.reg_vttmode = 0;
	dsi_delay_us(1);
	dphy_dev[sel]->dphy_ana2.bits.enck_cpu = 0;
	dsi_delay_us(1);
	dphy_dev[sel]->dphy_ana3.bits.endiv = 0;
	dsi_delay_us(1);
	dphy_dev[sel]->dphy_ana3.bits.envttd = 0;
	dphy_dev[sel]->dphy_ana3.bits.envttc = 0;
	dsi_delay_us(1);
	dphy_dev[sel]->dphy_ana3.bits.enldod = 0;
	dphy_dev[sel]->dphy_ana3.bits.enldoc = 0;
	dphy_dev[sel]->dphy_ana3.bits.enldor = 0;
	dsi_delay_us(5);
	dphy_dev[sel]->dphy_ana2.bits.enib = 0;

	dphy_dev[sel]->dphy_ana4.bits.reg_dmplvd = 0;
	dphy_dev[sel]->dphy_ana4.bits.reg_dmplvc = 0;
	dphy_dev[sel]->dphy_ana4.bits.reg_txpusd = 0;
	dphy_dev[sel]->dphy_ana4.bits.reg_txpusc = 0;
	dphy_dev[sel]->dphy_ana4.bits.reg_txdnsd = 0;
	dphy_dev[sel]->dphy_ana4.bits.reg_txdnsc = 0;
	dphy_dev[sel]->dphy_ana4.bits.reg_tmsd = 0;
	dphy_dev[sel]->dphy_ana4.bits.reg_tmsc = 0;
	dphy_dev[sel]->dphy_ana4.bits.reg_ckdv = 0;
	dphy_dev[sel]->dphy_ana1.bits.reg_svtt = 0;
	dphy_dev[sel]->dphy_ana0.bits.reg_den = 0;
	dphy_dev[sel]->dphy_ana0.bits.reg_slv = 0;
	dphy_dev[sel]->dphy_ana0.bits.reg_dmpd = 0;
	dphy_dev[sel]->dphy_ana0.bits.reg_dmpc = 0;
	dphy_dev[sel]->dphy_ana1.bits.reg_csmps = 0;
	dphy_dev[sel]->dphy_ana0.bits.reg_sfb = 0;
	dphy_dev[sel]->dphy_ana0.bits.reg_pws = 0;
	dphy_dev[sel]->dphy_ana0.bits.reg_selsck = 0;
	dphy_dev[sel]->dphy_ana1.bits.reg_vttmode = 0;

	return 0;
}

s32 dsi_clk_enable(u32 sel, u32 en)
{
	if (en)
		dsi_start(sel, DSI_START_HSC);

	return 0;
}

static s32 dsi_basic_cfg(u32 sel, disp_panel_para *panel)
{
	if (panel->lcd_dsi_if == LCD_DSI_IF_COMMAND_MODE) {
		dsi_dev[sel]->dsi_basic_ctl0.bits.ecc_en = 1;
		dsi_dev[sel]->dsi_basic_ctl0.bits.crc_en = 1;
		dsi_dev[sel]->dsi_basic_ctl0.bits.hs_eotp_en = 0;
		dsi_dev[sel]->dsi_basic_ctl1.bits.dsi_mode = 0;
		dsi_dev[sel]->dsi_trans_start.bits.trans_start_set = 10;
		dsi_dev[sel]->dsi_trans_zero.bits.hs_zero_reduce_set = 0;
	} else {
		s32 start_delay = panel->lcd_vt - panel->lcd_y - 10;
		u32 vfp = panel->lcd_vt - panel->lcd_y - panel->lcd_vbp;
		u32 dsi_start_delay;

		/*
		 * put start_delay to tcon.
		 * set ready sync early to dramfreq, so set start_delay 1
		 */
		start_delay = 1;

		dsi_start_delay = panel->lcd_vt - vfp + start_delay;
		if (dsi_start_delay > panel->lcd_vt)
			dsi_start_delay -= panel->lcd_vt;
		if (dsi_start_delay == 0)
			dsi_start_delay = 1;

		dsi_dev[sel]->dsi_basic_ctl0.bits.ecc_en = 1;
		dsi_dev[sel]->dsi_basic_ctl0.bits.crc_en = 1;
		dsi_dev[sel]->dsi_basic_ctl0.bits.hs_eotp_en = 0;
		dsi_dev[sel]->dsi_basic_ctl1.bits.video_start_delay =
		    dsi_start_delay;
		dsi_dev[sel]->dsi_basic_ctl1.bits.video_precision_mode_align =
		    1;
		dsi_dev[sel]->dsi_basic_ctl1.bits.video_frame_start = 1;
		dsi_dev[sel]->dsi_trans_start.bits.trans_start_set = 10;
		dsi_dev[sel]->dsi_trans_zero.bits.hs_zero_reduce_set = 0;
		dsi_dev[sel]->dsi_basic_ctl1.bits.dsi_mode = 1;

		if (panel->lcd_dsi_if == LCD_DSI_IF_BURST_MODE) {
			u32 line_num, edge0, edge1, sync_point = 40;

			line_num = panel->lcd_ht
			    * dsi_pixel_bits[panel->lcd_dsi_format]
				/ (8 * panel->lcd_dsi_lane);
			edge1 = sync_point
			    + (panel->lcd_x + panel->lcd_hbp + 20)
			    * dsi_pixel_bits[panel->lcd_dsi_format]
			    / (8 * panel->lcd_dsi_lane);
			edge1 = (edge1 > line_num) ? line_num : edge1;
			edge0 = edge1 + (panel->lcd_x + 40) * tcon_div / 8;
			edge0 = (edge0 > line_num) ? (edge0 - line_num) : 1;
			dsi_dev[sel]->dsi_burst_drq.bits.drq_edge0 = edge0;
			dsi_dev[sel]->dsi_burst_drq.bits.drq_edge1 = edge1;
			dsi_dev[sel]->dsi_tcon_drq.bits.drq_mode = 1;
			dsi_dev[sel]->dsi_burst_line.bits.line_num = line_num;
			dsi_dev[sel]->dsi_burst_line.bits.line_syncpoint =
			    sync_point;

			dsi_dev[sel]->dsi_basic_ctl.bits.video_mode_burst = 1;

			if (panel->lcd_dsi_lane == 4) {
				dsi_dev[sel]->dsi_basic_ctl.bits.trail_inv =
				    0xc;
				dsi_dev[sel]->dsi_basic_ctl.bits.trail_fill = 1;
			}
		} else {
			if ((panel->lcd_ht - panel->lcd_x - panel->lcd_hbp)
			    < 21) {
				dsi_dev[sel]->dsi_tcon_drq.bits.drq_mode = 0;
			} else {
				dsi_dev[sel]->dsi_tcon_drq.bits.drq_set =
				    (panel->lcd_ht - panel->lcd_x -
				     panel->lcd_hbp - 20)
				    * dsi_pixel_bits[panel->lcd_dsi_format] /
				    (8 * 4);
				dsi_dev[sel]->dsi_tcon_drq.bits.drq_mode = 1;
			}
		}
	}
	dsi_dev[sel]->dsi_inst_func[DSI_INST_ID_LP11].bits.instru_mode =
	    DSI_INST_MODE_STOP;
	dsi_dev[sel]->dsi_inst_func[DSI_INST_ID_LP11].bits.lane_cen = 1;
	dsi_dev[sel]->dsi_inst_func[DSI_INST_ID_LP11].bits.lane_den =
	    dsi_lane_den[panel->lcd_dsi_lane - 1];
	dsi_dev[sel]->dsi_inst_func[DSI_INST_ID_TBA].bits.instru_mode =
	    DSI_INST_MODE_TBA;
	dsi_dev[sel]->dsi_inst_func[DSI_INST_ID_TBA].bits.lane_cen = 0;
	dsi_dev[sel]->dsi_inst_func[DSI_INST_ID_TBA].bits.lane_den = 0x1;
	dsi_dev[sel]->dsi_inst_func[DSI_INST_ID_HSC].bits.instru_mode =
	    DSI_INST_MODE_HS;
	dsi_dev[sel]->dsi_inst_func[DSI_INST_ID_HSC].bits.trans_packet =
	    DSI_INST_PACK_PIXEL;
	dsi_dev[sel]->dsi_inst_func[DSI_INST_ID_HSC].bits.lane_cen = 1;
	dsi_dev[sel]->dsi_inst_func[DSI_INST_ID_HSC].bits.lane_den = 0;
	dsi_dev[sel]->dsi_inst_func[DSI_INST_ID_HSD].bits.instru_mode =
	    DSI_INST_MODE_HS;
	dsi_dev[sel]->dsi_inst_func[DSI_INST_ID_HSD].bits.trans_packet =
	    DSI_INST_PACK_PIXEL;
	dsi_dev[sel]->dsi_inst_func[DSI_INST_ID_HSD].bits.lane_cen = 0;
	dsi_dev[sel]->dsi_inst_func[DSI_INST_ID_HSD].bits.lane_den =
	    dsi_lane_den[panel->lcd_dsi_lane - 1];
	dsi_dev[sel]->dsi_inst_func[DSI_INST_ID_LPDT].bits.instru_mode =
	    DSI_INST_MODE_ESCAPE;
	dsi_dev[sel]->dsi_inst_func[DSI_INST_ID_LPDT].bits.escape_enrty =
	    DSI_INST_ESCA_LPDT;
	dsi_dev[sel]->dsi_inst_func[DSI_INST_ID_LPDT].bits.trans_packet =
	    DSI_INST_PACK_COMMAND;
	dsi_dev[sel]->dsi_inst_func[DSI_INST_ID_LPDT].bits.lane_cen = 0;
	dsi_dev[sel]->dsi_inst_func[DSI_INST_ID_LPDT].bits.lane_den = 0x1;
	dsi_dev[sel]->dsi_inst_func[DSI_INST_ID_HSCEXIT].bits.instru_mode =
	    DSI_INST_MODE_HSCEXIT;
	dsi_dev[sel]->dsi_inst_func[DSI_INST_ID_HSCEXIT].bits.lane_cen = 1;
	dsi_dev[sel]->dsi_inst_func[DSI_INST_ID_HSCEXIT].bits.lane_den = 0;
	dsi_dev[sel]->dsi_inst_func[DSI_INST_ID_NOP].bits.instru_mode =
	    DSI_INST_MODE_STOP;
	dsi_dev[sel]->dsi_inst_func[DSI_INST_ID_NOP].bits.lane_cen = 0;
	dsi_dev[sel]->dsi_inst_func[DSI_INST_ID_NOP].bits.lane_den =
	    dsi_lane_den[panel->lcd_dsi_lane - 1];
	dsi_dev[sel]->dsi_inst_func[DSI_INST_ID_DLY].bits.instru_mode =
	    DSI_INST_MODE_NOP;
	dsi_dev[sel]->dsi_inst_func[DSI_INST_ID_DLY].bits.lane_cen = 1;
	dsi_dev[sel]->dsi_inst_func[DSI_INST_ID_DLY].bits.lane_den =
	    dsi_lane_den[panel->lcd_dsi_lane - 1];
	dsi_dev[sel]->dsi_inst_loop_sel.dwval = 2 << (4 * DSI_INST_ID_LP11)
	    | 3 << (4 * DSI_INST_ID_DLY);
	dsi_dev[sel]->dsi_inst_loop_num.bits.loop_n0 = 50 - 1;
	dsi_dev[sel]->dsi_inst_loop_num2.bits.loop_n0 = 50 - 1;
	if (panel->lcd_dsi_if == LCD_DSI_IF_COMMAND_MODE) {
		dsi_dev[sel]->dsi_inst_loop_num.bits.loop_n1 = 1 - 1;
		dsi_dev[sel]->dsi_inst_loop_num2.bits.loop_n1 = 1 - 1;
	} else if (panel->lcd_dsi_if == LCD_DSI_IF_VIDEO_MODE) {
		dsi_dev[sel]->dsi_inst_loop_num.bits.loop_n1 = 50 - 1;
		dsi_dev[sel]->dsi_inst_loop_num2.bits.loop_n1 = 50 - 1;
	} else if (panel->lcd_dsi_if == LCD_DSI_IF_BURST_MODE) {
		dsi_dev[sel]->dsi_inst_loop_num.bits.loop_n1 =
		    (panel->lcd_ht - panel->lcd_x) * (150)
		    / (panel->lcd_dclk_freq * 8) - 50;
		dsi_dev[sel]->dsi_inst_loop_num2.bits.loop_n1 =
		    (panel->lcd_ht - panel->lcd_x) * (150)
		    / (panel->lcd_dclk_freq * 8) - 50;
	}

	if (panel->lcd_dsi_if == LCD_DSI_IF_COMMAND_MODE) {
		dsi_dev[sel]->dsi_inst_jump_cfg[0].bits.jump_cfg_en = 1;
		dsi_dev[sel]->dsi_inst_jump_cfg[0].bits.jump_cfg_num =
		    panel->lcd_y;
	} else {
		dsi_dev[sel]->dsi_inst_jump_cfg[0].bits.jump_cfg_en = 0;
		dsi_dev[sel]->dsi_inst_jump_cfg[0].bits.jump_cfg_num = 1;
	}

	dsi_dev[sel]->dsi_inst_jump_cfg[0].bits.jump_cfg_point =
	    DSI_INST_ID_NOP;
	dsi_dev[sel]->dsi_inst_jump_cfg[0].bits.jump_cfg_to =
	    DSI_INST_ID_HSCEXIT;
	dsi_dev[sel]->dsi_debug_data.bits.test_data = 0xff;
	dsi_dev[sel]->dsi_gctl.bits.dsi_en = 1;
	return 0;
}

static s32 dsi_packet_cfg(u32 sel, disp_panel_para *panel)
{
	if (panel->lcd_dsi_if == LCD_DSI_IF_COMMAND_MODE) {
		dsi_dev[sel]->dsi_pixel_ctl0.bits.pd_plug_dis = 0;
		dsi_dev[sel]->dsi_pixel_ph.bits.vc = panel->lcd_dsi_vc;
		dsi_dev[sel]->dsi_pixel_ph.bits.dt = DSI_DT_DCS_LONG_WR;
		dsi_dev[sel]->dsi_pixel_ph.bits.wc = 1 +
		    panel->lcd_x * dsi_pixel_bits[panel->lcd_dsi_format] / 8;
		dsi_dev[sel]->dsi_pixel_ph.bits.ecc =
		    dsi_ecc_pro(dsi_dev[sel]->dsi_pixel_ph.dwval);
		dsi_dev[sel]->dsi_pixel_pd.bits.pd_tran0 =
		    DSI_DCS_WRITE_MEMORY_START;
		dsi_dev[sel]->dsi_pixel_pd.bits.pd_trann =
		    DSI_DCS_WRITE_MEMORY_CONTINUE;
		dsi_dev[sel]->dsi_pixel_pf0.bits.crc_force = 0xffff;
		dsi_dev[sel]->dsi_pixel_pf1.bits.crc_init_line0 = 0xe4e9;
		dsi_dev[sel]->dsi_pixel_pf1.bits.crc_init_linen = 0xf468;
		dsi_dev[sel]->dsi_pixel_ctl0.bits.pixel_format =
		    panel->lcd_dsi_format;
	} else {
		dsi_dev[sel]->dsi_pixel_ctl0.bits.pd_plug_dis = 1;
		dsi_dev[sel]->dsi_pixel_ph.bits.vc = panel->lcd_dsi_vc;
		dsi_dev[sel]->dsi_pixel_ph.bits.dt =
		    DSI_DT_PIXEL_RGB888 - 0x10 * panel->lcd_dsi_format;
		dsi_dev[sel]->dsi_pixel_ph.bits.wc =
		    panel->lcd_x * dsi_pixel_bits[panel->lcd_dsi_format] / 8;
		dsi_dev[sel]->dsi_pixel_ph.bits.ecc =
		    dsi_ecc_pro(dsi_dev[sel]->dsi_pixel_ph.dwval);
		dsi_dev[sel]->dsi_pixel_pf0.bits.crc_force = 0xffff;
		dsi_dev[sel]->dsi_pixel_pf1.bits.crc_init_line0 = 0xffff;
		dsi_dev[sel]->dsi_pixel_pf1.bits.crc_init_linen = 0xffff;
		dsi_dev[sel]->dsi_pixel_ctl0.bits.pixel_format =
		    8 + panel->lcd_dsi_format;
	}

	if ((panel->lcd_dsi_if == LCD_DSI_IF_VIDEO_MODE)
	    || (panel->lcd_dsi_if == LCD_DSI_IF_BURST_MODE)) {
		u32 dsi_hsa, dsi_hact, dsi_hbp, dsi_hfp, dsi_hblk, dsi_vblk;
		u32 tmp;
		u32 vc = 0;
		u32 x = panel->lcd_x;
		u32 y = panel->lcd_y;
		u32 vt = panel->lcd_vt;
		u32 vbp = panel->lcd_vbp;
		u32 vspw = panel->lcd_vspw;
		u32 ht = panel->lcd_ht;
		u32 hbp = panel->lcd_hbp;
		u32 hspw = panel->lcd_hspw;
		u32 format = panel->lcd_dsi_format;
		u32 lane = panel->lcd_dsi_lane;

		if (panel->lcd_dsi_if == LCD_DSI_IF_BURST_MODE) {
			hspw = 0;
			hbp = 0;
			dsi_hsa = 0;
			dsi_hbp = 0;
			dsi_hact = x * dsi_pixel_bits[format] / 8;
			dsi_hblk = dsi_hact;
			dsi_hfp = 0;
			dsi_vblk = 0;
			dsi_dev[sel]->dsi_basic_ctl.bits.hsa_hse_dis = 1;
			dsi_dev[sel]->dsi_basic_ctl.bits.hbp_dis = 1;
		} else {
			dsi_hsa =
			    hspw * dsi_pixel_bits[format] / 8 - (4 + 4 + 2);
			dsi_hbp = (hbp - hspw) * dsi_pixel_bits[format] / 8
			    - (4 + 2);
			dsi_hact = x * dsi_pixel_bits[format] / 8;
			dsi_hblk = (ht - hspw) * dsi_pixel_bits[format] / 8
			    - (4 + 4 + 2);
			dsi_hfp = dsi_hblk - (4 + dsi_hact + 2)
			    - (4 + dsi_hbp + 2);

			if (lane == 4) {
				tmp = (ht * dsi_pixel_bits[format] / 8) * vt
				    - (4 + dsi_hblk + 2);
				dsi_vblk = (lane - tmp % lane);
			} else {
				dsi_vblk = 0;
			}
		}

		dsi_dev[sel]->dsi_sync_hss.bits.vc = vc;
		dsi_dev[sel]->dsi_sync_hss.bits.dt = DSI_DT_HSS;
		dsi_dev[sel]->dsi_sync_hss.bits.d0 = 0;
		dsi_dev[sel]->dsi_sync_hss.bits.d1 = 0;
		dsi_dev[sel]->dsi_sync_hss.bits.ecc =
		    dsi_ecc_pro(dsi_dev[sel]->dsi_sync_hss.dwval);
		dsi_dev[sel]->dsi_sync_hse.bits.vc = vc;
		dsi_dev[sel]->dsi_sync_hse.bits.dt = DSI_DT_HSE;
		dsi_dev[sel]->dsi_sync_hse.bits.d0 = 0;
		dsi_dev[sel]->dsi_sync_hse.bits.d1 = 0;
		dsi_dev[sel]->dsi_sync_hse.bits.ecc =
		    dsi_ecc_pro(dsi_dev[sel]->dsi_sync_hse.dwval);
		dsi_dev[sel]->dsi_sync_vss.bits.vc = vc;
		dsi_dev[sel]->dsi_sync_vss.bits.dt = DSI_DT_VSS;
		dsi_dev[sel]->dsi_sync_vss.bits.d0 = 0;
		dsi_dev[sel]->dsi_sync_vss.bits.d1 = 0;
		dsi_dev[sel]->dsi_sync_vss.bits.ecc =
		    dsi_ecc_pro(dsi_dev[sel]->dsi_sync_vss.dwval);
		dsi_dev[sel]->dsi_sync_vse.bits.vc = vc;
		dsi_dev[sel]->dsi_sync_vse.bits.dt = DSI_DT_VSE;
		dsi_dev[sel]->dsi_sync_vse.bits.d0 = 0;
		dsi_dev[sel]->dsi_sync_vse.bits.d1 = 0;
		dsi_dev[sel]->dsi_sync_vse.bits.ecc =
		    dsi_ecc_pro(dsi_dev[sel]->dsi_sync_vse.dwval);

		dsi_dev[sel]->dsi_basic_size0.bits.vsa = vspw;
		dsi_dev[sel]->dsi_basic_size0.bits.vbp = vbp - vspw;
		dsi_dev[sel]->dsi_basic_size1.bits.vact = y;
		dsi_dev[sel]->dsi_basic_size1.bits.vt = vt;
		dsi_dev[sel]->dsi_blk_hsa0.bits.vc = vc;
		dsi_dev[sel]->dsi_blk_hsa0.bits.dt = DSI_DT_BLK;
		dsi_dev[sel]->dsi_blk_hsa0.bits.wc = dsi_hsa;
		dsi_dev[sel]->dsi_blk_hsa0.bits.ecc =
		    dsi_ecc_pro(dsi_dev[sel]->dsi_blk_hsa0.dwval);
		dsi_dev[sel]->dsi_blk_hsa1.bits.pd = 0;
		dsi_dev[sel]->dsi_blk_hsa1.bits.pf =
		    dsi_crc_pro_pd_repeat(0, dsi_hsa);
		dsi_dev[sel]->dsi_blk_hbp0.bits.vc = vc;
		dsi_dev[sel]->dsi_blk_hbp0.bits.dt = DSI_DT_BLK;
		dsi_dev[sel]->dsi_blk_hbp0.bits.wc = dsi_hbp;
		dsi_dev[sel]->dsi_blk_hbp0.bits.ecc =
		    dsi_ecc_pro(dsi_dev[sel]->dsi_blk_hbp0.dwval);
		dsi_dev[sel]->dsi_blk_hbp1.bits.pd = 0;
		dsi_dev[sel]->dsi_blk_hbp1.bits.pf =
		    dsi_crc_pro_pd_repeat(0, dsi_hbp);
		dsi_dev[sel]->dsi_blk_hfp0.bits.vc = vc;
		dsi_dev[sel]->dsi_blk_hfp0.bits.dt = DSI_DT_BLK;
		dsi_dev[sel]->dsi_blk_hfp0.bits.wc = dsi_hfp;
		dsi_dev[sel]->dsi_blk_hfp0.bits.ecc =
		    dsi_ecc_pro(dsi_dev[sel]->dsi_blk_hfp0.dwval);
		dsi_dev[sel]->dsi_blk_hfp1.bits.pd = 0;
		dsi_dev[sel]->dsi_blk_hfp1.bits.pf =
		    dsi_crc_pro_pd_repeat(0, dsi_hfp);
		dsi_dev[sel]->dsi_blk_hblk0.bits.dt = DSI_DT_BLK;
		dsi_dev[sel]->dsi_blk_hblk0.bits.wc = dsi_hblk;
		dsi_dev[sel]->dsi_blk_hblk0.bits.ecc =
		    dsi_ecc_pro(dsi_dev[sel]->dsi_blk_hblk0.dwval);
		dsi_dev[sel]->dsi_blk_hblk1.bits.pd = 0;
		dsi_dev[sel]->dsi_blk_hblk1.bits.pf =
		    dsi_crc_pro_pd_repeat(0, dsi_hblk);
		dsi_dev[sel]->dsi_blk_vblk0.bits.dt = DSI_DT_BLK;
		dsi_dev[sel]->dsi_blk_vblk0.bits.wc = dsi_vblk;
		dsi_dev[sel]->dsi_blk_vblk0.bits.ecc =
		    dsi_ecc_pro(dsi_dev[sel]->dsi_blk_vblk0.dwval);
		dsi_dev[sel]->dsi_blk_vblk1.bits.pd = 0;
		dsi_dev[sel]->dsi_blk_vblk1.bits.pf =
		    dsi_crc_pro_pd_repeat(0, dsi_vblk);
	}
	return 0;
}

s32 dsi_cfg(u32 sel, disp_panel_para *panel)
{
	dsi_basic_cfg(sel, panel);
	dsi_packet_cfg(sel, panel);
	dsi_dphy_cfg(sel, panel);
	return 0;
}

s32 dsi_exit(u32 sel)
{
	return 0;
}

u8 dsi_ecc_pro(u32 dsi_ph)
{
	union dsi_ph_t ph;
	ph.bytes.byte012 = dsi_ph;

	ph.bits.bit29 =
	    ph.bits.bit10 ^ ph.bits.bit11 ^ ph.bits.bit12 ^ ph.bits.bit13 ^ ph.
	    bits.bit14 ^ ph.bits.bit15 ^ ph.bits.bit16 ^ ph.bits.bit17 ^ ph.
	    bits.bit18 ^ ph.bits.bit19 ^ ph.bits.bit21 ^ ph.bits.bit22 ^ ph.
	    bits.bit23;
	ph.bits.bit28 =
	    ph.bits.bit04 ^ ph.bits.bit05 ^ ph.bits.bit06 ^ ph.bits.bit07 ^ ph.
	    bits.bit08 ^ ph.bits.bit09 ^ ph.bits.bit16 ^ ph.bits.bit17 ^ ph.
	    bits.bit18 ^ ph.bits.bit19 ^ ph.bits.bit20 ^ ph.bits.bit22 ^ ph.
	    bits.bit23;
	ph.bits.bit27 =
	    ph.bits.bit01 ^ ph.bits.bit02 ^ ph.bits.bit03 ^ ph.bits.bit07 ^ ph.
	    bits.bit08 ^ ph.bits.bit09 ^ ph.bits.bit13 ^ ph.bits.bit14 ^ ph.
	    bits.bit15 ^ ph.bits.bit19 ^ ph.bits.bit20 ^ ph.bits.bit21 ^ ph.
	    bits.bit23;
	ph.bits.bit26 =
	    ph.bits.bit00 ^ ph.bits.bit02 ^ ph.bits.bit03 ^ ph.bits.bit05 ^ ph.
	    bits.bit06 ^ ph.bits.bit09 ^ ph.bits.bit11 ^ ph.bits.bit12 ^ ph.
	    bits.bit15 ^ ph.bits.bit18 ^ ph.bits.bit20 ^ ph.bits.bit21 ^ ph.
	    bits.bit22;
	ph.bits.bit25 =
	    ph.bits.bit00 ^ ph.bits.bit01 ^ ph.bits.bit03 ^ ph.bits.bit04 ^ ph.
	    bits.bit06 ^ ph.bits.bit08 ^ ph.bits.bit10 ^ ph.bits.bit12 ^ ph.
	    bits.bit14 ^ ph.bits.bit17 ^ ph.bits.bit20 ^ ph.bits.bit21 ^ ph.
	    bits.bit22 ^ ph.bits.bit23;
	ph.bits.bit24 =
	    ph.bits.bit00 ^ ph.bits.bit01 ^ ph.bits.bit02 ^ ph.bits.bit04 ^ ph.
	    bits.bit05 ^ ph.bits.bit07 ^ ph.bits.bit10 ^ ph.bits.bit11 ^ ph.
	    bits.bit13 ^ ph.bits.bit16 ^ ph.bits.bit20 ^ ph.bits.bit21 ^ ph.
	    bits.bit22 ^ ph.bits.bit23;
	return ph.bytes.byte3;
}

u16 dsi_crc_pro(u8 *pd_p, u32 pd_bytes)
{
	u16 gen_code = 0x8408;
	u16 byte_cntr;
	u8 bit_cntr;
	u8 curr_data;
	u16 crc = 0xffff;

	if (pd_bytes > 0) {
		for (byte_cntr = 0; byte_cntr < pd_bytes; byte_cntr++) {
			curr_data = *(pd_p + byte_cntr);
			for (bit_cntr = 0; bit_cntr < 8; bit_cntr++) {
				if (((crc & 0x0001) ^
				     ((0x0001 * curr_data) & 0x0001)) > 0)
					crc = ((crc >> 1) & 0x7fff) ^ gen_code;
				else
					crc = (crc >> 1) & 0x7fff;
				curr_data = (curr_data >> 1) & 0x7f;
			}
		}
	}
	return crc;
}

u16 dsi_crc_pro_pd_repeat(u8 pd, u32 pd_bytes)
{
	u16 gen_code = 0x8408;
	u16 byte_cntr;
	u8 bit_cntr;
	u8 curr_data;
	u16 crc = 0xffff;

	if (pd_bytes > 0) {
		for (byte_cntr = 0; byte_cntr < pd_bytes; byte_cntr++) {
			curr_data = pd;
			for (bit_cntr = 0; bit_cntr < 8; bit_cntr++) {
				if (((crc & 0x0001) ^
				     ((0x0001 * curr_data) & 0x0001)) > 0)
					crc = ((crc >> 1) & 0x7fff) ^ gen_code;
				else
					crc = (crc >> 1) & 0x7fff;
				curr_data = (curr_data >> 1) & 0x7f;
			}
		}
	}
	return crc;
}

#elif defined(SUPPORT_DSI) && defined(DSI_VERSION_28)

#include "de_dsi_type.h"
#include "de_dsi.h"

extern s32 disp_delay_us(u32 us);
extern s32 disp_delay_ms(u32 ms);
u32  dsi_bits_per_pixel[4] = {24, 24, 18, 16};
u32 dsi_pixel_bits[4] = { 24, 24, 18, 16 };
u32 dsi_lane_den[4] = { 0x1, 0x3, 0x7, 0xf };
u32 tcon_div = 4;

static volatile __de_dsi_dev_t *dsi_dev[2];
__s32 dsi_hs_clk(__u32 sel,__u32 on_off);

s32 dsi_delay_ms(u32 ms)
{
	disp_delay_ms(ms);

	return 0;
}

s32 dsi_delay_us(u32 us)
{
	disp_delay_us(us);

	return 0;
}

__s32 dsi_set_reg_base(__u32 sel, __u32 base)
{
	dsi_dev[sel]=(__de_dsi_dev_t *)(base);

	return 0;
}

__u32 dsi_get_reg_base(__u32 sel)
{
	return (__u32)dsi_dev[sel];
}

__u32 dsi_get_start_delay(__u32 sel)
{
	return 0;
}

__u32 dsi_get_cur_line(__u32 sel)
{
	return 0;
}

__s32 dsi_irq_enable(__u32 sel,enum __dsi_irq_id_t id)
{
	if(id<32)
	{
		dsi_dev[sel]->dsi_irq_en0.dwval &= ~(1<<id);
	}
	else
	{
		dsi_dev[sel]->dsi_irq_en1.dwval &= ~(1<<(id-32));
	}
	return 0;
}

__s32 dsi_irq_disable(__u32 sel,enum __dsi_irq_id_t id)
{
	if(id<32)
	{
		dsi_dev[sel]->dsi_irq_en0.dwval |= (1<<id);
	}
	else
	{
		dsi_dev[sel]->dsi_irq_en1.dwval |= (1<<(id-32));
	}
    return 0;
}

static __s32 dsi_irq_disable_all(__u32 sel)
{
	dsi_dev[sel]->dsi_irq_en0.dwval = 0xffffffff;
	dsi_dev[sel]->dsi_irq_en1.dwval = 0xffffffff;
	return 0;
}

__u32 dsi_irq_query(__u32 sel,enum __dsi_irq_id_t id)
{
	return 0;
}

__s32 dsi_inst_busy(__u32 sel)
{
	return 0;
}

__s32 dsi_start(__u32 sel,enum __dsi_start_t func)
{
	return 0;
}

__s32 dsi_open(__u32 sel,disp_panel_para * panel)
{
	dsi_dev[sel]->dsi_cmd_ctl.bits.cmd_mode_en		  = 0;
	dsi_dev[sel]->dsi_vid_ctl0.bits.video_mode_en	  = 1;
	dsi_dev[sel]->dsi_cfg1.bits.dpi_src = 1;

	return 0;
}

__s32 dsi_close(__u32 sel)
{
	dsi_dev[sel]->dsi_cfg1.bits.dpi_src = 0;
	//FIXME
	//LCD_delay_fs(sel, 1);
	dsi_delay_ms(16);
	dsi_dev[sel]->dsi_vid_ctl0.bits.video_mode_en	  = 0;
	dsi_dev[sel]->dsi_cmd_ctl.bits.cmd_mode_en		  = 1;
	dsi_hs_clk(sel,0);
	return 0;
}

__s32 dsi_tri_start(__u32 sel)
{
	return 0;
}

 __s32 dsi_dcs_wr(__u32 sel,__u8 cmd,__u8* para_p,__u32 para_num)
{
	__u8 dt,data0,data1;
	__u8 vc = 0;

	switch(para_num)
	{
	case 0:
		dt		= DSI_DT_DCS_WR_P0;
		data0	= cmd;
		data1	= 0;
		break;
	case 1:
		dt		= DSI_DT_DCS_WR_P1;
		data0	= cmd;
		data1 	= *para_p;
		break;
	default:
		dt		= DSI_DT_DCS_LONG_WR;
		data0	= ((para_num+1)>>0) & 0xff;
		data1 	= ((para_num+1)>>8) & 0xff;
		break;
	}

	if(para_num>1)
	{
		__u32 i=0;
		dsi_dev[sel]->dsi_pkg_ctl1.dwval = *(para_p+i+2) << 24 |
									       *(para_p+i+1) << 16 |
									       *(para_p+i+0) <<  8 |
									         cmd		 <<  0 ;
		i+=3;
		while(i<para_num)
		{
			dsi_dev[sel]->dsi_pkg_ctl1.dwval = *(para_p+i+3) << 24 |
											   *(para_p+i+2) << 16 |
											   *(para_p+i+1) <<  8 |
											   *(para_p+i+0) <<  0 ;
			i+=4;
		};
	}

	dsi_dev[sel]->dsi_pkg_ctl0.dwval = (data1<<16 | data0<<8 | vc<<6 | dt);
	dsi_delay_us(20 + para_num);
	return 0;
}

__s32 dsi_dcs_wr_index(__u32 sel,__u8 index)
{

	return 0;
}

__s32 dsi_dcs_wr_data(__u32 sel,__u8 data)
{

	return 0;
}

__s32 dsi_dcs_wr_0para(__u32 sel,__u8 cmd)
{
	__u8 tmp;
	dsi_dcs_wr(0,cmd,&tmp,0);
	return 0;
}

__s32 dsi_dcs_wr_1para(__u32 sel,__u8 cmd,__u8 para)
{
	__u8 tmp = para;
	dsi_dcs_wr(0,cmd,&tmp,1);
	return 0;
}

__s32 dsi_dcs_wr_2para(__u32 sel,__u8 cmd,__u8 para1,__u8 para2)
{
	__u8 tmp[2];
	tmp[0] = para1;
	tmp[1] = para2;
	dsi_dcs_wr(0,cmd,tmp,2);
	return 0;
}

__s32 dsi_dcs_wr_3para(__u32 sel,__u8 cmd,__u8 para1,__u8 para2,__u8 para3)
{
	__u8 tmp[3];
	tmp[0] = para1;
	tmp[1] = para2;
	tmp[2] = para3;
	dsi_dcs_wr(0,cmd,tmp,3);
	return 0;
}
__s32 dsi_dcs_wr_4para(__u32 sel,__u8 cmd,__u8 para1,__u8 para2,__u8 para3,__u8 para4)
{
	__u8 tmp[4];
	tmp[0] = para1;
	tmp[1] = para2;
	tmp[2] = para3;
	tmp[3] = para4;
	dsi_dcs_wr(0,cmd,tmp,4);
	return 0;
}

__s32 dsi_dcs_wr_5para(__u32 sel,__u8 cmd,__u8 para1,__u8 para2,__u8 para3,__u8 para4,__u8 para5)
{
	__u8 tmp[5];
	tmp[0] = para1;
	tmp[1] = para2;
	tmp[2] = para3;
	tmp[3] = para4;
	tmp[4] = para5;
	dsi_dcs_wr(0,cmd,tmp,5);
	return 0;
}

__s32 dsi_dcs_wr_memory(__u32 sel,__u32* p_data,__u32 length)
{
	__u32 tx_cntr=length;
	__u32 tx_num;
	__u32* tx_p=p_data;
	__u8 para[256];
	__u32 i;
	__u32 start=1;

	while(tx_cntr)
	{
		if(tx_cntr>=83)
			tx_num = 83;
		else
			tx_num = tx_cntr;
		tx_cntr -= tx_num;

		for(i=0;i<tx_num;i++)
		{
			para[i*3+0] = (*tx_p >> 16) & 0xff;
			para[i*3+1] = (*tx_p >> 8)  & 0xff;
			para[i*3+2] = (*tx_p >> 0)  & 0xff;
			tx_p++;
		}

		if(start)
		{
			dsi_dcs_wr(sel,DSI_DCS_WRITE_MEMORY_START,para,tx_num*3);
			start = 0;
		}
		else
		{
			dsi_dcs_wr(sel,DSI_DCS_WRITE_MEMORY_CONTINUE,para,tx_num*3);
		}
	}
	return 0;
}

__s32 dsi_gen_wr(__u32 sel,__u8 cmd,__u8* para_p,__u32 para_num)
{
	__u8 dt,data0,data1;
	switch(para_num)
	{
	case 0:
		dt		= DSI_DT_GEN_WR_P1;
		data0	= cmd;
		data1	= 0;
		break;
	case 1:
		dt		= DSI_DT_GEN_WR_P2;
		data0	= cmd;
		data1 	= *para_p;
		break;
	default:
		dt		= DSI_DT_GEN_LONG_WR;
		data0	= (para_num>>0) & 0xff;
		data1 	= (para_num>>8) & 0xff;
		break;
	}

	if(para_num>1)
	{
		__u32 i=0;
		dsi_dev[sel]->dsi_pkg_ctl1.dwval = *(para_p+i+2) << 24 |
									       *(para_p+i+1) << 16 |
									       *(para_p+i+0) <<  8 |
									         cmd		 <<  0 ;
		i+=3;
		while(i<para_num)
		{
			dsi_dev[sel]->dsi_pkg_ctl1.dwval = *(para_p+i+3) << 24 |
											   *(para_p+i+2) << 16 |
											   *(para_p+i+1) <<  8 |
											   *(para_p+i+0) <<  0 ;
			i+=4;
		};
	}

	dsi_dev[sel]->dsi_pkg_ctl0.dwval = (data1<<16 | data0<<8 | dt);
	dsi_delay_us(20 + para_num);
	return 0;
}

__s32 dsi_gen_wr_0para(__u32 sel,__u8 cmd)
{
	__u8 tmp;
	dsi_gen_wr(sel,cmd,&tmp,0);
	return 0;
}

__s32 dsi_gen_wr_1para(__u32 sel,__u8 cmd,__u8 para)
{
	__u8 tmp = para;
	dsi_gen_wr(sel,cmd,&tmp,1);
	return 0;
}

__s32 dsi_gen_wr_2para(__u32 sel,__u8 cmd,__u8 para1,__u8 para2)
{
	__u8 tmp[2];
	tmp[0] = para1;
	tmp[1] = para2;
	dsi_gen_wr(sel,cmd,tmp,2);
	return 0;
}

__s32 dsi_gen_wr_3para(__u32 sel,__u8 cmd,__u8 para1,__u8 para2,__u8 para3)
{
	__u8 tmp[3];
	tmp[0] = para1;
	tmp[1] = para2;
	tmp[2] = para3;
	dsi_gen_wr(sel,cmd,tmp,3);
	return 0;
}
__s32 dsi_gen_wr_4para(__u32 sel,__u8 cmd,__u8 para1,__u8 para2,__u8 para3,__u8 para4)
{
	__u8 tmp[4];
	tmp[0] = para1;
	tmp[1] = para2;
	tmp[2] = para3;
	tmp[3] = para4;
	dsi_gen_wr(sel,cmd,tmp,4);
	return 0;
}

__s32 dsi_gen_wr_5para(__u32 sel,__u8 cmd,__u8 para1,__u8 para2,__u8 para3,__u8 para4,__u8 para5)
{
	__u8 tmp[5];
	tmp[0] = para1;
	tmp[1] = para2;
	tmp[2] = para3;
	tmp[3] = para4;
	tmp[4] = para5;
	dsi_gen_wr(sel,cmd,tmp,5);
	return 0;
}

__s32 dsi_set_max_ret_size(__u32 sel,__u32 size)
{
	return 0;
}

__s32 dsi_dcs_rd(__u32 sel,__u8 cmd,__u8* para_p,__u32* num_p)
{
	return 0;
}


__s32 dsi_dcs_rd_memory(__u32 sel,__u32* p_data,__u32 length)
{
	return 0;
}


__s32 dsi_hs_clk(__u32 sel,__u32 on_off)
{
	if(on_off)
	{
		if(!dsi_dev[sel]->dsi_phy_status.bits.phy_ck_stop) {
			dsi_delay_ms(5);
			if(!dsi_dev[sel]->dsi_phy_status.bits.phy_ck_stop)
				__wrn("dsi clk enable error\n");
		}

		dsi_dev[sel]->dsi_ctl1.bits.phy_clk_lane_enable 	= 1;
	}
	else
	{
		dsi_dev[sel]->dsi_ctl1.bits.phy_clk_lane_enable 	= 0;
		if(!dsi_dev[sel]->dsi_phy_status.bits.phy_ck_stop) {
			dsi_delay_ms(5);
			if(!dsi_dev[sel]->dsi_phy_status.bits.phy_ck_stop)
				__wrn("dsi clk disable error\n");
		}
	}
	return 0;
}

__s32 dsi_dphy_cfg_0data(__u32 sel,__u32 code)
{
	dsi_delay_us(5);
	dsi_dev[sel]->dsi_phy_ctl3.bits.phy_cfg_en	= 1;
	dsi_delay_us(1);
	dsi_dev[sel]->dsi_phy_ctl3.bits.phy_cfg_clk	= 1;
	dsi_delay_us(1);
	//dsi_dev[sel]->dsi_phy_ctl3.bits.phy_cfg_din = code;
	dsi_dev[sel]->dsi_phy_ctl3.dwval = ((dsi_dev[sel]->dsi_phy_ctl3.dwval) & (~0x00ff0000)) | (code << 16);
	dsi_delay_us(1);
	dsi_dev[sel]->dsi_phy_ctl3.bits.phy_cfg_clk	= 0;
	dsi_delay_us(1);
	dsi_dev[sel]->dsi_phy_ctl3.bits.phy_cfg_en		= 0;
	dsi_delay_us(5);
	return 0;
}

__s32 dsi_dphy_cfg_1data(__u32 sel,__u32 code,__u32 data)
{
	dsi_delay_us(5);
	dsi_dev[sel]->dsi_phy_ctl3.bits.phy_cfg_en	= 1;
	dsi_delay_us(1);
	dsi_dev[sel]->dsi_phy_ctl3.bits.phy_cfg_clk	= 1;
	dsi_delay_us(1);
	//dsi_dev[sel]->dsi_phy_ctl3.bits.phy_cfg_din = code;
	dsi_dev[sel]->dsi_phy_ctl3.dwval = ((dsi_dev[sel]->dsi_phy_ctl3.dwval) & (~0x00ff0000)) | (code << 16);
	dsi_delay_us(1);
	dsi_dev[sel]->dsi_phy_ctl3.bits.phy_cfg_clk	= 0;
	dsi_delay_us(1);
	dsi_dev[sel]->dsi_phy_ctl3.bits.phy_cfg_en	= 0;
	dsi_delay_us(1);
	//dsi_dev[sel]->dsi_phy_ctl3.bits.phy_cfg_din = data;
	dsi_dev[sel]->dsi_phy_ctl3.dwval = ((dsi_dev[sel]->dsi_phy_ctl3.dwval) & (~0x00ff0000)) | (data << 16);
	dsi_delay_us(1);
	dsi_dev[sel]->dsi_phy_ctl3.bits.phy_cfg_clk	= 1;
	dsi_delay_us(1);
	dsi_dev[sel]->dsi_phy_ctl3.bits.phy_cfg_clk	= 0;
	dsi_delay_us(5);
	return 0;
}

__s32 dsi_dphy_cfg_2data(__u32 sel,__u32 code,__u32 data0,__u32 data1)
{
	dsi_delay_us(5);
	dsi_dev[sel]->dsi_phy_ctl3.bits.phy_cfg_en		= 1;
	dsi_delay_us(1);
	dsi_dev[sel]->dsi_phy_ctl3.bits.phy_cfg_clk	= 1;
	dsi_delay_us(1);
	//dsi_dev[sel]->dsi_phy_ctl3.bits.phy_cfg_din = code;
	dsi_dev[sel]->dsi_phy_ctl3.dwval = ((dsi_dev[sel]->dsi_phy_ctl3.dwval) & (~0x00ff0000)) | (code << 16);
	dsi_delay_us(1);
	dsi_dev[sel]->dsi_phy_ctl3.bits.phy_cfg_clk	= 0;
	dsi_delay_us(1);
	dsi_dev[sel]->dsi_phy_ctl3.bits.phy_cfg_en		= 0;
	dsi_delay_us(1);
	//dsi_dev[sel]->dsi_phy_ctl3.bits.phy_cfg_din = data0;
	dsi_dev[sel]->dsi_phy_ctl3.dwval = ((dsi_dev[sel]->dsi_phy_ctl3.dwval) & (~0x00ff0000)) | (data0 << 16);
	dsi_delay_us(1);
	dsi_dev[sel]->dsi_phy_ctl3.bits.phy_cfg_clk	= 1;
	dsi_delay_us(1);
	dsi_dev[sel]->dsi_phy_ctl3.bits.phy_cfg_clk	= 0;
	dsi_delay_us(1);
	//dsi_dev[sel]->dsi_phy_ctl3.bits.phy_cfg_din = data1;
	dsi_dev[sel]->dsi_phy_ctl3.dwval = ((dsi_dev[sel]->dsi_phy_ctl3.dwval) & (~0x00ff0000)) | (data1 << 16);
	dsi_delay_us(1);
	dsi_dev[sel]->dsi_phy_ctl3.bits.phy_cfg_clk	= 1;
	dsi_delay_us(1);
	dsi_dev[sel]->dsi_phy_ctl3.bits.phy_cfg_clk	= 0;
	dsi_delay_us(5);
	return 0;
}

static __s32 dsi_dphy_cfg_hsfrq(__u32 sel,__u32 hs_frq)
{
	__u32 hs_frq_set;
	__u32 hs_frq_tmp;

	hs_frq_tmp = hs_frq;

	if     ( (hs_frq_tmp >=   80) && (hs_frq_tmp <  90) )	hs_frq_set = 0x00;	//b00_0000;
	else if( (hs_frq_tmp >=   90) && (hs_frq_tmp < 100) )  	hs_frq_set = 0x10;	//b01_0000;
	else if( (hs_frq_tmp >=  100) && (hs_frq_tmp < 110) ) 	hs_frq_set = 0x20;	//b10_0000;
	else if( (hs_frq_tmp >=  110) && (hs_frq_tmp < 130) ) 	hs_frq_set = 0x01;	//b00_0001;
	else if( (hs_frq_tmp >=  130) && (hs_frq_tmp < 140) ) 	hs_frq_set = 0x11;	//b01_0001;
	else if( (hs_frq_tmp >=  140) && (hs_frq_tmp < 150) ) 	hs_frq_set = 0x21;	//b10_0001;
	else if( (hs_frq_tmp >=  150) && (hs_frq_tmp < 170) ) 	hs_frq_set = 0x02;	//b00_0010;
	else if( (hs_frq_tmp >=  170) && (hs_frq_tmp < 180) ) 	hs_frq_set = 0x12;	//b01_0010;
	else if( (hs_frq_tmp >=  180) && (hs_frq_tmp < 200) ) 	hs_frq_set = 0x22;	//b10_0010;
	else if( (hs_frq_tmp >=  200) && (hs_frq_tmp < 220) ) 	hs_frq_set = 0x03;	//b00_0011;
	else if( (hs_frq_tmp >=  220) && (hs_frq_tmp < 240) ) 	hs_frq_set = 0x13;	//b01_0011;
	else if( (hs_frq_tmp >=  240) && (hs_frq_tmp < 250) ) 	hs_frq_set = 0x23;	//b10_0011;
	else if( (hs_frq_tmp >=  250) && (hs_frq_tmp < 270) ) 	hs_frq_set = 0x04;	//b00_0100;
	else if( (hs_frq_tmp >=  270) && (hs_frq_tmp < 300) ) 	hs_frq_set = 0x14;	//b01_0100;
	else if( (hs_frq_tmp >=  300) && (hs_frq_tmp < 330) ) 	hs_frq_set = 0x05;	//b00_0101;
	else if( (hs_frq_tmp >=  330) && (hs_frq_tmp < 360) ) 	hs_frq_set = 0x15;	//b01_0101;
	else if( (hs_frq_tmp >=  360) && (hs_frq_tmp < 400) ) 	hs_frq_set = 0x25;	//b10_0101;
	else if( (hs_frq_tmp >=  400) && (hs_frq_tmp < 450) ) 	hs_frq_set = 0x06;	//b00_0110;
	else if( (hs_frq_tmp >=  450) && (hs_frq_tmp < 500) ) 	hs_frq_set = 0x16;	//b01_0110;
	else if( (hs_frq_tmp >=  500) && (hs_frq_tmp < 550) ) 	hs_frq_set = 0x07;	//b00_0111;
	else if( (hs_frq_tmp >=  550) && (hs_frq_tmp < 600) ) 	hs_frq_set = 0x17;	//b01_0111;
	else if( (hs_frq_tmp >=  600) && (hs_frq_tmp < 650) ) 	hs_frq_set = 0x08;	//b00_1000;
	else if( (hs_frq_tmp >=  650) && (hs_frq_tmp < 700) ) 	hs_frq_set = 0x18;	//b01_1000;
	else if( (hs_frq_tmp >=  700) && (hs_frq_tmp < 750) ) 	hs_frq_set = 0x09;	//b00_1001;
	else if( (hs_frq_tmp >=  750) && (hs_frq_tmp < 800) ) 	hs_frq_set = 0x19;	//b01_1001;
	else if( (hs_frq_tmp >=  800) && (hs_frq_tmp < 850) ) 	hs_frq_set = 0x29;	//b10_1001;
	else if( (hs_frq_tmp >=  850) && (hs_frq_tmp < 900) ) 	hs_frq_set = 0x39;	//b11_1001;
	else if( (hs_frq_tmp >=  900) && (hs_frq_tmp < 950) ) 	hs_frq_set = 0x0a;	//b00_1010;
	else if( (hs_frq_tmp >=  950) && (hs_frq_tmp <1000) ) 	hs_frq_set = 0x1a;	//b01_1010;
	else if( (hs_frq_tmp >= 1000) && (hs_frq_tmp <1050) ) 	hs_frq_set = 0x2a;	//b10_1010;
	else if( (hs_frq_tmp >= 1050) && (hs_frq_tmp <1100) ) 	hs_frq_set = 0x3a;	//b11_1010;
	else if( (hs_frq_tmp >= 1100) && (hs_frq_tmp <1150) ) 	hs_frq_set = 0x0b;	//b00_1011;
	else if( (hs_frq_tmp >= 1150) && (hs_frq_tmp <1200) ) 	hs_frq_set = 0x1b;	//b01_1011;
	else if( (hs_frq_tmp >= 1200) && (hs_frq_tmp <1250) ) 	hs_frq_set = 0x2b;	//b10_1011;
	else if( (hs_frq_tmp >= 1250) && (hs_frq_tmp <1300) ) 	hs_frq_set = 0x3b;	//b11_1011;
	else if( (hs_frq_tmp >= 1300) && (hs_frq_tmp <1350) ) 	hs_frq_set = 0x0c;	//b00_1100;
	else if( (hs_frq_tmp >= 1350) && (hs_frq_tmp <1400) ) 	hs_frq_set = 0x1c;	//b01_1100;
	else if( (hs_frq_tmp >= 1400) && (hs_frq_tmp <1450) ) 	hs_frq_set = 0x2c;	//b10_1100;
	else if( (hs_frq_tmp >= 1450) && (hs_frq_tmp <1500) ) 	hs_frq_set = 0x3c;	//b11_1100;
	else													hs_frq_set = 0x00;	//b00_0000;

	dsi_dphy_cfg_1data(sel,0x44,hs_frq_set<<1);
	return 0;
}

static __s32 dsi_dphy_cfg_pll(__u32 sel,__u32 m,__u32 n)
{
	dsi_dphy_cfg_1data(sel,0x19,0x30);
	dsi_dphy_cfg_1data(sel,0x17,n-1);
	dsi_dphy_cfg_1data(sel,0x18,0x7f & (((m-1)>>0) & 0x1f) );
	dsi_dphy_cfg_1data(sel,0x18,0x80 | (((m-1)>>5) & 0x0f) );
	return 0;
}

static __s32 dsi_dphy_cfg(__u32 sel,disp_panel_para * panel)
{
	/*Input sequence:
	1.Set RSTZ = 1'b0.
	2.Set SHUTDOWNZ = 1'b0.
	3.Set TESTCLEAR = 1'b1.
	4.Set MASTERSLAVEZ = 1'b1 (for MASTER) / 1'b0 (for SLAVE).
	5.Set BASEDIRX to the desired values.
	6.Set all REQUEST inputs to zero.
	7.Wait for t > Tmin.
	8.Set RSTZ = 1'b1.
	9.Set SHUTDOWNZ = 1'b1.
	10.Set TESTCLEAR = 1'b0.
	*/

	/*
	dsi_dev[sel]->dsi_phy_ctl3.bits.phy_cfg_clr       	= 1;
	dsi_dev[sel]->dsi_ctl1.bits.phy_rst     			= 0;
	dsi_dev[sel]->dsi_ctl1.bits.phy_en  			= 0;
	delayms(1);//?
	dsi_dev[sel]->dsi_phy_ctl3.bits.phy_cfg_clr       	= 0;

	dsi_dphy_cfg_hsfrq(sel,dclk*dsi_bits_per_pixel[format]/lane);
	dsi_dphy_cfg_pll(sel,dsi_bits_per_pixel[format]*2,lane*2);
	dsi_dphy_cfg_0data(sel,0x00);

	dsi_dev[sel]->dsi_ctl1.bits.phy_rst     			= 1;
	dsi_dev[sel]->dsi_ctl1.bits.phy_en  			= 1;
	*/

	__u32 mode, lane, format, dclk;

	mode	= panel->lcd_dsi_if;
	lane	= panel->lcd_dsi_lane;
	format	= panel->lcd_dsi_format;
	dclk	= panel->lcd_dclk_freq;

	dsi_dev[sel]->dsi_cfg3.bits.reg_rext             	=	0;
	dsi_dev[sel]->dsi_cfg3.bits.reg_snk      			=	2;
	dsi_dev[sel]->dsi_cfg3.bits.reg_rint				=	2;
	dsi_delay_ms(1);
	dsi_dev[sel]->dsi_cfg3.bits.reg_rext               	=	1;
	dsi_delay_ms(1);

	dsi_dev[sel]->dsi_phy_ctl3.bits.phy_cfg_clr       	= 1;
	dsi_delay_ms(1);
	dsi_dev[sel]->dsi_ctl1.bits.phy_rst     			= 0;
	dsi_delay_ms(1);
	dsi_dev[sel]->dsi_ctl1.bits.phy_en  				= 0;
	dsi_delay_ms(1);
	dsi_dev[sel]->dsi_phy_ctl3.bits.phy_cfg_clr       	= 0;
	dsi_delay_ms(1);

	dsi_dphy_cfg_1data(sel, 0x20, 0xcd);
	dsi_dphy_cfg_1data(sel, 0x22, 0x42);

	if(LCD_DSI_IF_VIDEO_MODE==mode || LCD_DSI_IF_BURST_MODE==mode)
	{
		dsi_dphy_cfg_hsfrq(sel,dclk*dsi_bits_per_pixel[format]/lane);
		dsi_dphy_cfg_pll(sel,dsi_bits_per_pixel[format]*2,lane*2);
		dsi_dphy_cfg_0data(sel,0x00);
	}
	else
	{
		__u32 n = dclk*6/30;
		dsi_dphy_cfg_hsfrq(sel,dclk*36);
		dsi_dphy_cfg_pll(sel,6*n,n);
		dsi_dphy_cfg_0data(sel,0x00);
	}

	dsi_delay_ms(1);
	dsi_dev[sel]->dsi_ctl1.bits.phy_en  			= 1;
	dsi_delay_ms(1);
	dsi_dev[sel]->dsi_ctl1.bits.phy_rst     		= 1;
	dsi_delay_ms(1);
	//dsi_dev[sel]->dsi_phy_ctl1.bits.phy_stop_set	= 10;
	dsi_dev[sel]->dsi_phy_ctl1.dwval = ((dsi_dev[sel]->dsi_phy_ctl1.dwval) & (~0xff)) | 10;
	dsi_dev[sel]->dsi_ctl1.bits.phy_lane_num        = lane-1;
	dsi_delay_ms(1);
	dsi_dev[sel]->dsi_ctl1.bits.phy_clk_gating		= 1;
	dsi_delay_ms(1);

	return 0;
}


__u32 dsi_io_open(__u32 sel,disp_panel_para * panel)
{
	dsi_dphy_cfg(sel,panel);
	return 0;
}

__u32 dsi_io_close(__u32 sel)
{
	//modify next time
	dsi_dev[sel]->dsi_ctl1.bits.phy_clk_gating			= 0;
	dsi_delay_ms(1);
	dsi_dev[sel]->dsi_ctl1.bits.phy_rst     			= 0;
	dsi_delay_ms(1);
	dsi_dev[sel]->dsi_ctl1.bits.phy_en  				= 0;
	dsi_delay_ms(1);
	dsi_dev[sel]->dsi_phy_ctl3.bits.phy_cfg_clr       	= 0;
	dsi_delay_ms(1);

	return 0;
}

__s32 dsi_clk_enable(__u32 sel, __u32 en)
{
	dsi_hs_clk(sel,en);
	return 0;
}

static __s32 dsi_basic_cfg(__u32 sel,disp_panel_para * panel)
{
	__u32 mode, lane, format, dclk;

	mode	= panel->lcd_dsi_if;
	lane	= panel->lcd_dsi_lane;
	format	= panel->lcd_dsi_format;
	dclk	= panel->lcd_dclk_freq;

	dsi_dev[sel]->dsi_ctl0.bits.module_en 				= 1;
	if(LCD_DSI_IF_VIDEO_MODE==mode || LCD_DSI_IF_BURST_MODE==mode)
	{
		//dsi_dev[sel]->dsi_ctl2.bits.lp_clk_div 	= (dclk*dsi_bits_per_pixel[format]+4*lane*10)/(8*lane*10);
		dsi_dev[sel]->dsi_ctl2.dwval = ((dclk*dsi_bits_per_pixel[format]+4*lane*10)/(8*lane*10)) & 0xff;
		dsi_delay_ms(100);
		//dsi_dev[sel]->dsi_to_ctl0.bits.to_clk_div 		= (dclk*dsi_bits_per_pixel[format]+4*lane*10)/(8*lane*10);
		dsi_dev[sel]->dsi_to_ctl0.dwval 		= ((dclk*dsi_bits_per_pixel[format]+4*lane*10)/(8*lane*10)) & 0xff;
	}
	else
	{
		//dsi_dev[sel]->dsi_ctl2.bits.lp_clk_div 	= (dclk*6*dsi_bits_per_pixel[format]+4*lane*10)/(8*lane*10);
		dsi_dev[sel]->dsi_ctl2.dwval = ((dclk*6*dsi_bits_per_pixel[format]+4*lane*10)/(8*lane*10)) & 0xff;
		dsi_delay_ms(100);
		//dsi_dev[sel]->dsi_to_ctl0.bits.to_clk_div 		= (dclk*6*dsi_bits_per_pixel[format]+4*lane*10)/(8*lane*10);
		dsi_dev[sel]->dsi_to_ctl0.dwval = ((dclk*6*dsi_bits_per_pixel[format]+4*lane*10)/(8*lane*10)) & 0xff;
	}
	dsi_dev[sel]->dsi_pkg_ctl2.bits.eotp_tx_en			= 0;
	dsi_dev[sel]->dsi_pkg_ctl2.bits.eotp_rx_en			= 0;
	dsi_dev[sel]->dsi_pkg_ctl2.bits.bta_en    			= 0;
	dsi_dev[sel]->dsi_pkg_ctl2.bits.ecc_rx_en 			= 1;
	dsi_dev[sel]->dsi_pkg_ctl2.bits.crc_rx_en 			= 1;
	dsi_dev[sel]->dsi_pkg_ctl2.bits.vid_rx				= 1;

	//dsi_dev[sel]->dsi_vid_ctl2.bits.lpcmd_time_invact 	= 64;
	//dsi_dev[sel]->dsi_vid_ctl2.bits.lpcmd_time_outvact  = 0;
	dsi_dev[sel]->dsi_vid_ctl2.dwval = 0x00400000;


	//dsi_dev[sel]->dsi_phy_ctl1.bits.max_rd_set     		= 50;
	dsi_dev[sel]->dsi_phy_ctl1.dwval = ((dsi_dev[sel]->dsi_phy_ctl1.dwval) & (~0x7fff0000)) | (50 << 16);
	//dsi_dev[sel]->dsi_phy_ctl0.bits.phy_lp2hs_set	  	= 20;
	//dsi_dev[sel]->dsi_phy_ctl0.bits.phy_hs2lp_set	    = 20;
	dsi_dev[sel]->dsi_phy_ctl0.dwval = 0x00140014;

	//dsi_dev[sel]->dsi_to_ctl1.bits.to_hstx_set      	= 0;
	//dsi_dev[sel]->dsi_to_ctl1.bits.to_lprx_set      	= 0;
	dsi_dev[sel]->dsi_to_ctl1.dwval = 0x0;

	dsi_dev[sel]->dsi_cfg0.bits.force_rx_0				= 0;
	dsi_dev[sel]->dsi_cfg0.bits.force_rx_1				= 0;
	dsi_dev[sel]->dsi_cfg0.bits.force_rx_2				= 0;
	dsi_dev[sel]->dsi_cfg0.bits.force_rx_3				= 0;
	dsi_dev[sel]->dsi_cfg0.bits.base_dir				= 0;
	dsi_dev[sel]->dsi_ctl1.bits.phy_clk_lane_enable  	= 0;    	//disable clk hs
	dsi_dev[sel]->dsi_phy_ctl2.bits.phy_ck_tx_ulps_req  = 0;    	//
	dsi_dev[sel]->dsi_phy_ctl2.bits.phy_ck_tx_ulps_exit	= 0;    	//
	dsi_dev[sel]->dsi_phy_ctl2.bits.phy_data_tx_ulps_req= 0;    	//
	dsi_dev[sel]->dsi_phy_ctl2.bits.phy_data_tx_upls_exit= 0;    	//
	dsi_dev[sel]->dsi_phy_ctl2.bits.phy_tx_triger     	= 0;

	dsi_dev[sel]->dsi_cmd_ctl.bits.gen_sw_0p_tx_lp     	= 1;
	dsi_dev[sel]->dsi_cmd_ctl.bits.gen_sw_1p_tx_lp     	= 1;
	dsi_dev[sel]->dsi_cmd_ctl.bits.gen_sw_2p_tx_lp     	= 1;
	dsi_dev[sel]->dsi_cmd_ctl.bits.gen_sr_0p_tx_lp     	= 1;
	dsi_dev[sel]->dsi_cmd_ctl.bits.gen_sr_1p_tx_lp     	= 1;
	dsi_dev[sel]->dsi_cmd_ctl.bits.gen_sr_2p_tx_lp     	= 1;
	dsi_dev[sel]->dsi_cmd_ctl.bits.dcs_sw_0p_tx_lp     	= 1;
	dsi_dev[sel]->dsi_cmd_ctl.bits.dcs_sw_1p_tx_lp     	= 1;
	dsi_dev[sel]->dsi_cmd_ctl.bits.dcs_sr_0p_tx_lp     	= 1;
	dsi_dev[sel]->dsi_cmd_ctl.bits.max_rd_pkg_size_lp  	= 1;
	dsi_dev[sel]->dsi_cmd_ctl.bits.gen_lw_tx_lp        	= 1;
	dsi_dev[sel]->dsi_cmd_ctl.bits.dcs_lw_tx_lp        	= 1;

	dsi_dev[sel]->dsi_cmd_ctl.bits.cmd_mode_en			= 1;
	return 0;
}

static __s32 dsi_packet_cfg(__u32 sel,disp_panel_para * panel)
{
	if(panel->lcd_dsi_if == LCD_DSI_IF_COMMAND_MODE)
	{
		__u32 vc		=	0;
		__u32 format	=	panel->lcd_dsi_format;
		__u32 x			=	panel->lcd_x;

		dsi_dev[sel]->dsi_dbi_ctl0.bits.dbi_vid          	= vc;
		switch(format)
		{
		case 0:
			dsi_dev[sel]->dsi_dbi_ctl0.bits.dbi_in_format  	= 11;
			dsi_dev[sel]->dsi_dbi_ctl0.bits.dbi_out_format	= 11;
			dsi_dev[sel]->dsi_dbi_ctl0.bits.lut_size_cfg	= 2;
			break;
		case 1:
			dsi_dev[sel]->dsi_dbi_ctl0.bits.dbi_in_format	= 9;
			dsi_dev[sel]->dsi_dbi_ctl0.bits.dbi_out_format	= 9;
			dsi_dev[sel]->dsi_dbi_ctl0.bits.lut_size_cfg	= 1;
			break;
		case 2:
			dsi_dev[sel]->dsi_dbi_ctl0.bits.dbi_in_format	= 9;
			dsi_dev[sel]->dsi_dbi_ctl0.bits.dbi_out_format	= 9;
			dsi_dev[sel]->dsi_dbi_ctl0.bits.lut_size_cfg	= 1;
			break;
		case 3:
			dsi_dev[sel]->dsi_dbi_ctl0.bits.dbi_in_format  	= 8;
			dsi_dev[sel]->dsi_dbi_ctl0.bits.dbi_out_format	= 8;
			dsi_dev[sel]->dsi_dbi_ctl0.bits.lut_size_cfg	= 0;
			break;
		default:
			dsi_dev[sel]->dsi_dbi_ctl0.bits.dbi_in_format  	= 11;
			dsi_dev[sel]->dsi_dbi_ctl0.bits.dbi_out_format	= 11;
			dsi_dev[sel]->dsi_dbi_ctl0.bits.lut_size_cfg	= 2;
			break;
		}

		dsi_dev[sel]->dsi_dbi_ctl0.bits.partion_mode  		= 1;

		dsi_dev[sel]->dsi_dbi_ctl1.bits.allowed_cmd_size 	= x*dsi_bits_per_pixel[format]/8+1;//??
		dsi_dev[sel]->dsi_dbi_ctl1.bits.wr_cmd_size 		= x*dsi_bits_per_pixel[format]/8+1;//??

		dsi_dev[sel]->dsi_pkg_ctl2.bits.bta_en 			= 0;
		dsi_dev[sel]->dsi_cmd_ctl.bits.pkg_ack_req		= 0;
		dsi_dev[sel]->dsi_cmd_ctl.bits.te_ack_en		= 0;

		dsi_dev[sel]->dsi_cmd_ctl.bits.cmd_mode_en		= 1;

		dsi_dev[sel]->dsi_cfg2.bits.dbi_rst			= 1;
		dsi_dev[sel]->dsi_cfg2.bits.dbi_src				= 1;
	}
	else
	{
		__u32 vc		=	0;
		__u32 lane		=	panel->lcd_dsi_lane;
		__u32 format	=	panel->lcd_dsi_format;
		__u32 x			=	panel->lcd_x;
		__u32 y			=	panel->lcd_y;
		__u32 ht		=	panel->lcd_ht;
		__u32 hbp		=	panel->lcd_hbp;
		__u32 hspw		=	panel->lcd_hspw;
		__u32 vt		=	panel->lcd_vt;
		__u32 vbp		=	panel->lcd_vbp;
		__u32 vspw		=	panel->lcd_vspw;

		switch(format)
		{
		case 0:
			dsi_dev[sel]->dsi_cfg1.bits.dpi_src_format  = 0;
			dsi_dev[sel]->dsi_dpi_cfg0.bits.dpi_format  = 5;
			break;
		case 1:
			dsi_dev[sel]->dsi_cfg1.bits.dpi_src_format  = 1;
			dsi_dev[sel]->dsi_dpi_cfg0.bits.dpi_format  = 3;
			break;
		case 2:
			dsi_dev[sel]->dsi_cfg1.bits.dpi_src_format  = 1;
			dsi_dev[sel]->dsi_dpi_cfg0.bits.dpi_format  = 3;
			break;
		case 3:
			dsi_dev[sel]->dsi_cfg1.bits.dpi_src_format  = 2;
			dsi_dev[sel]->dsi_dpi_cfg0.bits.dpi_format  = 0;
			break;
		default:
			dsi_dev[sel]->dsi_cfg1.bits.dpi_src_format  = 0;
			dsi_dev[sel]->dsi_dpi_cfg0.bits.dpi_format  = 5;
			break;
		}

		dsi_dev[sel]->dsi_dpi_cfg0.bits.dpi_vid           		=	vc;
		dsi_dev[sel]->dsi_dpi_cfg1.bits.de_ploarity  			=	0;
		dsi_dev[sel]->dsi_dpi_cfg1.bits.vsync_ploarity   		=	1;
		dsi_dev[sel]->dsi_dpi_cfg1.bits.hsync_ploarity   		=	1;
		dsi_dev[sel]->dsi_dpi_cfg1.bits.shutd_polarity   		=	0;
		dsi_dev[sel]->dsi_dpi_cfg1.bits.colorm_polarity  		=	0;
		dsi_dev[sel]->dsi_dpi_cfg0.bits.video_mode_format_18bit =	(format==1)?1:0;
		if(panel->lcd_dsi_if == LCD_DSI_IF_VIDEO_MODE)
		{
			dsi_dev[sel]->dsi_vid_ctl0.bits.video_mode_cfg     	= 0;
			dsi_dev[sel]->dsi_vid_ctl0.bits.hfp_lp_en          	= 0;//burst
		}
		else
		{
			dsi_dev[sel]->dsi_vid_ctl0.bits.video_mode_cfg     	= 2;
			dsi_dev[sel]->dsi_vid_ctl0.bits.hfp_lp_en          	= 1;
		}
		dsi_dev[sel]->dsi_vid_ctl0.bits.vsa_lp_en          	= 1;
		dsi_dev[sel]->dsi_vid_ctl0.bits.vbp_lp_en          	= 1;
		dsi_dev[sel]->dsi_vid_ctl0.bits.vfp_lp_en        	= 1;
		dsi_dev[sel]->dsi_vid_ctl0.bits.vact_lp_en         	= 1;
		dsi_dev[sel]->dsi_vid_ctl0.bits.hbp_lp_en          	= 0;

		dsi_dev[sel]->dsi_vid_ctl1.bits.pkt_multi_en       	= 0;
		dsi_dev[sel]->dsi_vid_ctl1.bits.pkt_null_in_hact   	= 0;
		dsi_dev[sel]->dsi_vid_ctl1.bits.bta_per_frame      	= 0;
		dsi_dev[sel]->dsi_vid_ctl1.bits.lp_cmd_en          	= 0;

		dsi_dev[sel]->dsi_vid_tim2.bits.pixels_per_pkg 		= x;
		dsi_dev[sel]->dsi_vid_ctl1.bits.pkt_num_per_line	= 1;
		dsi_dev[sel]->dsi_vid_ctl1.bits.pkt_null_size      	= 0;

		dsi_dev[sel]->dsi_vid_tim3.bits.hsa		       		= hspw      *dsi_bits_per_pixel[format]/(8*lane);// - (4+4+2);
		dsi_dev[sel]->dsi_vid_tim3.bits.hbp		       		= (hbp-hspw)*dsi_bits_per_pixel[format]/(8*lane);// - (4+2);
		dsi_dev[sel]->dsi_vid_tim2.bits.ht		     		= ht        *dsi_bits_per_pixel[format]/(8*lane);

		dsi_dev[sel]->dsi_vid_tim0.bits.vsa       			= vspw;
		dsi_dev[sel]->dsi_vid_tim0.bits.vbp		       		= vbp-vspw;
		dsi_dev[sel]->dsi_vid_tim1.bits.vfp		       		= vt-(vbp+y);
		dsi_dev[sel]->dsi_vid_tim1.bits.vact		  		= y;

		//dsi_dev[sel]->dsi_vid_ctl0.bits.en_video_mode	  = 1;

		dsi_dev[sel]->dsi_cfg1.bits.dpi_shut_down  = 0;
		dsi_dev[sel]->dsi_cfg1.bits.dpi_color_mode = 0;
		//dsi_dev[sel]->dsi_cfg1.bits.dpi_src = 1;

	}
	return 0;
}


__s32 dsi_cfg(__u32 sel,disp_panel_para * panel)
{
	dsi_irq_disable_all(sel);
	dsi_basic_cfg(sel,panel);
	dsi_packet_cfg(sel,panel);

	return 0;
}


__s32 dsi_exit(__u32 sel)
{
	return 0;
}

__u8 dsi_ecc_pro(__u32 dsi_ph)
{
	return 0;
}

__u16 dsi_crc_pro(__u8* pd_p,__u32 pd_bytes)
{
	return 0;
}

__u16 dsi_crc_pro_pd_repeat(__u8 pd,__u32 pd_bytes)
{
	return 0;
}

#ifdef __LINUX_OSAL__
EXPORT_SYMBOL(dsi_dcs_wr);
EXPORT_SYMBOL(dsi_dcs_wr_0para);
EXPORT_SYMBOL(dsi_dcs_wr_1para);
EXPORT_SYMBOL(dsi_dcs_wr_2para);
EXPORT_SYMBOL(dsi_dcs_wr_3para);
EXPORT_SYMBOL(dsi_dcs_wr_4para);
EXPORT_SYMBOL(dsi_dcs_wr_5para);
#endif

#endif
