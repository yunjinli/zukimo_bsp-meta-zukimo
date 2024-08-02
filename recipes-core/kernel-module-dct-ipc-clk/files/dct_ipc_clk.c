/*
 * dct_ipc_clk.c, by Dream Chip
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/clkdev.h>
#include <linux/clk-provider.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/list.h>
#include <dt-bindings/clock/zukimo_clock.h>

#include <dct_ipc.h>
#include <dct_ipc_fsp.h>

#define COMPAT_PHASE 1

#define DST IPC_FSP
#define CHANNEL 0
#define IPC_RETRY_TIMEOUT_MS 20u

#define CLK_CACHE_SIZE 16

#define DIVIDE_AND_ROUND_UINT32(n, d) ((min((n), (U32_MAX - (d) / 2u)) + ((d) / 2u)) / (d))

struct clk_rounding_cache {
	struct list_head list;
	uint32_t requested_rate;
	uint32_t rounded_rate;
};

struct dct_ipc_clk {
	struct device *dev;
	struct platform_device *ipc_dev;
	struct platform_device *cpufreq;
	struct clk_hw hw;
	uint32_t clk_id;
	uint32_t rate;
	bool on;
	struct list_head cache; /* a LRU cache to hold the last rounded clock rates */
	bool clk_off_disabled;
};

static int send_ipc_clk_cmd(struct platform_device *ipc_dev, struct ipc_header *tx_msg, struct ipc_header *rx_msg, uint32_t rx_data_len)
{
	int rc = dct_ipc_send_receive_timeout(ipc_dev, IPC_FSP, CHANNEL, tx_msg, rx_msg, rx_data_len, IPC_RETRY_TIMEOUT_MS);

	if (rc) {
		pr_err("dct_ipc_send_receive_timeout failed with rc = %d\n", rc);
	}

	return rc;
}

static int ipc_clk_cache_init(struct dct_ipc_clk *clkdata, size_t n_cache_entries)
{
	INIT_LIST_HEAD(&clkdata->cache);

	while (n_cache_entries--) {
		struct clk_rounding_cache *cache_entry = kmalloc(sizeof(struct clk_rounding_cache), GFP_KERNEL);

		if (cache_entry == NULL) {
			return -ENOMEM;
		}

		cache_entry->requested_rate = 0;
		cache_entry->rounded_rate = 0;
		INIT_LIST_HEAD(&cache_entry->list);

		list_add(&cache_entry->list, &clkdata->cache);
	}

	return 0;
}

static uint32_t ipc_clk_cache_lookup(struct dct_ipc_clk *clkdata, uint32_t requested_rate)
{
	struct clk_rounding_cache *cache_entry;
	uint32_t rounded_rate = 0u;

	list_for_each_entry(cache_entry, &clkdata->cache, list) {
		if ((cache_entry->rounded_rate != 0) && (cache_entry->requested_rate == requested_rate)) {
			rounded_rate = cache_entry->rounded_rate;
			break;
		}
	}

	if (rounded_rate) {
		/* move entry to the top of the list since this is an lru cache */
		list_del_init(&cache_entry->list);
		list_add(&cache_entry->list, &clkdata->cache);
	}

	return rounded_rate;
}

static void ipc_clk_cache_release(struct dct_ipc_clk *clkdata)
{
	struct clk_rounding_cache *cache_entry;
	struct clk_rounding_cache *temp;

	list_for_each_entry_safe(cache_entry, temp, &clkdata->cache, list) {
		list_del(&cache_entry->list);
		kfree(cache_entry);
	}
}

static void ipc_clk_cache_update(struct dct_ipc_clk *clkdata, uint32_t requested_rate, uint32_t rounded_rate)
{
	struct clk_rounding_cache *cache_entry = list_last_entry(&clkdata->cache, struct clk_rounding_cache, list);
	struct clk_rounding_cache *temp_entry;

	list_for_each_entry(temp_entry, &clkdata->cache, list) {
		if ((temp_entry->rounded_rate == 0) || ((temp_entry->rounded_rate == rounded_rate) && (temp_entry->requested_rate == requested_rate))) {
			/* either found an unused element
			 * or (unusual case): a client called update on a previous cache hit
			 */
			cache_entry = temp_entry;
			break;
		}
	}

	/* If the loop over the list didn't find a match (which is kind of expected)
	 * cache_entry is initialized to the last element of the list. This means
	 * we will evict the oldest element and replace it with the updated one.
	 * If a match (or empty element) was found, the element is simply replaced
	 * and inserted at the head of the list
	 */
	list_del_init(&cache_entry->list);
	cache_entry->requested_rate = requested_rate;
	cache_entry->rounded_rate = rounded_rate;
	list_add(&cache_entry->list, &clkdata->cache);
}

