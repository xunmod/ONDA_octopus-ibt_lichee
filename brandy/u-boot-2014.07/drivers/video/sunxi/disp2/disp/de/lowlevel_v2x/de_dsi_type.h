#ifndef __DE_DSI_TYPE_H__
#define __DE_DSI_TYPE_H__

#include "de_lcd.h"

#if defined(DSI_VERSION_40)
/*
 * Detail information of registers
 */
union dsi_ctl_reg_t {
	u32 dwval;
	struct {
		u32 dsi_en:1;
		u32 res0:31;
	} bits;
};

union dsi_gint0_reg_t {
	u32 dwval;
	struct {
		u32 dsi_irq_en:16;
		u32 dsi_irq_flag:16;
	} bits;
};

union dsi_gint1_reg_t {
	u32 dwval;
	struct {
		u32 video_line_int_num:13;
		u32 res0:19;
	} bits;
};

union dsi_basic_ctl_reg_t {
	u32 dwval;
	struct {
		u32 video_mode_burst:1;
		u32 hsa_hse_dis:1;
		u32 hbp_dis:1;
		u32 trail_fill:1;
		u32 trail_inv:4;
		u32 res0:8;
		u32 brdy_set:8;
		u32 brdy_l_sel:3;
		u32 res1:5;
	} bits;
};

union dsi_basic_ctl0_reg_t {
	u32 dwval;
	struct {
		u32 inst_st:1;
		u32 res0:3;
		u32 src_sel:2;
		u32 res1:4;
		u32 fifo_manual_reset:1;
		u32 res2:1;
		u32 fifo_gating:1;
		u32 res3:3;
		u32 ecc_en:1;
		u32 crc_en:1;
		u32 hs_eotp_en:1;
		u32 res4:13;
	} bits;
};

union dsi_basic_ctl1_reg_t {
	u32 dwval;
	struct {
		u32 dsi_mode:1;
		u32 video_frame_start:1;
		u32 video_precision_mode_align:1;
		u32 res0:1;
		u32 video_start_delay:13;
		u32 res1:15;
	} bits;
};

union dsi_basic_size0_reg_t {
	u32 dwval;
	struct {
		u32 vsa:12;
		u32 res0:4;
		u32 vbp:12;
		u32 res1:4;
	} bits;
};

union dsi_basic_size1_reg_t {
	u32 dwval;
	struct {
		u32 vact:12;
		u32 res0:4;
		u32 vt:13;
		u32 res1:3;
	} bits;
};

union dsi_basic_inst0_reg_t {
	u32 dwval;
	struct {
		u32 lane_den:4;
		u32 lane_cen:1;
		u32 res0:11;
		u32 trans_start_condition:4;
		u32 trans_packet:4;
		u32 escape_enrty:4;
		u32 instru_mode:4;
	} bits;
};

union dsi_basic_inst1_reg_t {
	u32 dwval;
	struct {
		u32 inst0_sel:4;
		u32 inst1_sel:4;
		u32 inst2_sel:4;
		u32 inst3_sel:4;
		u32 inst4_sel:4;
		u32 inst5_sel:4;
		u32 inst6_sel:4;
		u32 inst7_sel:4;
	} bits;
};

union dsi_basic_inst2_reg_t {
	u32 dwval;
	struct {
		u32 loop_n0:12;
		u32 res0:4;
		u32 loop_n1:12;
		u32 res1:4;
	} bits;
};

union dsi_basic_inst3_reg_t {
	u32 dwval;
	struct {
		u32 inst0_jump:4;
		u32 inst1_jump:4;
		u32 inst2_jump:4;
		u32 inst3_jump:4;
		u32 inst4_jump:4;
		u32 inst5_jump:4;
		u32 inst6_jump:4;
		u32 inst7_jump:4;
	} bits;
};

union dsi_basic_inst4_reg_t {
	u32 dwval;
	struct {
		u32 jump_cfg_num:16;
		u32 jump_cfg_point:4;
		u32 jump_cfg_to:4;
		u32 res0:4;
		u32 jump_cfg_en:1;
		u32 res1:3;
	} bits;
};

union dsi_basic_tran0_reg_t {
	u32 dwval;
	struct {
		u32 trans_start_set:13;
		u32 res0:19;
	} bits;
};

union dsi_basic_tran1_reg_t {
	u32 dwval;
	struct {
		u32 trans_size:16;
		u32 res0:12;
		u32 trans_end_condition:1;
		u32 res1:3;
	} bits;
};

union dsi_basic_tran2_reg_t {
	u32 dwval;
	struct {
		u32 trans_cycle_set:16;
		u32 res0:16;
	} bits;
};

union dsi_basic_tran3_reg_t {
	u32 dwval;
	struct {
		u32 trans_blank_set:16;
		u32 res0:16;
	} bits;
};

union dsi_basic_tran4_reg_t {
	u32 dwval;
	struct {
		u32 hs_zero_reduce_set:16;
		u32 res0:16;
	} bits;
};

union dsi_basic_tran5_reg_t {
	u32 dwval;
	struct {
		u32 drq_set:10;
		u32 res0:18;
		u32 drq_mode:1;
		u32 res1:3;
	} bits;
};

union dsi_pixel_ctl0_reg_t {
	u32 dwval;
	struct {
		u32 pixel_format:4;
		u32 pixel_endian:1;
		u32 res0:11;
		u32 pd_plug_dis:1;
		u32 res1:15;
	} bits;
};

union dsi_pixel_ctl1_reg_t {
	u32 dwval;
	struct {
		u32 res0;
	} bits;
};

union dsi_pixel_ph_reg_t {
	u32 dwval;
	struct {
		u32 dt:6;
		u32 vc:2;
		u32 wc:16;
		u32 ecc:8;
	} bits;
};

union dsi_pixel_pd_reg_t {
	u32 dwval;
	struct {
		u32 pd_tran0:8;
		u32 res0:8;
		u32 pd_trann:8;
		u32 res1:8;
	} bits;
};

union dsi_pixel_pf0_reg_t {
	u32 dwval;
	struct {
		u32 crc_force:16;
		u32 res0:16;
	} bits;
};

union dsi_pixel_pf1_reg_t {
	u32 dwval;
	struct {
		u32 crc_init_line0:16;
		u32 crc_init_linen:16;
	} bits;
};

union dsi_short_pkg_reg_t {
	u32 dwval;
	struct {
		u32 dt:6;
		u32 vc:2;
		u32 d0:8;
		u32 d1:8;
		u32 ecc:8;
	} bits;
};

union dsi_blk_pkg0_reg_t {
	u32 dwval;
	struct {
		u32 dt:6;
		u32 vc:2;
		u32 wc:16;
		u32 ecc:8;
	} bits;
};

union dsi_blk_pkg1_reg_t {
	u32 dwval;
	struct {
		u32 pd:8;
		u32 res0:8;
		u32 pf:16;
	} bits;
};

union dsi_burst_line_reg_t {
	u32 dwval;
	struct {
		u32 line_num:16;
		u32 line_syncpoint:16;
	} bits;
};

union dsi_burst_drq_reg_t {
	u32 dwval;
	struct {
		u32 drq_edge0:16;
		u32 drq_edge1:16;
	} bits;
};

union dsi_cmd_ctl_reg_t {
	u32 dwval;
	struct {
		u32 tx_size:8;
		u32 tx_status:1;
		u32 tx_flag:1;
		u32 res0:6;
		u32 rx_size:5;
		u32 res1:3;
		u32 rx_status:1;
		u32 rx_flag:1;
		u32 rx_overflow:1;
		u32 res2:5;
	} bits;
};

union dsi_cmd_data_reg_t {
	u32 dwval;
	struct {
		u32 byte0:8;
		u32 byte1:8;
		u32 byte2:8;
		u32 byte3:8;
	} bits;
};

union dsi_debug0_reg_t {
	u32 dwval;
	struct {
		u32 video_curr_line:13;
		u32 res0:19;
	} bits;
};

union dsi_debug1_reg_t {
	u32 dwval;
	struct {
		u32 video_curr_lp2hs:16;
		u32 res0:16;
	} bits;
};

