// SPDX-License-Identifier: GPL-2.0
/*
 * Stub clk driver for non-essential clocks.
 *
 * This driver should be used for clock controllers
 * which are described as dependencies in DT but aren't
 * actually necessary for hardware functionality.
 */

#include <clk-uclass.h>
#include <dm.h>
#include <dm/lists.h>

/* NOP parent nodes to stub clocks */
static const struct udevice_id nop_parent_ids[] = {
	{ .compatible = "qcom,glink-rpm" },
	{ .compatible = "qcom,glink-smd-rpm" },
	{ .compatible = "qcom,smd-rpm" },
	{ }
};

U_BOOT_DRIVER(nop_parent) = {
	.name = "nop_parent",
	.id = UCLASS_NOP,
	.of_match = nop_parent_ids,
	.bind = dm_scan_fdt_dev,
	.flags = DM_FLAG_DEFAULT_PD_CTRL_OFF,
};

int smd_rpm_bind(struct udevice *dev)
{
	ofnode child, child_smd;
	struct udevice *child_dev;
	int ret;

	dev_for_each_subnode(child, dev) {
		/* smd-edge has no compatible, iterate down manually */
		if (strcmp(child.np->name, "smd-edge") == 0) {
			printk("DBG %s:%d child=%s\n", __func__, __LINE__, child.np->name);
			ofnode_for_each_subnode(child_smd, child) {
				printk("DBG %s:%d child_smd=%s\n", __func__, __LINE__, child_smd.np->name);
				ret = lists_bind_fdt(dev, child_smd, &child_dev, NULL, false);
				if (ret)
					return ret;
			}
		} else {
			printk("DBG %s:%d child=%s\n", __func__, __LINE__, child.np->name);
			ret = lists_bind_fdt(dev, child, &child_dev, NULL, false);
			if (ret)
				return ret;
		}

		//if (!ofnode_is_enabled(child))
		//	continue;


		//ret = 0;
		///* Find the device for this ofnode, or bind it */
		//if (device_find_global_by_ofnode(child, &child_dev))
		//	ret = lists_bind_fdt(dev, child, &child_dev, NULL, false);	
		//if (ret) {
		//	/* Skip nodes that don't have drivers */
		//	debug("Failed to probe child %s: %d\n", ofnode_get_name(child), ret);
		//	continue;
		//}
		//debug("Probing child %s\n", child_dev->name);
		//device_probe(child_dev);
	}

	return 0;
}

static const struct udevice_id smd_rpm_ids[] = {
	{ .compatible = "qcom,rpm-proc" },
};

U_BOOT_DRIVER(smd_rpm) = {
	.name = "smd_rpm",
	.id = UCLASS_NOP,
	.of_match = smd_rpm_ids,
	.bind = smd_rpm_bind,
};

static ulong stub_clk_set_rate(struct clk *clk, ulong rate)
{
	return (clk->rate = rate);
}

static ulong stub_clk_get_rate(struct clk *clk)
{
	return clk->rate;
}

static int stub_clk_nop(struct clk *clk)
{
	return 0;
}

static struct clk_ops stub_clk_ops = {
	.set_rate = stub_clk_set_rate,
	.get_rate = stub_clk_get_rate,
	.enable = stub_clk_nop,
	.disable = stub_clk_nop,
};

static const struct udevice_id stub_clk_ids[] = {
	{ .compatible = "qcom,rpmcc" },
	{ .compatible = "qcom,sdm670-rpmh-clk" },
	{ .compatible = "qcom,sdm845-rpmh-clk" },
	{ .compatible = "qcom,sc7180-rpmh-clk" },
	{ .compatible = "qcom,sc7280-rpmh-clk" },
	{ .compatible = "qcom,sm6350-rpmh-clk" },
	{ .compatible = "qcom,sm8150-rpmh-clk" },
	{ .compatible = "qcom,sm8250-rpmh-clk" },
	{ .compatible = "qcom,sm8550-rpmh-clk" },
	{ .compatible = "qcom,sm8650-rpmh-clk" },
	{ }
};

U_BOOT_DRIVER(clk_stub) = {
	.name = "clk_stub",
	.id = UCLASS_CLK,
	.ops = &stub_clk_ops,
	.of_match = stub_clk_ids,
	.flags = DM_FLAG_DEFAULT_PD_CTRL_OFF,
};