static uint32_t ipc_clk_round_rate(struct dct_ipc_clk *clkdata, uint32_t requested_rate, uint8_t round_mode)
{
	uint32_t rounded_rate = ipc_clk_cache_lookup(clkdata, requested_rate);

	if (rounded_rate == 0) {
		struct ipc_fsp_clk_round_rate rx_msg;
		struct ipc_fsp_clk_round_rate tx_msg;
		uint32_t max_rx_data_len;
		int rc = 0;

		memset(&rx_msg, 0, sizeof(rx_msg));
		memset(&tx_msg, 0, sizeof(tx_msg));
		tx_msg.hdr.cmd = IPC_FSP_CLK_ROUND_RATE;
		tx_msg.hdr.len = sizeof(struct  ipc_fsp_clk_round_rate) - sizeof(struct ipc_header);
		tx_msg.clk = clkdata->clk_id;
		tx_msg.rate_khz = DIVIDE_AND_ROUND_UINT32(requested_rate, 1000u);
		tx_msg.round_mode = round_mode;
		max_rx_data_len = sizeof(struct  ipc_fsp_clk_round_rate) - sizeof(struct ipc_header);
		if ((rc = send_ipc_clk_cmd(clkdata->ipc_dev,
						(struct ipc_header *)&tx_msg,
						(struct ipc_header *)&rx_msg,
						max_rx_data_len)) != 0) {
			return 0;
		}

		rounded_rate = rx_msg.rate_khz * 1000u;
		ipc_clk_cache_update(clkdata, requested_rate, rounded_rate);
	}

	return rounded_rate;
}

static int ipc_clk_update_state(struct dct_ipc_clk *clkdata)
{
	struct ipc_fsp_clk_state rx_clk_state;
	struct ipc_fsp_clk_state tx_clk_state;
	uint32_t max_rx_data_len;
	int rc = 0;

	memset(&rx_clk_state, 0, sizeof(rx_clk_state));
	memset(&tx_clk_state, 0, sizeof(tx_clk_state));
	tx_clk_state.hdr.cmd = IPC_FSP_CLK_GET_STATE;
	tx_clk_state.hdr.len = sizeof(struct ipc_fsp_clk_state) - sizeof(struct ipc_header);
	tx_clk_state.clk = clkdata->clk_id;
	max_rx_data_len = sizeof(struct ipc_fsp_clk_state) - sizeof(struct ipc_header);
	if ((rc = send_ipc_clk_cmd(clkdata->ipc_dev,
					(struct ipc_header *)&tx_clk_state,
					(struct ipc_header *)&rx_clk_state,
					max_rx_data_len)) != 0) {
		return rc;
	}

	clkdata->on = rx_clk_state.on ? true : false;
	clkdata->rate = rx_clk_state.rate_khz * 1000;

	pr_debug("%s, id: %u, on: %s, rate: %u\n", __func__,
			clkdata->clk_id,
			clkdata->on ? "true" : "false",
			clkdata->rate);
	return 0;
}
static int ipc_clk_set_state(struct dct_ipc_clk *clkdata, bool on, uint32_t rate)
{
	struct ipc_fsp_clk_state rx_clk_state;
	struct ipc_fsp_clk_state tx_clk_state;
	uint32_t max_rx_data_len;
	int rc = 0;

	memset(&rx_clk_state, 0, sizeof(rx_clk_state));
	memset(&tx_clk_state, 0, sizeof(tx_clk_state));
	tx_clk_state.hdr.cmd = IPC_FSP_CLK_SET_STATE;
	tx_clk_state.hdr.len = sizeof(struct ipc_fsp_clk_state) - sizeof(struct ipc_header);
	tx_clk_state.rate_khz = rate / 1000;
	tx_clk_state.clk = clkdata->clk_id;
	tx_clk_state.on = on ? 1 : 0;
	max_rx_data_len = sizeof(struct ipc_fsp_clk_state) - sizeof(struct ipc_header);
	if ((rc = send_ipc_clk_cmd(clkdata->ipc_dev,
					(struct ipc_header *)&tx_clk_state,
					(struct ipc_header *)&rx_clk_state,
					max_rx_data_len)) != 0) {
		return rc;
	}

	clkdata->on = on;
	clkdata->rate = rate;

	pr_debug("%s, id: %u, on: %s, rate: %u\n", __func__,
			clkdata->clk_id,
			clkdata->on ? "true" : "false",
			clkdata->rate);
	return 0;
}