union dsi_debug2_reg_t {
	u32 dwval;
	struct {
		u32 trans_low_flag:1;
		u32 trans_fast_flag:1;
		u32 res0:2;
		u32 curr_loop_num:16;
		u32 curr_instru_num:3;
		u32 res1:1;
		u32 instru_unknow_flag:8;
	} bits;
};

union dsi_debug3_reg_t {
	u32 dwval;
	struct {
		u32 res0:16;
		u32 curr_fifo_num:16;
	} bits;
};

union dsi_debug4_reg_t {
	u32 dwval;
	struct {
		u32 test_data:24;
		u32 res0:4;
		u32 dsi_fifo_bist_en:1;
		u32 res1:3;
	} bits;
};

union dsi_reservd_reg_t {
	u32 dwval;
	struct {
		u32 res0;
	} bits;
};

struct __de_dsi_dev_t {
	/* 0x00 - 0x0c */
	union dsi_ctl_reg_t dsi_gctl;
	union dsi_gint0_reg_t dsi_gint0;
	union dsi_gint1_reg_t dsi_gint1;
	union dsi_basic_ctl_reg_t dsi_basic_ctl;
	/* 0x10 - 0x1c */
	union dsi_basic_ctl0_reg_t dsi_basic_ctl0;
	union dsi_basic_ctl1_reg_t dsi_basic_ctl1;
	union dsi_basic_size0_reg_t dsi_basic_size0;
	union dsi_basic_size1_reg_t dsi_basic_size1;
	/* 0x20 - 0x3c */
	union dsi_basic_inst0_reg_t dsi_inst_func[8];
	/* 0x40 - 0x5c */
	union dsi_basic_inst1_reg_t dsi_inst_loop_sel;
	union dsi_basic_inst2_reg_t dsi_inst_loop_num;
	union dsi_basic_inst3_reg_t dsi_inst_jump_sel;
	union dsi_basic_inst4_reg_t dsi_inst_jump_cfg[2];
	union dsi_basic_inst2_reg_t dsi_inst_loop_num2;
	union dsi_reservd_reg_t dsi_reg058[2];
	/* 0x60 - 0x6c */
	union dsi_basic_tran0_reg_t dsi_trans_start;
	union dsi_reservd_reg_t dsi_reg064[3];
	/* 0x70 - 0x7c */
	union dsi_reservd_reg_t dsi_reg070[2];
	union dsi_basic_tran4_reg_t dsi_trans_zero;
	union dsi_basic_tran5_reg_t dsi_tcon_drq;
	/* 0x80 - 0x8c */
	union dsi_pixel_ctl0_reg_t dsi_pixel_ctl0;
	union dsi_pixel_ctl1_reg_t dsi_pixel_ctl1;
	union dsi_reservd_reg_t dsi_reg088[2];
	/* 0x90 - 0x9c */
	union dsi_pixel_ph_reg_t dsi_pixel_ph;
	union dsi_pixel_pd_reg_t dsi_pixel_pd;
	union dsi_pixel_pf0_reg_t dsi_pixel_pf0;
	union dsi_pixel_pf1_reg_t dsi_pixel_pf1;
	/* 0xa0 - 0xac */
	union dsi_reservd_reg_t dsi_reg0a0[4];
	/* 0xb0 - 0xbc */
	union dsi_short_pkg_reg_t dsi_sync_hss;
	union dsi_short_pkg_reg_t dsi_sync_hse;
	union dsi_short_pkg_reg_t dsi_sync_vss;
	union dsi_short_pkg_reg_t dsi_sync_vse;
	/* 0xc0 - 0xcc */
	union dsi_blk_pkg0_reg_t dsi_blk_hsa0;
	union dsi_blk_pkg1_reg_t dsi_blk_hsa1;
	union dsi_blk_pkg0_reg_t dsi_blk_hbp0;
	union dsi_blk_pkg1_reg_t dsi_blk_hbp1;
	/* 0xd0 - 0xdc */
	union dsi_blk_pkg0_reg_t dsi_blk_hfp0;
	union dsi_blk_pkg1_reg_t dsi_blk_hfp1;
	union dsi_reservd_reg_t dsi_reg0d8[2];
	/* 0xe0 - 0xec */
	union dsi_blk_pkg0_reg_t dsi_blk_hblk0;
	union dsi_blk_pkg1_reg_t dsi_blk_hblk1;
	union dsi_blk_pkg0_reg_t dsi_blk_vblk0;
	union dsi_blk_pkg1_reg_t dsi_blk_vblk1;
	/* 0xf0 - 0x1fc */
	union dsi_burst_line_reg_t dsi_burst_line;
	union dsi_burst_drq_reg_t dsi_burst_drq;
	union dsi_reservd_reg_t dsi_reg0f0[66];
	/* 0x200 - 0x23c */
	union dsi_cmd_ctl_reg_t dsi_cmd_ctl;
	union dsi_reservd_reg_t dsi_reg204[15];
	/* 0x240 - 0x2dc */
	union dsi_cmd_data_reg_t dsi_cmd_rx[8];
	union dsi_reservd_reg_t dsi_reg260[32];
	/* 0x2e0 - 0x2ec */
	union dsi_debug0_reg_t dsi_debug_video0;
	union dsi_debug1_reg_t dsi_debug_video1;
	union dsi_reservd_reg_t dsi_reg2e8[2];
	/* 0x2f0 - 0x2fc */
	union dsi_debug2_reg_t dsi_debug_inst;
	union dsi_debug3_reg_t dsi_debug_fifo;
	union dsi_debug4_reg_t dsi_debug_data;
	union dsi_reservd_reg_t dsi_reg2fc;
	/* 0x300 - 0x3fc */
	union dsi_cmd_data_reg_t dsi_cmd_tx[64];
};

union dphy_ctl_reg_t {
	u32 dwval;
	struct {
		u32 module_en:1;
		u32 res0:3;
		u32 lane_num:2;
		u32 res1:26;
	} bits;
};

union dphy_tx_ctl_reg_t {
	u32 dwval;
	struct {
		u32 tx_d0_force:1;
		u32 tx_d1_force:1;
		u32 tx_d2_force:1;
		u32 tx_d3_force:1;
		u32 tx_clk_force:1;
		u32 res0:3;
		u32 lptx_endian:1;
		u32 hstx_endian:1;
		u32 lptx_8b9b_en:1;
		u32 hstx_8b9b_en:1;
		u32 force_lp11:1;
		u32 res1:3;
		u32 ulpstx_data0_exit:1;
		u32 ulpstx_data1_exit:1;
		u32 ulpstx_data2_exit:1;
		u32 ulpstx_data3_exit:1;
		u32 ulpstx_clk_exit:1;
		u32 res2:3;
		u32 hstx_data_exit:1;
		u32 hstx_clk_exit:1;
		u32 res3:2;
		u32 hstx_clk_cont:1;
		u32 ulpstx_enter:1;
		u32 res4:2;
	} bits;
};

union dphy_rx_ctl_reg_t {
	u32 dwval;
	struct {
		u32 res0:8;
		u32 lprx_endian:1;
		u32 hsrx_endian:1;
		u32 lprx_8b9b_en:1;
		u32 hsrx_8b9b_en:1;
		u32 hsrx_sync:1;
		u32 res1:3;
		u32 lprx_trnd_mask:4;
		u32 rx_d0_force:1;
		u32 rx_d1_force:1;
		u32 rx_d2_force:1;
		u32 rx_d3_force:1;
		u32 rx_clk_force:1;
		u32 res2:6;
		u32 dbc_en:1;
	} bits;
};

union dphy_tx_time0_reg_t {
	u32 dwval;
	struct {
		u32 lpx_tm_set:8;
		u32 dterm_set:8;
		u32 hs_pre_set:8;
		u32 hs_trail_set:8;
	} bits;
};

union dphy_tx_time1_reg_t {
	u32 dwval;
	struct {
		u32 ck_prep_set:8;
		u32 ck_zero_set:8;
		u32 ck_pre_set:8;
		u32 ck_post_set:8;
	} bits;
};

