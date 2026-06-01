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

static const struct freq_tbl ftbl_sdcc1_apps_clk_src[] = {
	F(144000, CFG_CLK_SRC_CXO, 16, 3, 25),
	F(400000, CFG_CLK_SRC_CXO, 12, 1, 4),
	F(20000000, (4 << 8) /*P_GPLL0_DIV2*/, 5, 1, 4),
	F(25000000, (4 << 8) /*P_GPLL0_DIV2*/, 16, 0, 0),
	F(50000000, CFG_CLK_SRC_GPLL0, 16, 0, 0),
	F(100000000, CFG_CLK_SRC_GPLL0, 8, 0, 0),
	F(177770000, CFG_CLK_SRC_GPLL0, 4.5, 0, 0),
	F(192000000, (2 << 8) /*P_GPLL4*/, 6, 0, 0),
	F(384000000, (2 << 8) /*P_GPLL4*/, 3, 0, 0),
	{ }
};

static const struct freq_tbl ftbl_sdcc2_apps_clk_src[] = {
	F(144000, CFG_CLK_SRC_CXO, 16, 3, 25),
	F(400000, CFG_CLK_SRC_CXO, 12, 1, 4),
	F(20000000, (4 << 8) /*P_GPLL0_DIV2*/, 5, 1, 4),
	F(25000000, (4 << 8) /*P_GPLL0_DIV2*/, 16, 0, 0),
	F(50000000, CFG_CLK_SRC_GPLL0, 16, 0, 0),
	F(100000000, CFG_CLK_SRC_GPLL0, 8, 0, 0),
	F(177770000, CFG_CLK_SRC_GPLL0, 4.5, 0, 0),
	F(192000000, (2 << 8) /*P_GPLL4*/, 6, 0, 0),
	F(200000000, CFG_CLK_SRC_GPLL0, 4, 0, 0),
	{ }
};

static const struct freq_tbl ftbl_usb30_master_clk_src[] = {
	F(80000000, (2 << 8) /*P_GPLL0_DIV2*/, 5, 0, 0),
	F(100000000, CFG_CLK_SRC_GPLL0, 8, 0, 0),
	F(133330000, CFG_CLK_SRC_GPLL0, 6, 0, 0),
	{ }
};

static const struct freq_tbl ftbl_usb30_mock_utmi_clk_src[] = {
	F(19200000, CFG_CLK_SRC_CXO, 1, 0, 0),
	F(60000000, (2 << 8) /*P_GPLL6_DIV2*/, 9, 1, 1),
	{ }
};

static ulong msm8953_set_rate(struct clk *clk, ulong rate)
{
	struct msm_clk_priv *priv = dev_get_priv(clk->dev);
	const struct freq_tbl *freq;

	log_err("%s:%d DBG clk->id=%ld rate=%ld\n", __func__, __LINE__, clk->id, rate);

	switch (clk->id) {
	case GCC_SDCC1_APPS_CLK: /* SDC1 */
		freq = qcom_find_freq(ftbl_sdcc1_apps_clk_src, rate);
		clk_rcg_set_rate_mnd(priv->base, 0x42004,
				     freq->pre_div, freq->m, freq->n, freq->src, 8);
		return freq->freq;
	case GCC_SDCC2_APPS_CLK: /* SDC2 */
		freq = qcom_find_freq(ftbl_sdcc2_apps_clk_src, rate);
		clk_rcg_set_rate_mnd(priv->base, 0x43004,
				     freq->pre_div, freq->m, freq->n, freq->src, 8);
		return freq->freq;
	case GCC_BLSP1_UART1_APPS_CLK: /* UART1 */
		//msm8953_clk_init_uart(priv->base, clk->id);
		return 7372800;
	case GCC_USB30_MASTER_CLK:
		freq = qcom_find_freq(ftbl_usb30_master_clk_src, rate);
		clk_rcg_set_rate_mnd(priv->base, 0x3f00c,
				     freq->pre_div, freq->m, freq->n, freq->src, 0);
		return freq->freq;
	case GCC_USB30_MOCK_UTMI_CLK:
		freq = qcom_find_freq(ftbl_usb30_mock_utmi_clk_src, rate);
		clk_rcg_set_rate_mnd(priv->base, 0x3f020,
				     freq->pre_div, freq->m, freq->n, freq->src, 8);
		return freq->freq;
	default:
		// TODO Enable usb30_master_clk_src for gcc_pcnoc_usb3_axi_clk & gcc_usb30_master_clk?
		// TODO Enable usb30_mock_utmi_clk_src for gcc_usb30_mock_utmi_clk?
		log_err("%s:%d DBG clk->id=%ld\n", __func__, __LINE__, clk->id);
		return 0;
	}
}

static const struct gate_clk msm8953_clks[] = {
	GATE_CLK_POLLED(GCC_PRNG_AHB_CLK,	0x45004, BIT(8), 0x13004),
	GATE_CLK_POLLED(GCC_SDCC1_AHB_CLK,	0x4201c, BIT(0), 0x4201c),
	GATE_CLK_POLLED(GCC_SDCC1_APPS_CLK,	0x42018, BIT(0), 0x42018),
	GATE_CLK_POLLED(GCC_SDCC2_AHB_CLK,	0x4301c, BIT(0), 0x4301c),
	GATE_CLK_POLLED(GCC_SDCC2_APPS_CLK,	0x43018, BIT(0), 0x43018),

	GATE_CLK_POLLED(GCC_USB30_MASTER_CLK,		0x3f000, BIT(0), 0x3f000),
	GATE_CLK_POLLED(GCC_USB30_SLEEP_CLK,		0x3f004, BIT(0), 0x3f004),
	GATE_CLK_POLLED(GCC_USB30_MOCK_UTMI_CLK,	0x3f008, BIT(0), 0x3f008),
	GATE_CLK_POLLED(GCC_PCNOC_USB3_AXI_CLK,		0x3f038, BIT(0), 0x3f038),
	//GATE_CLK_POLLED(GCC_USB_PHY_CFG_AHB_CLK,	0x3f080, BIT(0), 0x3f080),
	GATE_CLK(GCC_USB_PHY_CFG_AHB_CLK,		0x3f080, BIT(0)),
};

static int msm8953_enable(struct clk *clk)
{
	struct msm_clk_priv *priv = dev_get_priv(clk->dev);

	if (priv->data->num_clks < clk->id || !msm8953_clks[clk->id].reg) {
		log_warning("%s: unknown clk id %lu\n", __func__, clk->id);
		return 0;
	}

	//switch (clk->id) {
	//case GCC_USB30_MASTER_CLK:
	//case GCC_PCNOC_USB3_AXI_CLK:
	//	qcom_gate_clk_en(priv, USB30_MASTER_CLK_SRC);
	//}

	debug("%s: enabling clock %s\n", __func__, msm8953_clks[clk->id].name);

	return qcom_gate_clk_en(priv, clk->id);
}

static const struct qcom_reset_map msm8953_resets[] = {
	[GCC_QUSB2_PHY_BCR]	= { 0x4103c },
};

static const struct qcom_power_map msm8953_gdscs[] = {
	[USB30_GDSC]		= { 0x3f078 },
};

static struct msm_clk_data msm8953_clk_data = {
	.resets = msm8953_resets,
	.num_resets = ARRAY_SIZE(msm8953_resets),

	.clks = msm8953_clks,
	.num_clks = ARRAY_SIZE(msm8953_clks),

	.power_domains = msm8953_gdscs,
	.num_power_domains = ARRAY_SIZE(msm8953_gdscs),

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