static int dct_ipc_clk_is_on(struct clk_hw *hw)
{
	struct dct_ipc_clk *clkdata =
		container_of(hw, struct dct_ipc_clk, hw);

	pr_info("%s, id: %u, on: %s, rate: %u\n", __func__,
			clkdata->clk_id,
			clkdata->on ? "true" : "false",
			clkdata->rate);
	return (int)clkdata->on;
}

static int dct_ipc_clk_on(struct clk_hw *hw)
{
	struct dct_ipc_clk *clkdata =
		container_of(hw, struct dct_ipc_clk, hw);
	int rc = 0;

	if (clkdata->on) {
		pr_debug("%s, id: %u - clk is already on\n",
				__func__, clkdata->clk_id);
		return 0;
	}

	if ((rc = ipc_clk_set_state(clkdata, true, clkdata->rate)) != 0) {
		return rc;
	}

	pr_info("Clock %u, switched on.\n", clkdata->clk_id);
	return 0;
}

static void dct_ipc_clk_off(struct clk_hw *hw)
{
	struct dct_ipc_clk *clkdata =
		container_of(hw, struct dct_ipc_clk, hw);

	if (!clkdata->on) {
		pr_debug("%s, id: %u - clk is already off\n",
				__func__, clkdata->clk_id);
		return;
	} else if (clkdata->clk_off_disabled) {
		pr_err("%s, id: %u - no support for turning clk off\n",
				__func__, clkdata->clk_id);
		return;
	}

	if (ipc_clk_set_state(clkdata, false, clkdata->rate) == 0) {
		pr_info("Clock %u, switched off.\n", clkdata->clk_id);
	}
}

static unsigned long dct_ipc_clk_get_rate(struct clk_hw *hw,
					     unsigned long parent_rate)
{
	struct dct_ipc_clk *clkdata =
		container_of(hw, struct dct_ipc_clk, hw);

	return clkdata->on ? clkdata->rate : 0;
}

static long dct_ipc_clk_round_rate(struct clk_hw *hw, unsigned long rate,
        unsigned long *parent_rate)
{
	struct dct_ipc_clk *clkdata =
		container_of(hw, struct dct_ipc_clk, hw);
	uint32_t rounded_rate;

	if ((rounded_rate = ipc_clk_round_rate(clkdata, (uint32_t) rate, ipc_fsp_clk_round_nearest)) == 0) {
		return -EINVAL;
	}

	pr_debug("%s, r: %lu --> %u\n", __func__, rate, rounded_rate);
	return (long) rounded_rate;
}

static int dct_ipc_clk_set_rate(struct clk_hw *hw, unsigned long rate,
				   unsigned long parent_rate)
{
	struct dct_ipc_clk *clkdata =
		container_of(hw, struct dct_ipc_clk, hw);
	int rc = 0;

	if ((rc = ipc_clk_set_state(clkdata, clkdata->on, rate)) != 0) {
		return rc;
	}

	pr_info("Clock %u, new rate: %lu.\n", clkdata->clk_id, rate);
	return rate;
}

static const struct clk_ops dct_ipc_clk_ops = {
	.is_prepared = dct_ipc_clk_is_on,
	.prepare = dct_ipc_clk_on,
	.unprepare = dct_ipc_clk_off,
	.recalc_rate = dct_ipc_clk_get_rate,
	.round_rate = dct_ipc_clk_round_rate,
	.set_rate = dct_ipc_clk_set_rate,
};