union dphy_tx_time2_reg_t {
	u32 dwval;
	struct {
		u32 ck_trail_set:8;
		u32 hs_dly_set:16;
		u32 res0:4;
		u32 hs_dly_mode:1;
		u32 res1:3;
	} bits;
};

union dphy_tx_time3_reg_t {
	u32 dwval;
	struct {
		u32 lptx_ulps_exit_set:20;
		u32 res0:12;
	} bits;
};

union dphy_tx_time4_reg_t {
	u32 dwval;
	struct {
		u32 hstx_ana0_set:8;
		u32 hstx_ana1_set:8;
		u32 res0:16;
	} bits;
};

union dphy_rx_time0_reg_t {
	u32 dwval;
	struct {
		u32 lprx_to_en:1;
		u32 freq_cnt_en:1;
		u32 res0:2;
		u32 hsrx_clk_miss_en:1;
		u32 hsrx_sync_err_to_en:1;
		u32 res1:2;
		u32 lprx_to:8;
		u32 hsrx_clk_miss:8;
		u32 hsrx_sync_err_to:8;
	} bits;
};

union dphy_rx_time1_reg_t {
	u32 dwval;
	struct {
		u32 lprx_ulps_wp:20;
		u32 rx_dly:12;
	} bits;
};

union dphy_rx_time2_reg_t {
	u32 dwval;
	struct {
		u32 hsrx_ana0_set:8;
		u32 hsrx_ana1_set:8;
		u32 res0:16;
	} bits;
};

union dphy_rx_time3_reg_t {
	u32 dwval;
	struct {
		u32 freq_cnt:16;
		u32 res0:8;
		u32 lprst_dly:8;
	} bits;
};

union dphy_ana0_reg_t {
	u32 dwval;
	struct {
		u32 reg_selsck:1;
		u32 reg_rsd:1;
		u32 reg_sfb:2;
		u32 reg_plr:4;
		u32 reg_den:4;
		u32 reg_slv:3;
		u32 reg_sdiv2:1;
		u32 reg_srxck:4;
		u32 reg_srxdt:4;
		u32 reg_dmpd:4;
		u32 reg_dmpc:1;
		u32 reg_pwenc:1;
		u32 reg_pwend:1;
		u32 reg_pws:1;
	} bits;
};

union dphy_ana1_reg_t {
	u32 dwval;
	struct {
		u32 reg_stxck:1;
		u32 res0:3;
		u32 reg_svdl0:4;
		u32 reg_svdl1:4;
		u32 reg_svdl2:4;
		u32 reg_svdl3:4;
		u32 reg_svdlc:4;
		u32 reg_svtt:4;
		u32 reg_csmps:2;
		u32 res1:1;
		u32 reg_vttmode:1;
	} bits;
};

union dphy_ana2_reg_t {
	u32 dwval;
	struct {
		u32 ana_cpu_en:1;
		u32 enib:1;
		u32 enrvs:1;
		u32 res0:1;
		u32 enck_cpu:1;
		u32 entxc_cpu:1;
		u32 enckq_cpu:1;
		u32 res1:1;
		u32 entx_cpu:4;
		u32 res2:1;
		u32 entermc_cpu:1;
		u32 enrxc_cpu:1;
		u32 res3:1;
		u32 enterm_cpu:4;
		u32 enrx_cpu:4;
		u32 enp2s_cpu:4;
		u32 res4:4;
	} bits;
};

union dphy_ana3_reg_t {
	u32 dwval;
	struct {
		u32 enlptx_cpu:4;
		u32 enlprx_cpu:4;
		u32 enlpcd_cpu:4;
		u32 enlprxc_cpu:1;
		u32 enlptxc_cpu:1;
		u32 enlpcdc_cpu:1;
		u32 res0:1;
		u32 entest:1;
		u32 enckdbg:1;
		u32 enldor:1;
		u32 res1:5;
		u32 enldod:1;
		u32 enldoc:1;
		u32 endiv:1;
		u32 envttc:1;
		u32 envttd:4;
	} bits;
};

union dphy_ana4_reg_t {
	u32 dwval;
	struct {
		u32 reg_txpusd:2;
		u32 reg_txpusc:2;
		u32 reg_txdnsd:2;
		u32 reg_txdnsc:2;
		u32 reg_tmsd:2;
		u32 reg_tmsc:2;
		u32 reg_ckdv:5;
		u32 res0:3;
		u32 reg_dmplvd:4;
		u32 reg_dmplvc:1;
		u32 res1:7;

	} bits;
};

union dphy_int_en0_reg_t {
	u32 dwval;
	struct {
		u32 sot_d0_int:1;
		u32 sot_d1_int:1;
		u32 sot_d2_int:1;
		u32 sot_d3_int:1;
		u32 sot_err_d0_int:1;
		u32 sot_err_d1_int:1;
		u32 sot_err_d2_int:1;
		u32 sot_err_d3_int:1;
		u32 sot_sync_err_d0_int:1;
		u32 sot_sync_err_d1_int:1;
		u32 sot_sync_err_d2_int:1;
		u32 sot_sync_err_d3_int:1;
		u32 rx_alg_err_d0_int:1;
		u32 rx_alg_err_d1_int:1;
		u32 rx_alg_err_d2_int:1;
		u32 rx_alg_err_d3_int:1;
		u32 res0:6;
		u32 cd_lp0_err_clk_int:1;
		u32 cd_lp1_err_clk_int:1;
		u32 cd_lp0_err_d0_int:1;
		u32 cd_lp1_err_d0_int:1;
		u32 cd_lp0_err_d1_int:1;
		u32 cd_lp1_err_d1_int:1;
		u32 cd_lp0_err_d2_int:1;
		u32 cd_lp1_err_d2_int:1;
		u32 cd_lp0_err_d3_int:1;
		u32 cd_lp1_err_d3_int:1;
	} bits;
};

union dphy_int_en1_reg_t {
	u32 dwval;
	struct {
		u32 ulps_d0_int:1;
		u32 ulps_d1_int:1;
		u32 ulps_d2_int:1;
		u32 ulps_d3_int:1;
		u32 ulps_wp_d0_int:1;
		u32 ulps_wp_d1_int:1;
		u32 ulps_wp_d2_int:1;
		u32 ulps_wp_d3_int:1;
		u32 ulps_clk_int:1;
		u32 ulps_wp_clk_int:1;
		u32 res0:2;
		u32 lpdt_d0_int:1;
		u32 rx_trnd_d0_int:1;
		u32 tx_trnd_err_d0_int:1;
		u32 undef1_d0_int:1;
		u32 undef2_d0_int:1;
		u32 undef3_d0_int:1;
		u32 undef4_d0_int:1;
		u32 undef5_d0_int:1;
		u32 rst_d0_int:1;
		u32 rst_d1_int:1;
		u32 rst_d2_int:1;
		u32 rst_d3_int:1;
		u32 esc_cmd_err_d0_int:1;
		u32 esc_cmd_err_d1_int:1;
		u32 esc_cmd_err_d2_int:1;
		u32 esc_cmd_err_d3_int:1;
		u32 false_ctl_d0_int:1;
		u32 false_ctl_d1_int:1;
		u32 false_ctl_d2_int:1;
		u32 false_ctl_d3_int:1;
	} bits;
};

union dphy_int_en2_reg_t {
	u32 dwval;
	struct {
		u32 res0;
	} bits;
};

