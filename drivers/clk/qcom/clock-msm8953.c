// SPDX-License-Identifier: BSD-3-Clause
/*
 * Clock drivers for Qualcomm MSM8953
 *
 * Copyright (c) 2026 Luca Weiss <luca.weiss@fairphone.com>
 */

#include <clk-uclass.h>
#include <dm.h>
#include <asm/io.h>
#include <linux/bitops.h>
#include <dt-bindings/clock/qcom,gcc-msm8953.h>

#include "clock-qcom.h"

/* Clocks: (from CLK_CTL_BASE)  */
//#define GPLL0_STATUS			(0x2101C)
//#define APCS_GPLL_ENA_VOTE		(0x45000)
//#define APCS_CLOCK_BRANCH_ENA_VOTE (0x45004)
//
///* BLSP1 AHB clock (root clock for BLSP) */
//#define BLSP1_AHB_CBCR			0x1008
//
///* Uart clock control registers */
//#define BLSP1_UART1_APPS_CBCR		(0x203C)
//#define BLSP1_UART1_APPS_CMD_RCGR	(0x2044)
//#define BLSP1_UART2_APPS_CBCR		(0x302C)
//#define BLSP1_UART2_APPS_CMD_RCGR	(0x3034)
//
///* GPLL0 clock control registers */
//#define GPLL0_STATUS_ACTIVE BIT(17)

//static struct pll_vote_clk gpll0_vote_clk = {
//	.status = GPLL0_STATUS,
//	.status_bit = GPLL0_STATUS_ACTIVE,
//	.ena_vote = APCS_GPLL_ENA_VOTE,
//	.vote_bit = BIT(0),
//};
//
//static struct vote_clk gcc_blsp1_ahb_clk = {
//	.cbcr_reg = BLSP1_AHB_CBCR,
//	.ena_vote = APCS_CLOCK_BRANCH_ENA_VOTE,
//	.vote_bit = BIT(10),
//};

/* UART: 115200 */
//int msm8953_clk_init_uart(phys_addr_t base, unsigned long id)
//{
//	u32 cmd_rcgr, apps_cbcr;
//
//	switch (id) {
//	case GCC_BLSP1_UART1_APPS_CLK:
//		cmd_rcgr = BLSP1_UART1_APPS_CMD_RCGR;
//		apps_cbcr = BLSP1_UART1_APPS_CBCR;
//		break;
//	default:
//		return 0;
//	}
//
//	/* Enable AHB clock */
//	clk_enable_vote_clk(base, &gcc_blsp1_ahb_clk);
//
//	/* 7372800 uart block clock @ GPLL0 */
//	clk_rcg_set_rate_mnd(base, cmd_rcgr, 1, 144, 15625, CFG_CLK_SRC_GPLL0,
//			     16);
//
//	/* Vote for gpll0 clock */
//	clk_enable_gpll0(base, &gpll0_vote_clk);
//
//	/* Enable core clk */
//	clk_enable_cbc(base + apps_cbcr);
//
//	return 0;
//}

static ulong msm8953_set_rate(struct clk *clk, ulong rate)
{
	//struct msm_clk_priv *priv = dev_get_priv(clk->dev);

	switch (clk->id) {
	case GCC_BLSP1_UART1_APPS_CLK: /* UART1 */
		//msm8953_clk_init_uart(priv->base, clk->id);
		return 7372800;
	default:
		return 0;
	}
}

static const struct gate_clk msm8953_clks[] = {
	GATE_CLK_POLLED(GCC_PRNG_AHB_CLK,	0x45004, BIT(8), 0x13004),
	GATE_CLK_POLLED(GCC_SDCC1_AHB_CLK,	0x4201c, BIT(0), 0x4201c),
	GATE_CLK_POLLED(GCC_SDCC1_APPS_CLK,	0x42018, BIT(0), 0x42018),
	GATE_CLK_POLLED(GCC_SDCC2_AHB_CLK,	0x4301c, BIT(0), 0x4301c),
	GATE_CLK_POLLED(GCC_SDCC2_APPS_CLK,	0x43018, BIT(0), 0x43018),
};

static int msm8953_enable(struct clk *clk)
{
	struct msm_clk_priv *priv = dev_get_priv(clk->dev);

	if (priv->data->num_clks < clk->id || !msm8953_clks[clk->id].reg) {
		log_warning("%s: unknown clk id %lu\n", __func__, clk->id);
		return 0;
	}

	debug("%s: enabling clock %s\n", __func__, msm8953_clks[clk->id].name);

	return qcom_gate_clk_en(priv, clk->id);
}

static struct msm_clk_data msm8953_clk_data = {

	.clks = msm8953_clks,
	.num_clks = ARRAY_SIZE(msm8953_clks),

	.enable = msm8953_enable,
	.set_rate = msm8953_set_rate,
};

static const struct udevice_id gcc_msm8953_of_match[] = {
	{
		.compatible = "qcom,gcc-msm8953",
		.data = (ulong)&msm8953_clk_data,
	},
	{}
};

U_BOOT_DRIVER(gcc_msm8953) = {
	.name = "gcc_msm8953",
	.id = UCLASS_NOP,
	.of_match = gcc_msm8953_of_match,
	.bind = qcom_cc_bind,
	.flags = DM_FLAG_PRE_RELOC,
};