static int dct_ipc_clk_probe(struct platform_device *pdev)
{
	struct dct_ipc_clk *clkdata;
	struct device *dev = &pdev->dev;
	struct clk_init_data init = {};
	struct platform_device *ipc_dev;
	unsigned long min;
	unsigned long max;
	int ret;

	ipc_dev = dct_devm_ipc_get_byphandle(dev, dev->of_node, "dct-ipc");
	if (IS_ERR(ipc_dev)) {
		if (PTR_ERR(ipc_dev) == -EAGAIN) {
			dev_info(dev, "waiting for dct-ipc device\n");
			return -EPROBE_DEFER;
		} else {
			dev_err(dev, "failed to retrieve dct-ipc device\n");
			return -EINVAL;
		}
	}
	if (WARN_ON(!ipc_dev)) {
		return -EINVAL;
	}

	clkdata = devm_kzalloc(dev, sizeof(*clkdata), GFP_KERNEL);
	if (!clkdata) {
		return -ENOMEM;
	}

	clkdata->dev = dev;
	clkdata->ipc_dev = ipc_dev;
	if (of_property_read_u32(dev->of_node, "clock-id", &clkdata->clk_id) != 0) {
		dev_err(dev, "failed to read clock id\n");
		return -EINVAL;
	}

	platform_set_drvdata(pdev, clkdata);

	if (of_property_read_string(dev->of_node, "clock-output-names", &init.name) != 0) {
		devm_kasprintf(dev, GFP_KERNEL, "dct-ipc-clk-%02u", clkdata->clk_id);
	}
	init.ops = &dct_ipc_clk_ops;
	init.flags = CLK_GET_RATE_NOCACHE;
	init.num_parents = 0;
	clkdata->hw.init = &init;

	if ((ret = ipc_clk_update_state(clkdata)) != 0) {
		return ret;
	}

#if defined(COMPAT_PHASE)
	if (clkdata->on) {
		/* don't allow a clock to be turned off if it was turned on by bootloader */
		clkdata->clk_off_disabled = true;
	}
#endif

	ret = ipc_clk_cache_init(clkdata, CLK_CACHE_SIZE);
	if (ret < 0) {
		return ret;
	}

	if ((min = ipc_clk_round_rate(clkdata, 0u, ipc_fsp_clk_round_nearest)) == 0) {
		ret = -EINVAL;
		goto release_cache;
	}

	if ((max = ipc_clk_round_rate(clkdata, U32_MAX, ipc_fsp_clk_round_nearest)) == 0) {
		ret = -EINVAL;
		goto release_cache;
	}

	ret = devm_clk_hw_register(dev, &clkdata->hw);
	if (ret < 0) {
		goto release_cache;
	}
	devm_of_clk_add_hw_provider(dev, of_clk_hw_simple_get, &clkdata->hw);

	pr_info("Registered clock %u with range: %lu - %lu\n", clkdata->clk_id, min, max);
	clk_hw_set_rate_range(&clkdata->hw, min, max);

	if (clkdata->clk_id == ZUK_CLK_PLL_APU) {
		/* register the cpu-freq driver if this is the CPU clk */
		clkdata->cpufreq = platform_device_register_data(dev, "dct-cpufreq",
				-1, NULL, 0);
		clkdata->clk_off_disabled = true; /* don't allow CPU clock to be turned off */
	}

	return 0;

release_cache:
	ipc_clk_cache_release(clkdata);
	return ret;
}

static int dct_ipc_clk_remove(struct platform_device *pdev)
{
	struct dct_ipc_clk *clkdata = platform_get_drvdata(pdev);

	if (clkdata->cpufreq) {
		platform_device_unregister(clkdata->cpufreq);
	}

	ipc_clk_cache_release(clkdata);

	return 0;
}

static const struct of_device_id dct_ipc_clk_match[] = {
	{ .compatible = "dct,ipc-clk" },
	{ },
};
MODULE_DEVICE_TABLE(of, dct_ipc_clk_match);

static struct platform_driver dct_ipc_clk_driver = {
	.driver = {
		.name = "dct-ipc-clk",
		.of_match_table = dct_ipc_clk_match,
	},
	.probe = dct_ipc_clk_probe,
	.remove = dct_ipc_clk_remove,
};
module_platform_driver(dct_ipc_clk_driver);

MODULE_DESCRIPTION("DCT ipc clock driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:dct-ipc-clk");