union dphy_int_pd0_reg_t {
	u32 dwval;
	struct {
		u32 sot_d0_pd:1;
		u32 sot_d1_pd:1;
		u32 sot_d2_pd:1;
		u32 sot_d3_pd:1;
		u32 sot_err_d0_pd:1;
		u32 sot_err_d1_pd:1;
		u32 sot_err_d2_pd:1;
		u32 sot_err_d3_pd:1;
		u32 sot_sync_err_d0_pd:1;
		u32 sot_sync_err_d1_pd:1;
		u32 sot_sync_err_d2_pd:1;
		u32 sot_sync_err_d3_pd:1;
		u32 rx_alg_err_d0_pd:1;
		u32 rx_alg_err_d1_pd:1;
		u32 rx_alg_err_d2_pd:1;
		u32 rx_alg_err_d3_pd:1;
		u32 res0:6;
		u32 cd_lp0_err_clk_pd:1;
		u32 cd_lp1_err_clk_pd:1;
		u32 cd_lp0_err_d1_pd:1;
		u32 cd_lp1_err_d1_pd:1;
		u32 cd_lp0_err_d0_pd:1;
		u32 cd_lp1_err_d0_pd:1;
		u32 cd_lp0_err_d2_pd:1;
		u32 cd_lp1_err_d2_pd:1;
		u32 cd_lp0_err_d3_pd:1;
		u32 cd_lp1_err_d3_pd:1;
	} bits;
};

union dphy_int_pd1_reg_t {
	u32 dwval;
	struct {
		u32 ulps_d0_pd:1;
		u32 ulps_d1_pd:1;
		u32 ulps_d2_pd:1;
		u32 ulps_d3_pd:1;
		u32 ulps_wp_d0_pd:1;
		u32 ulps_wp_d1_pd:1;
		u32 ulps_wp_d2_pd:1;
		u32 ulps_wp_d3_pd:1;
		u32 ulps_clk_pd:1;
		u32 ulps_wp_clk_pd:1;
		u32 res0:2;
		u32 lpdt_d0_pd:1;
		u32 rx_trnd_d0_pd:1;
		u32 tx_trnd_err_d0_pd:1;
		u32 undef1_d0_pd:1;
		u32 undef2_d0_pd:1;
		u32 undef3_d0_pd:1;
		u32 undef4_d0_pd:1;
		u32 undef5_d0_pd:1;
		u32 rst_d0_pd:1;
		u32 rst_d1_pd:1;
		u32 rst_d2_pd:1;
		u32 rst_d3_pd:1;
		u32 esc_cmd_err_d0_pd:1;
		u32 esc_cmd_err_d1_pd:1;
		u32 esc_cmd_err_d2_pd:1;
		u32 esc_cmd_err_d3_pd:1;
		u32 false_ctl_d0_pd:1;
		u32 false_ctl_d1_pd:1;
		u32 false_ctl_d2_pd:1;
		u32 false_ctl_d3_pd:1;
	} bits;
};

union dphy_int_pd2_reg_t {
	u32 dwval;
	struct {
		u32 res0;
	} bits;
};

union dphy_dbg0_reg_t {
	u32 dwval;
	struct {
		u32 lptx_sta_d0:3;
		u32 res0:1;
		u32 lptx_sta_d1:3;
		u32 res1:1;
		u32 lptx_sta_d2:3;
		u32 res2:1;
		u32 lptx_sta_d3:3;
		u32 res3:1;
		u32 lptx_sta_clk:3;
		u32 res4:9;
		u32 direction:1;
		u32 res5:3;
	} bits;
};

union dphy_dbg1_reg_t {
	u32 dwval;
	struct {
		u32 lptx_dbg_en:1;
		u32 hstx_dbg_en:1;
		u32 res0:2;
		u32 lptx_set_d0:2;
		u32 lptx_set_d1:2;
		u32 lptx_set_d2:2;
		u32 lptx_set_d3:2;
		u32 lptx_set_ck:2;
		u32 res1:18;
	} bits;
};

union dphy_dbg2_reg_t {
	u32 dwval;
	struct {
		u32 hstx_data;
	} bits;
};

union dphy_dbg3_reg_t {
	u32 dwval;
	struct {
		u32 lprx_sta_d0:4;
		u32 lprx_sta_d1:4;
		u32 lprx_sta_d2:4;
		u32 lprx_sta_d3:4;
		u32 lprx_sta_clk:4;
		u32 res0:12;
	} bits;
};

union dphy_dbg4_reg_t {
	u32 dwval;
	struct {
		u32 lprx_phy_d0:2;
		u32 lprx_phy_d1:2;
		u32 lprx_phy_d2:2;
		u32 lprx_phy_d3:2;
		u32 lprx_phy_clk:2;
		u32 res0:6;
		u32 lpcd_phy_d0:2;
		u32 lpcd_phy_d1:2;
		u32 lpcd_phy_d2:2;
		u32 lpcd_phy_d3:2;
		u32 lpcd_phy_clk:2;
		u32 res1:6;
	} bits;
};

union dphy_dbg5_reg_t {
	u32 dwval;
	struct {
		u32 hsrx_data;
	} bits;
};

union dphy_reservd_reg_t {
	u32 dwval;
	struct {
		u32 res0;
	} bits;
};

/* dphy register define */
struct __de_dsi_dphy_dev_t {
	/* 0x00 - 0x0c */
	union dphy_ctl_reg_t dphy_gctl;
	union dphy_tx_ctl_reg_t dphy_tx_ctl;
	union dphy_rx_ctl_reg_t dphy_rx_ctl;
	union dphy_reservd_reg_t dphy_reg00c;
	/* 0x10 - 0x1c */
	union dphy_tx_time0_reg_t dphy_tx_time0;
	union dphy_tx_time1_reg_t dphy_tx_time1;
	union dphy_tx_time2_reg_t dphy_tx_time2;
	union dphy_tx_time3_reg_t dphy_tx_time3;
	/* 0x20 - 0x2c */
	union dphy_tx_time4_reg_t dphy_tx_time4;
	union dphy_reservd_reg_t dphy_reg024[3];
	/* 0x30 - 0x3c */
	union dphy_rx_time0_reg_t dphy_rx_time0;
	union dphy_rx_time1_reg_t dphy_rx_time1;
	union dphy_rx_time2_reg_t dphy_rx_time2;
	union dphy_reservd_reg_t dphy_reg03c;
	/* 0x40 - 0x4c */
	union dphy_rx_time3_reg_t dphy_rx_time3;
	union dphy_reservd_reg_t dphy_reg044[2];
	union dphy_ana0_reg_t dphy_ana0;
	/* 0x50 - 0x5c */
	union dphy_ana1_reg_t dphy_ana1;
	union dphy_ana2_reg_t dphy_ana2;
	union dphy_ana3_reg_t dphy_ana3;
	union dphy_ana4_reg_t dphy_ana4;
	/* 0x60 - 0x6c */
	union dphy_int_en0_reg_t dphy_int_en0;
	union dphy_int_en1_reg_t dphy_int_en1;
	union dphy_int_en2_reg_t dphy_int_en2;
	union dphy_reservd_reg_t dphy_reg06c;
	/* 0x70 - 0x7c */
	union dphy_int_pd0_reg_t dphy_int_pd0;
	union dphy_int_pd1_reg_t dphy_int_pd1;
	union dphy_int_pd2_reg_t dphy_int_pd2;
	union dphy_reservd_reg_t dphy_reg07c;
	/* 0x80 - 0xdc */
	union dphy_reservd_reg_t dphy_reg080[24];
	/* 0xe0 - 0xec */
	union dphy_dbg0_reg_t dphy_dbg0;
	union dphy_dbg1_reg_t dphy_dbg1;
	union dphy_dbg2_reg_t dphy_dbg2;
	union dphy_dbg3_reg_t dphy_dbg3;
	/* 0xf0 - 0xfc */
	union dphy_dbg4_reg_t dphy_dbg4;
	union dphy_dbg5_reg_t dphy_dbg5;
};

union dsi_ph_t {
	struct {
		u32 byte012:24;
		u32 byte3:8;
	} bytes;
	struct {
		u32 bit00:1;
		u32 bit01:1;
		u32 bit02:1;
		u32 bit03:1;
		u32 bit04:1;
		u32 bit05:1;
		u32 bit06:1;
		u32 bit07:1;
		u32 bit08:1;
		u32 bit09:1;
		u32 bit10:1;
		u32 bit11:1;
		u32 bit12:1;
		u32 bit13:1;
		u32 bit14:1;
		u32 bit15:1;
		u32 bit16:1;
		u32 bit17:1;
		u32 bit18:1;
		u32 bit19:1;
		u32 bit20:1;
		u32 bit21:1;
		u32 bit22:1;
		u32 bit23:1;
		u32 bit24:1;
		u32 bit25:1;
		u32 bit26:1;
		u32 bit27:1;
		u32 bit28:1;
		u32 bit29:1;
		u32 bit30:1;
		u32 bit31:1;
	} bits;
};

#elif defined (DSI_VERSION_28)

//
// Detail information of registers
//

typedef union
{
	u32 dwval;
	struct
	{
		u32 module_en               :  1 ;    // Default: 0x0;
		u32 res0                    : 15 ;    // Default: ;
		u32 version                 : 16 ;    // Default: 0x0102;
	} bits;
} DSI_CTL0_t;

typedef union
{
	u32 dwval;
	struct
	{
		u32 phy_en                  :  1 ;    // Default: 0x0;
		u32 phy_rst                 :  1 ;    // Default: 0x0;
		u32 res0                    :  2 ;    // Default: ;
		u32 phy_lane_num            :  2 ;    // Default: 0x0;
		u32 res1                    :  2 ;    // Default: ;
		u32 phy_clk_gating          :  1 ;    // Default: 0x0;
		u32 phy_clk_lane_enable     :  1 ;    // Default: 0x0;
		u32 res2                    : 22 ;    // Default: ;
	} bits;
} DSI_CTL1_t;

typedef union
{
	u32 dwval;
	struct
	{
		u32 lp_clk_div              :  8 ;    // Default: 0x0;
		u32 res0                    : 24 ;    // Default: ;
	} bits;
} DSI_CTL2_t;

typedef union
{
	u32 dwval;
	struct
	{
		u32 video_mode_en           :  1 ;    // Default: 0x0;
		u32 res0                    :  3 ;    // Default: ;
		u32 video_mode_cfg          :  2 ;    // Default: 0x0;
		u32 res1                    :  2 ;    // Default: ;
		u32 vsa_lp_en               :  1 ;    // Default: 0x0;
		u32 vbp_lp_en               :  1 ;    // Default: 0x0;
		u32 vact_lp_en              :  1 ;    // Default: 0x0;
		u32 vfp_lp_en               :  1 ;    // Default: 0x0;
		u32 hbp_lp_en               :  1 ;    // Default: 0x0;
		u32 hfp_lp_en               :  1 ;    // Default: 0x0;
		u32 res2                    : 18 ;    // Default: ;
	} bits;
} DSI_VID_CTL0_t;

typedef union
{
	u32 dwval;
	struct
	{
		u32 bta_per_frame           :  1 ;    // Default: 0x0;
		u32 lp_cmd_en               :  1 ;    // Default: 0x0;
		u32 res0                    :  2 ;    // Default: ;
		u32 pkt_multi_en            :  1 ;    // Default: 0x0;
		u32 pkt_null_in_hact        :  1 ;    // Default: 0x0;
		u32 res1                    :  2 ;    // Default: ;
		u32 pkt_num_per_line        : 10 ;    // Default: 0x0;
		u32 res2                    :  2 ;    // Default: ;
		u32 pkt_null_size           : 10 ;    // Default: 0x0;
		u32 res3                    :  2 ;    // Default: ;
	} bits;
} DSI_VID_CTL1_t;

typedef union
{
	u32 dwval;
	struct
	{
		u32 vsa                     :  4 ;    // Default: 0x0;
		u32 res0                    : 12 ;    // Default: ;
		u32 vbp                     :  6 ;    // Default: 0x0;
		u32 res1                    : 10 ;    // Default: ;
	} bits;
} DSI_VID_TIM0_t;

typedef union
{
	u32 dwval;
	struct
	{
		u32 vact                    : 11 ;    // Default: 0x0;
		u32 res0                    :  5 ;    // Default: ;
		u32 vfp                     :  6 ;    // Default: 0x0;
		u32 res1                    : 10 ;    // Default: ;
	} bits;
} DSI_VID_TIM1_t;

typedef union
{
	u32 dwval;
	struct
	{
		u32 pixels_per_pkg          : 11 ;    // Default: 0x0;
		u32 res0                    :  5 ;    // Default: ;
		u32 ht                      : 14 ;    // Default: 0x0;
		u32 res1                    :  2 ;    // Default: ;
	} bits;
} DSI_VID_TIM2_t;

typedef union
{
	u32 dwval;
	struct
	{
		u32 hsa                     :  9 ;    // Default: 0x0;
		u32 res0                    :  7 ;    // Default: ;
		u32 hbp                     :  9 ;    // Default: 0x0;
		u32 res1                    :  7 ;    // Default: ;
	} bits;
} DSI_VID_TIM3_t;

typedef union
{
	u32 dwval;
	struct
	{
		u32 res0                    :  8 ;    // Default: ;
		u32 dpi_format              :  3 ;    // Default: 0x0;
		u32 res1                    :  1 ;    // Default: ;
		u32 video_mode_format_18bit :  1 ;    // Default: 0x0;
		u32 res2                    : 15 ;    // Default: ;
		u32 dpi_vid                 :  2 ;    // Default: 0x0;
		u32 res3                    :  2 ;    // Default: ;
	} bits;
} DSI_DPI_CFG0_t;

typedef union
{
	u32 dwval;
	struct
	{
		u32 vsync_ploarity          :  1 ;    // Default: 0x0;
		u32 hsync_ploarity          :  1 ;    // Default: 0x0;
		u32 res0                    :  1 ;    // Default: ;
		u32 de_ploarity             :  1 ;    // Default: 0x0;
		u32 shutd_polarity          :  1 ;    // Default: 0x0;
		u32 colorm_polarity         :  1 ;    // Default: 0x0;
		u32 res1                    : 26 ;    // Default: ;
	} bits;
} DSI_DPI_CFG1_t;

typedef union
{
	u32 dwval;
	struct
	{
		u32 dt                      :  6 ;    // Default: 0x0;
		u32 vc                      :  2 ;    // Default: 0x0;
		u32 wc                      : 16 ;    // Default: 0x0;
		u32 res0                    :  8 ;    // Default: ;
	} bits;
} DSI_PKG_CTL0_t;

typedef union
{
	u32 dwval;
	struct
	{
		u32 pld_byte1               :  8 ;    // Default: 0x0;
		u32 pld_byte2               :  8 ;    // Default: 0x0;
		u32 pld_byte3               :  8 ;    // Default: 0x0;
		u32 pld_byte4               :  8 ;    // Default: 0x0;
	} bits;
} DSI_PKG_CTL1_t;

typedef union
{
	u32 dwval;
	struct
	{
		u32 fifo_cmd_empty          :  1 ;    // Default: 0x1;
		u32 fifo_pld_w_empty        :  1 ;    // Default: 0x1;
		u32 fifo_pld_r_empty        :  1 ;    // Default: 0x1;
		u32 res0                    :  1 ;    // Default: ;
		u32 fifo_cmd_full           :  1 ;    // Default: 0x0;
		u32 fifo_pld_w_full         :  1 ;    // Default: 0x0;
		u32 fifo_pld_r_full         :  1 ;    // Default: 0x0;
		u32 res1                    :  1 ;    // Default: ;
		u32 rd_cmd_busy             :  1 ;    // Default: 0x0;
		u32 res2                    : 23 ;    // Default: ;
	} bits;
} DSI_PKG_STATUS_t;

typedef union
{
	u32 dwval;
	struct
	{
		u32 bta_en                  :  1 ;    // Default: 0x0;
		u32 res0                    :  3 ;    // Default: ;
		u32 eotp_tx_en              :  1 ;    // Default: 0x0;
		u32 res1                    :  3 ;    // Default: ;
		u32 eotp_rx_en              :  1 ;    // Default: 0x0;
		u32 ecc_rx_en               :  1 ;    // Default: 0x0;
		u32 crc_rx_en               :  1 ;    // Default: 0x0;
		u32 res2                    : 17 ;    // Default: ;
		u32 vid_rx                  :  2 ;    // Default: 0x0;
		u32 res3                    :  2 ;    // Default: ;
	} bits;
} DSI_PKG_CTL2_t;

typedef union
{
	u32 dwval;
	struct
	{
		u32 cmd_mode_en             :  1 ;    // Default: 0x0;
		u32 res0                    :  3 ;    // Default: ;
		u32 gen_sw_0p_tx_lp         :  1 ;    // Default: 0x0;
		u32 gen_sw_1p_tx_lp         :  1 ;    // Default: 0x0;
		u32 gen_sw_2p_tx_lp         :  1 ;    // Default: 0x0;
		u32 gen_lw_tx_lp            :  1 ;    // Default: 0x0;
		u32 gen_sr_0p_tx_lp         :  1 ;    // Default: 0x0;
		u32 gen_sr_1p_tx_lp         :  1 ;    // Default: 0x0;
		u32 gen_sr_2p_tx_lp         :  1 ;    // Default: 0x0;
		u32 res1                    :  1 ;    // Default: ;
		u32 dcs_sw_0p_tx_lp         :  1 ;    // Default: 0x0;
		u32 dcs_sw_1p_tx_lp         :  1 ;    // Default: 0x0;
		u32 dcs_lw_tx_lp            :  1 ;    // Default: 0x0;
		u32 res2                    :  1 ;    // Default: ;
		u32 dcs_sr_0p_tx_lp         :  1 ;    // Default: 0x0;
		u32 res3                    :  3 ;    // Default: ;
		u32 max_rd_pkg_size_lp      :  1 ;    // Default: 0x0;
		u32 res4                    :  3 ;    // Default: ;
		u32 pkg_ack_req             :  1 ;    // Default: 0x0;
		u32 res5                    :  3 ;    // Default: ;
		u32 te_ack_en               :  1 ;    // Default: 0x0;
		u32 res6                    :  3 ;    // Default: ;
	} bits;
} DSI_CMD_CTL_t;

typedef union
{
	u32 dwval;
	struct
	{
		u32 res0                    :  4 ;    // Default: ;
		u32 dbi_in_format           :  4 ;    // Default: 0x0;
		u32 dbi_out_format          :  4 ;    // Default: 0x0;
		u32 lut_size_cfg            :  2 ;    // Default: 0x0;
		u32 res1                    :  2 ;    // Default: ;
		u32 partion_mode            :  1 ;    // Default: 0x0;
		u32 res2                    : 11 ;    // Default: ;
		u32 dbi_vid                 :  2 ;    // Default: 0x0;
		u32 res3                    :  2 ;    // Default: ;
	} bits;
} DSI_DBI_CTL0_t;

typedef union
{
	u32 dwval;
	struct
	{
		u32 wr_cmd_size             : 16 ;    // Default: 0x0;
		u32 allowed_cmd_size        : 16 ;    // Default: 0x0;
	} bits;
} DSI_DBI_CTL1_t;

typedef union
{
	u32 dwval;
	struct
	{
		u32 dbi_fifo_cmd_empty      :  1 ;    // Default: 0x1;
		u32 dbi_fifo_pld_w_empty    :  1 ;    // Default: 0x1;
		u32 dbi_fifo_pld_r_empty    :  1 ;    // Default: 0x1;
		u32 res0                    :  1 ;    // Default: ;
		u32 dbi_fifo_cmd_full       :  1 ;    // Default: 0x0;
		u32 dbi_fifo_pld_w_full     :  1 ;    // Default: 0x0;
		u32 dbi_fifo_pld_r_full     :  1 ;    // Default: 0x0;
		u32 res1                    :  1 ;    // Default: ;
		u32 dbi_rd_cmd_busy         :  1 ;    // Default: 0x0;
		u32 res2                    : 23 ;    // Default: ;
	} bits;
} DSI_DBI_CTL2_t;

typedef union
{
	u32 dwval;
	struct
	{
		u32 to_clk_div              :  8 ;    // Default: 0x0;
		u32 res0                    : 24 ;    // Default: ;
	} bits;
} DSI_TO_CTL0_t;

typedef union
{
	u32 dwval;
	struct
	{
		u32 to_hstx_set             : 16 ;    // Default: 0x0;
		u32 to_lprx_set             : 16 ;    // Default: 0x0;
	} bits;
} DSI_TO_CTL1_t;

typedef union
{
	u32 dwval;
	struct
	{
		u32 lpcmd_time_invact       :  8 ;    // Default: 0x0;
		u32 res0                    :  8 ;    // Default: ;
		u32 lpcmd_time_outvact      :  8 ;    // Default: 0x0;
		u32 res1                    :  8 ;    // Default: ;
	} bits;
} DSI_VID_CTL2_t;

typedef union
{
	u32 dwval;
	struct
	{
		u32 phy_lp2hs_set           :  8 ;    // Default: 0x0;
		u32 res0                    :  8 ;    // Default: ;
		u32 phy_hs2lp_set           :  8 ;    // Default: 0x0;
		u32 res1                    :  8 ;    // Default: ;
	} bits;
} DSI_PHY_CTL0_t;

typedef union
{
	u32 dwval;
	struct
	{
		u32 phy_stop_set            :  8 ;    // Default: 0x0;
		u32 res0                    :  8 ;    // Default: ;
		u32 max_rd_set              : 15 ;    // Default: 0x0;
		u32 res1                    :  1 ;    // Default: ;
	} bits;
} DSI_PHY_CTL1_t;

typedef union
{
	u32 dwval;
	struct
	{
		u32 res0                    :  4 ;    // Default: ;
		u32 phy_ck_tx_ulps_req      :  1 ;    // Default: 0x0;
		u32 phy_ck_tx_ulps_exit     :  1 ;    // Default: 0x0;
		u32 res1                    :  2 ;    // Default: ;
		u32 phy_data_tx_ulps_req    :  1 ;    // Default: 0x0;
		u32 phy_data_tx_upls_exit   :  1 ;    // Default: 0x0;
		u32 res2                    :  6 ;    // Default: ;
		u32 phy_tx_triger           :  4 ;    // Default: 0x0;
		u32 res3                    : 12 ;    // Default: ;
	} bits;
} DSI_PHY_CTL2_t;

typedef union
{
	u32 dwval;
	struct
	{
		u32 phy_lock                :  1 ;    // Default: 0x0;
		u32 res0                    :  3 ;    // Default: ;
		u32 phy_dir                 :  1 ;    // Default: 0x0;
		u32 res1                    :  3 ;    // Default: ;
		u32 phy_d0_stop             :  1 ;    // Default: 0x0;
		u32 phy_d1_stop             :  1 ;    // Default: 0x0;
		u32 phy_d2_stop             :  1 ;    // Default: 0x0;
		u32 phy_d3_stop             :  1 ;    // Default: 0x0;
		u32 phy_ck_stop             :  1 ;    // Default: 0x0;
		u32 res2                    :  3 ;    // Default: ;
		u32 phy_d0_no_ulps          :  1 ;    // Default: 0x1;
		u32 phy_d1_no_ulps          :  1 ;    // Default: 0x1;
		u32 phy_d2_no_ulps          :  1 ;    // Default: 0x1;
		u32 phy_d3_no_ulps          :  1 ;    // Default: 0x1;
		u32 phy_ck_no_ulps          :  1 ;    // Default: 0x1;
		u32 res3                    :  3 ;    // Default: ;
		u32 phy_d0rx_no_ulps        :  1 ;    // Default: 0x0;
		u32 res4                    :  7 ;    // Default: ;
	} bits;
} DSI_PHY_STATUS_t;

typedef union
{
	u32 dwval;
	struct
	{
		u32 phy_cfg_clr             :  1 ;    // Default: 0x1;
		u32 phy_cfg_clk             :  1 ;    // Default: 0x0;
		u32 res0                    :  2 ;    // Default: ;
		u32 phy_cfg_en              :  1 ;    // Default: 0x0;
		u32 res1                    : 11 ;    // Default: ;
		u32 phy_cfg_din             :  8 ;    // Default: 0x0;
		u32 phy_cfg_dout            :  8 ;    // Default: 0x0;
	} bits;
} DSI_PHY_CTL3_t;

typedef union
{
	u32 dwval;
	struct
	{
		u32 ack_with_err_0          :  1 ;    // Default: 0x0;
		u32 ack_with_err_1          :  1 ;    // Default: 0x0;
		u32 ack_with_err_2          :  1 ;    // Default: 0x0;
		u32 ack_with_err_3          :  1 ;    // Default: 0x0;
		u32 ack_with_err_4          :  1 ;    // Default: 0x0;
		u32 ack_with_err_5          :  1 ;    // Default: 0x0;
		u32 ack_with_err_6          :  1 ;    // Default: 0x0;
		u32 ack_with_err_7          :  1 ;    // Default: 0x0;
		u32 ack_with_err_8          :  1 ;    // Default: 0x0;
		u32 ack_with_err_9          :  1 ;    // Default: 0x0;
		u32 ack_with_err_10         :  1 ;    // Default: 0x0;
		u32 ack_with_err_11         :  1 ;    // Default: 0x0;
		u32 ack_with_err_12         :  1 ;    // Default: 0x0;
		u32 ack_with_err_13         :  1 ;    // Default: 0x0;
		u32 ack_with_err_14         :  1 ;    // Default: 0x0;
		u32 ack_with_err_15         :  1 ;    // Default: 0x0;
		u32 dphy_errors_0           :  1 ;    // Default: 0x0;
		u32 dphy_errors_1           :  1 ;    // Default: 0x0;
		u32 dphy_errors_2           :  1 ;    // Default: 0x0;
		u32 dphy_errors_3           :  1 ;    // Default: 0x0;
		u32 dphy_errors_4           :  1 ;    // Default: 0x0;
		u32 res0                    : 11 ;    // Default: ;
	} bits;
} DSI_IRQ_EN0_t;

typedef union
{
	u32 dwval;
	struct
	{
		u32 ecc_single_err          :  1 ;    // Default: 0x0;
		u32 ecc_multi_err           :  1 ;    // Default: 0x0;
		u32 crc_err                 :  1 ;    // Default: 0x0;
		u32 eopt_err                :  1 ;    // Default: 0x0;
		u32 pkt_size_err            :  1 ;    // Default: 0x0;
		u32 res0                    :  3 ;    // Default: ;
		u32 dpi_pld_wr_err          :  1 ;    // Default: 0x0;
		u32 res1                    :  3 ;    // Default: ;
		u32 gen_cmd_wr_err          :  1 ;    // Default: 0x0;
		u32 gen_pld_wr_err          :  1 ;    // Default: 0x0;
		u32 gen_pld_send_err        :  1 ;    // Default: 0x0;
		u32 gen_pld_rd_err          :  1 ;    // Default: 0x0;
		u32 gen_pld_recv_err        :  1 ;    // Default: 0x0;
		u32 res2                    :  3 ;    // Default: ;
		u32 dbi_cmd_wr_err          :  1 ;    // Default: 0x0;
		u32 dbi_pld_wr_err          :  1 ;    // Default: 0x0;
		u32 dbi_pld_rd_err          :  1 ;    // Default: 0x0;
		u32 dbi_pld_recv_err        :  1 ;    // Default: 0x0;
		u32 dbi_illegal_comm_err    :  1 ;    // Default: 0x0;
		u32 res3                    :  3 ;    // Default: ;
		u32 to_hs_tx                :  1 ;    // Default: 0x0;
		u32 to_lp_rx                :  1 ;    // Default: 0x0;
		u32 res4                    :  2 ;    // Default: ;
	} bits;
} DSI_IRQ_EN1_t;

typedef union
{
	u32 dwval;
	struct
	{
		u32 ack_with_err_0          :  1 ;    // Default: 0x0;
		u32 ack_with_err_1          :  1 ;    // Default: 0x0;
		u32 ack_with_err_2          :  1 ;    // Default: 0x0;
		u32 ack_with_err_3          :  1 ;    // Default: 0x0;
		u32 ack_with_err_4          :  1 ;    // Default: 0x0;
		u32 ack_with_err_5          :  1 ;    // Default: 0x0;
		u32 ack_with_err_6          :  1 ;    // Default: 0x0;
		u32 ack_with_err_7          :  1 ;    // Default: 0x0;
		u32 ack_with_err_8          :  1 ;    // Default: 0x0;
		u32 ack_with_err_9          :  1 ;    // Default: 0x0;
		u32 ack_with_err_10         :  1 ;    // Default: 0x0;
		u32 ack_with_err_11         :  1 ;    // Default: 0x0;
		u32 ack_with_err_12         :  1 ;    // Default: 0x0;
		u32 ack_with_err_13         :  1 ;    // Default: 0x0;
		u32 ack_with_err_14         :  1 ;    // Default: 0x0;
		u32 ack_with_err_15         :  1 ;    // Default: 0x0;
		u32 dphy_errors_0           :  1 ;    // Default: 0x0;
		u32 dphy_errors_1           :  1 ;    // Default: 0x0;
		u32 dphy_errors_2           :  1 ;    // Default: 0x0;
		u32 dphy_errors_3           :  1 ;    // Default: 0x0;
		u32 dphy_errors_4           :  1 ;    // Default: 0x0;
		u32 res0                    : 11 ;    // Default: ;
	} bits;
} DSI_IRQ_STATUS0_t;

typedef union
{
	u32 dwval;
	struct
	{
		u32 ecc_single_err          :  1 ;    // Default: 0x0;
		u32 ecc_multi_err           :  1 ;    // Default: 0x0;
		u32 crc_err                 :  1 ;    // Default: 0x0;
		u32 eopt_err                :  1 ;    // Default: 0x0;
		u32 pkt_size_err            :  1 ;    // Default: 0x0;
		u32 res0                    :  3 ;    // Default: ;
		u32 dpi_pld_wr_err          :  1 ;    // Default: 0x0;
		u32 res1                    :  3 ;    // Default: ;
		u32 gen_cmd_wr_err          :  1 ;    // Default: 0x0;
		u32 gen_pld_wr_err          :  1 ;    // Default: 0x0;
		u32 gen_pld_send_err        :  1 ;    // Default: 0x0;
		u32 gen_pld_rd_err          :  1 ;    // Default: 0x0;
		u32 gen_pld_recv_err        :  1 ;    // Default: 0x0;
		u32 res2                    :  3 ;    // Default: ;
		u32 dbi_cmd_wr_err          :  1 ;    // Default: 0x0;
		u32 dbi_pld_wr_err          :  1 ;    // Default: 0x0;
		u32 dbi_pld_rd_err          :  1 ;    // Default: 0x0;
		u32 dbi_pld_recv_err        :  1 ;    // Default: 0x0;
		u32 dbi_illegal_comm_err    :  1 ;    // Default: 0x0;
		u32 res3                    :  3 ;    // Default: ;
		u32 to_hs_tx                :  1 ;    // Default: 0x0;
		u32 to_lp_rx                :  1 ;    // Default: 0x0;
		u32 res4                    :  2 ;    // Default: ;
	} bits;
} DSI_IRQ_STATUS1_t;

typedef union
{
	u32 dwval;
	struct
	{
		u32 base_dir                :  4 ;    // Default: 0x0;
		u32 res0                    : 11 ;    // Default: ;
		u32 bist_on                 :  1 ;    // Default: 0x0;
		u32 turn_dis_0              :  1 ;    // Default: 0x0;
		u32 force_rx_0              :  1 ;    // Default: 0x0;
		u32 force_tx_stop_0         :  1 ;    // Default: 0x0;
		u32 res1                    :  1 ;    // Default: ;
		u32 turn_dis_1              :  1 ;    // Default: 0x0;
		u32 force_rx_1              :  1 ;    // Default: 0x0;
		u32 force_tx_stop_1         :  1 ;    // Default: 0x0;
		u32 res2                    :  1 ;    // Default: ;
		u32 turn_dis_2              :  1 ;    // Default: 0x0;
		u32 force_rx_2              :  1 ;    // Default: 0x0;
		u32 force_tx_stop_2         :  1 ;    // Default: 0x0;
		u32 res3                    :  1 ;    // Default: ;
		u32 turn_dis_3              :  1 ;    // Default: 0x0;
		u32 force_rx_3              :  1 ;    // Default: 0x0;
		u32 force_tx_stop_3         :  1 ;    // Default: 0x0;
		u32 res4                    :  1 ;    // Default: ;
	} bits;
} DSI_CFG0_t;

typedef union
{
	u32 dwval;
	struct
	{
		u32 dpi_color_mode          :  1 ;    // Default: 0x0;
		u32 dpi_shut_down           :  1 ;    // Default: 0x0;
		u32 dpi_src_format          :  2 ;    // Default: 0x0;
		u32 res0                    : 27 ;    // Default: ;
		u32 dpi_src                 :  1 ;    // Default: 0x0;
	} bits;
} DSI_CFG1_t;

typedef union
{
	u32 dwval;
	struct
	{
		u32 dbi_rst                 :  1 ;    // Default: 0x0;
		u32 lcd_te_en               :  1 ;    // Default: 0x0;
		u32 res0                    : 29 ;    // Default: ;
		u32 dbi_src                 :  1 ;    // Default: 0x0;
	} bits;
} DSI_CFG2_t;

typedef union
{
	u32 dwval;
	struct
	{
		u32 bist_en                 :  1 ;    // Default: 0x0;
		u32 res0                    :  3 ;    // Default: ;
		u32 sram0_bist_en           :  1 ;    // Default: 0x0;
		u32 sram1_bist_en           :  1 ;    // Default: 0x0;
		u32 sram2_bist_en           :  1 ;    // Default: 0x0;
		u32 sram3_bist_en           :  1 ;    // Default: 0x0;
		u32 res1                    : 12 ;    // Default: ;
		u32 reg_snk                 :  3 ;    // Default: 0x2;
		u32 res2                    :  5 ;    // Default: ;
		u32 reg_rint                :  2 ;    // Default: 0x2;
		u32 reg_rext                :  1 ;    // Default: 0x0;
		u32 res3                    :  1 ;    // Default: ;
	} bits;
} DSI_CFG3_t;

typedef union
{
	u32 dwval;
	struct
	{
		u32 bist_status             :  1 ;    // Default: 0x0;
		u32 res0                    : 31 ;    // Default: ;
	} bits;
} DSI_STATUS_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 res0                    ;    // Default: ;
	} bits;
}DSI_RESERVD_REG_t;

typedef struct
{
	DSI_CTL0_t			dsi_ctl0;			//0x000
	DSI_CTL1_t			dsi_ctl1;			//0x004
	DSI_CTL2_t			dsi_ctl2;           //0x008
	DSI_RESERVD_REG_t	dsi_reg00C;         //0x00C
	DSI_VID_CTL0_t		dsi_vid_ctl0;       //0x010
	DSI_RESERVD_REG_t	dsi_reg014[2];      //0x014~0x018
	DSI_VID_CTL1_t		dsi_vid_ctl1;       //0x01C
	DSI_VID_TIM0_t		dsi_vid_tim0;       //0x020
	DSI_VID_TIM1_t		dsi_vid_tim1;       //0x024
	DSI_VID_TIM2_t		dsi_vid_tim2;       //0x028
	DSI_VID_TIM3_t		dsi_vid_tim3;       //0x02C
	DSI_DPI_CFG0_t		dsi_dpi_cfg0;       //0x030
	DSI_RESERVD_REG_t	dsi_reg034[2];      //0x034~0x038
	DSI_DPI_CFG1_t		dsi_dpi_cfg1;       //0x03C
	DSI_PKG_CTL0_t		dsi_pkg_ctl0;       //0x040
	DSI_PKG_CTL1_t		dsi_pkg_ctl1;       //0x044
	DSI_PKG_STATUS_t	dsi_pkg_status;		//0x048
	DSI_RESERVD_REG_t	dsi_reg04C;         //0x04C
	DSI_PKG_CTL2_t		dsi_pkg_ctl2;       //0x050
	DSI_CMD_CTL_t		dsi_cmd_ctl; 		//0x054
	DSI_RESERVD_REG_t	dsi_reg058[10];     //0x058~0x07C
	DSI_DBI_CTL0_t		dsi_dbi_ctl0;       //0x080
	DSI_DBI_CTL1_t		dsi_dbi_ctl1;       //0x084
	DSI_DBI_CTL2_t		dsi_dbi_ctl2;       //0x088
	DSI_RESERVD_REG_t	dsi_reg08C[5];      //0x08C~0x09C
	DSI_TO_CTL0_t		dsi_to_ctl0;		//0x0A0
	DSI_TO_CTL1_t		dsi_to_ctl1;		//0x0A4
	DSI_VID_CTL2_t		dsi_vid_ctl2;       //0x0A8
	DSI_RESERVD_REG_t	dsi_reg0AC;         //0x0AC
	DSI_PHY_CTL0_t		dsi_phy_ctl0;       //0x0B0
	DSI_PHY_CTL1_t  	dsi_phy_ctl1;       //0x0B4
	DSI_RESERVD_REG_t	dsi_reg0B8[2];      //0x0B8~0x0BC
	DSI_PHY_CTL2_t		dsi_phy_ctl2;       //0x0C0
	DSI_PHY_STATUS_t	dsi_phy_status;     //0x0C4
	DSI_RESERVD_REG_t	dsi_reg0C8[10];     //0x0C8~0x0EC
	DSI_PHY_CTL3_t		dsi_phy_ctl3;       //0x0F0
	DSI_RESERVD_REG_t	dsi_reg0F4[59];     //0x0F4~0x1DC
	DSI_IRQ_EN0_t		dsi_irq_en0;        //0x1E0
	DSI_IRQ_EN1_t		dsi_irq_en1;        //0x1E4
	DSI_RESERVD_REG_t	dsi_reg1E8[2];      //0x1E8~0x1EC
	DSI_IRQ_STATUS0_t	dsi_irq_status0;    //0x1F0
	DSI_IRQ_STATUS1_t	dsi_irq_status1;    //0x1F4
	DSI_RESERVD_REG_t	dsi_reg1F8[130];    //0x1E8~0x1EC
	DSI_CFG0_t			dsi_cfg0;           //0x400
	DSI_CFG1_t			dsi_cfg1;           //0x404
	DSI_CFG2_t			dsi_cfg2;           //0x408
	DSI_CFG3_t			dsi_cfg3; 			//0x40C
	DSI_STATUS_t		dsi_statu_s;        //0x410
}__de_dsi_dev_t;

__u8 	dsi_ecc_pro(__u32 dsi_ph);
__u16 	dsi_crc_pro(__u8* pd_p,__u32 pd_bytes);
__u16 	dsi_crc_pro_pd_repeat(__u8 pd,__u32 pd_bytes);
//__s32 	dsi_dphy_init(__u32 sel,__panel_para_t * panel);
//__s32 	dsi_inst_init(__u32 sel,__panel_para_t * panel);
//__s32 	dsi_packet_init(__u32 sel,__panel_para_t * panel);



/* lvds */
typedef union
{
	u32 dwval;
	struct
	{
		u32 lvds_data_revert     :  4 ;    //default; 0:revert
		u32 lvds_clk_revert      :  1 ;    //default; 0: revert
		u32 res0                       : 15 ;    // default: ;
		u32 lvds_clk_sel         :  1 ;    // default: 0;
		u32 res1                       :  2 ;    // default: ;
		u32 lvds_correct_mode    :  1 ;    // default: 0;
		u32 lvds_debug_mode      :  1 ;    // default: 0;
		u32 lvds_debug_en        :  1 ;    // default: 0;
		u32 lvds_bitwidth        :  1 ;    // default: 0;
		u32 lvds_mode            :  1 ;    // default: 0;
		u32 lvds_dir             :  1 ;    // default: 0;
		u32 lvds_even_odd_dir    :  1 ;    // default: 0;
		u32 lvds_link	           :  1 ;    // default: 0;
		u32 lvds_en              :  1 ;    // default: 0;
	} bits;
} lvds_if_reg_t;
#endif
#endif
