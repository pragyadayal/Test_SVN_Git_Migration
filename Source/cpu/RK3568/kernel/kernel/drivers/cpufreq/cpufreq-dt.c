/*
 * Copyright (C) 2012 Freescale Semiconductor, Inc.
 *
 * Copyright (C) 2014 Linaro.
 * Viresh Kumar <viresh.kumar@linaro.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#define pr_fmt(fmt)	KBUILD_MODNAME ": " fmt

#include <linux/clk.h>
#include <linux/cpu.h>
#include <linux/cpufreq.h>
#include <linux/cpumask.h>
#include <linux/energy_model.h>
#include <linux/err.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/pm_opp.h>
#include <linux/platform_device.h>
#include <linux/regulator/consumer.h>
#include <linux/slab.h>
#include <linux/thermal.h>

#include "cpufreq-dt.h"
#ifdef CONFIG_ARCH_ROCKCHIP
#include "rockchip-cpufreq.h"
#include <soc/rockchip/rockchip_ipa.h>
#include <soc/rockchip/rockchip_system_monitor.h>
#endif

struct private_data {
	struct opp_table *opp_table;
	struct device *cpu_dev;
#ifdef CONFIG_ARCH_ROCKCHIP
	struct monitor_dev_info *mdev_info;
	struct monitor_dev_profile *mdevp;
#endif
	const char *reg_name;
	bool have_static_opps;
};

static struct freq_attr *cpufreq_dt_attr[] = {
	&cpufreq_freq_attr_scaling_available_freqs,
	NULL,   /* Extra space for boost-attr if required */
	NULL,
};

static int set_target(struct cpufreq_policy *policy, unsigned int index)
{
	struct private_data *priv = policy->driver_data;
	unsigned long freq = policy->freq_table[index].frequency;
	int ret;

#ifdef CONFIG_ROCKCHIP_SYSTEM_MONITOR
	if (priv->mdev_info)
		ret = rockchip_monitor_opp_set_rate(priv->mdev_info,
						    freq * 1000);
	else
		ret = dev_pm_opp_set_rate(priv->cpu_dev, freq * 1000);
#else
	ret = dev_pm_opp_set_rate(priv->cpu_dev, freq * 1000);
#endif

	if (!ret) {
		arch_set_freq_scale(policy->related_cpus, freq,
				    policy->cpuinfo.max_freq);
	}

	return ret;
}

/*
 * An earlier version of opp-v1 bindings used to name the regulator
 * "cpu0-supply", we still need to handle that for backwards compatibility.
 */
static const char *find_supply_name(struct device *dev)
{
	struct device_node *np;
	struct property *pp;
	int cpu = dev->id;
	const char *name = NULL;

	np = of_node_get(dev->of_node);

	/* This must be valid for sure */
	if (WARN_ON(!np))
		return NULL;

	/* Try "cpu0" for older DTs */
	if (!cpu) {
		pp = of_find_property(np, "cpu0-supply", NULL);
		if (pp) {
			name = "cpu0";
			goto node_put;
		}
	}

	pp = of_find_property(np, "cpu-supply", NULL);
	if (pp) {
		name = "cpu";
		//printk("cpu-supply.......\n");
		goto node_put;
	}
	
	dev_dbg(dev, "no regulator for cpu%d\n", cpu);

node_put:
	of_node_put(np);
	return name;
}

/*
 * An earlier version of opp-v1 bindings used to name the regulator
 * "cpu0-supply", we still need to handle that for backwards compatibility.
 */
static const char *find_supply_name_rk860(struct device *dev)
{
	struct device_node *np;
	struct property *pp;
	int cpu = dev->id;
	const char *name = NULL;

	np = of_node_get(dev->of_node);

	/* This must be valid for sure */
	if (WARN_ON(!np))
		return NULL;

	/* Try "cpu0" for older DTs */
	if (!cpu) {
		pp = of_find_property(np, "cpu0-supply", NULL);
		if (pp) {
			name = "cpu0";
			goto node_put;
		}
	}
	
	pp = of_find_property(np, "cpu-rk860-supply", NULL);
	if (pp) {
		name = "cpu-rk860";
		//printk("cpu-rk860-supply.......\n");;
		goto node_put;
	}

	dev_dbg(dev, "no regulator for cpu%d\n", cpu);

node_put:
	of_node_put(np);
	return name;
}

static int resources_available(void)
{
	struct device *cpu_dev;
	struct regulator *cpu_reg;
	struct clk *cpu_clk;
	int ret = 0;
	const char *name;
	int rk860_chip_enable = 0;

	cpu_dev = get_cpu_device(0);
	if (!cpu_dev) {
		pr_err("failed to get cpu0 device\n");
		return -ENODEV;
	}

	cpu_clk = clk_get(cpu_dev, NULL);
	ret = PTR_ERR_OR_ZERO(cpu_clk);
	if (ret) {
		/*
		 * If cpu's clk node is present, but clock is not yet
		 * registered, we should try defering probe.
		 */
		if (ret == -EPROBE_DEFER)
			dev_dbg(cpu_dev, "clock not ready, retry\n");
		else
			dev_err(cpu_dev, "failed to get clock: %d\n", ret);

		return ret;
	}

	clk_put(cpu_clk);

	name = find_supply_name(cpu_dev);
	/* Platform doesn't require regulator */
	if (name)
	{
		cpu_reg = regulator_get_optional(cpu_dev, name);
		ret = PTR_ERR_OR_ZERO(cpu_reg);
		if (ret) {
			/*
			 * If cpu's regulator supply node is present, but regulator is
			 * not yet registered, we should try defering probe.
			 */
			if (ret == -EPROBE_DEFER)
				printk("cpu0 regulator not ready, retry\n");
			else
				printk("no regulator for cpu0: %d\n", ret);
			
			//return ret;
			rk860_chip_enable = 1;
		}

	}
	else
	{
		rk860_chip_enable = 1;
	}


	if(1 == rk860_chip_enable)
	{
		name = find_supply_name_rk860(cpu_dev);
		/* Platform doesn't require regulator */
		if (!name)
			return 0;

		cpu_reg = regulator_get_optional(cpu_dev, name);
		ret = PTR_ERR_OR_ZERO(cpu_reg);
		if (ret) {
			/*
			 * If cpu's regulator supply node is present, but regulator is
			 * not yet registered, we should try defering probe.
			 */
			if (ret == -EPROBE_DEFER)
				printk("cpu0 regulator not ready, retry\n");
			else
				printk("no regulator for cpu0: %d\n", ret);

			return ret;
		}
	}
	
	regulator_put(cpu_reg);
	return 0;
}

static int cpufreq_init(struct cpufreq_policy *policy)
{
	struct em_data_callback em_cb = EM_DATA_CB(of_dev_pm_opp_get_cpu_power);
	struct cpufreq_frequency_table *freq_table;
	struct opp_table *opp_table = NULL;
	struct private_data *priv;
	struct device *cpu_dev;
	struct clk *cpu_clk;
	unsigned int transition_latency;
	bool fallback = false;
	const char *name;
	int ret, nr_opp;
	int rk860_chip_enable = 0;

	cpu_dev = get_cpu_device(policy->cpu);
	if (!cpu_dev) {
		pr_err("failed to get cpu%d device\n", policy->cpu);
		return -ENODEV;
	}

	cpu_clk = clk_get(cpu_dev, NULL);
	if (IS_ERR(cpu_clk)) {
		ret = PTR_ERR(cpu_clk);
		dev_err(cpu_dev, "%s: failed to get clk: %d\n", __func__, ret);
		return ret;
	}

	/* Get OPP-sharing information from "operating-points-v2" bindings */
	ret = dev_pm_opp_of_get_sharing_cpus(cpu_dev, policy->cpus);
	if (ret) {
		if (ret != -ENOENT)
			goto out_put_clk;

		/*
		 * operating-points-v2 not supported, fallback to old method of
		 * finding shared-OPPs for backward compatibility if the
		 * platform hasn't set sharing CPUs.
		 */
		if (dev_pm_opp_get_sharing_cpus(cpu_dev, policy->cpus))
			fallback = true;
	}

	/*
	 * OPP layer will be taking care of regulators now, but it needs to know
	 * the name of the regulator first.
	 */
	name = find_supply_name(cpu_dev);
	if (name) {
		opp_table = dev_pm_opp_set_regulators(cpu_dev, &name, 1);
		if (IS_ERR(opp_table)) {
			ret = PTR_ERR(opp_table);
			dev_err(cpu_dev, "Failed to set regulator for cpu%d: %d\n",
				policy->cpu, ret);
			//goto out_put_clk;
			rk860_chip_enable = 1;
		}
	}
	else
	{
		rk860_chip_enable = 1;
	}

	if(1 == rk860_chip_enable)
	{
		opp_table = NULL;
		name = find_supply_name_rk860(cpu_dev);
		if (name) {
			opp_table = dev_pm_opp_set_regulators(cpu_dev, &name, 1);
			if (IS_ERR(opp_table)) {
				ret = PTR_ERR(opp_table);
				dev_err(cpu_dev, "Failed to set regulator for cpu%d: %d\n",
					policy->cpu, ret);
				goto out_put_clk;
			}
		}
	}
	
	priv = kzalloc(sizeof(*priv), GFP_KERNEL);
	if (!priv) {
		ret = -ENOMEM;
		goto out_put_regulator;
	}

	priv->reg_name = name;
	priv->opp_table = opp_table;

	/*
	 * Initialize OPP tables for all policy->cpus. They will be shared by
	 * all CPUs which have marked their CPUs shared with OPP bindings.
	 *
	 * For platforms not using operating-points-v2 bindings, we do this
	 * before updating policy->cpus. Otherwise, we will end up creating
	 * duplicate OPPs for policy->cpus.
	 *
	 * OPPs might be populated at runtime, don't check for error here
	 */
#ifdef CONFIG_ARCH_ROCKCHIP
	rockchip_cpufreq_set_opp_info(cpu_dev);
	ret = dev_pm_opp_of_add_table(cpu_dev);
	if (ret) {
		dev_err(cpu_dev, "couldn't find opp table for cpu:%d, %d\n",
			policy->cpu, ret);
	} else {
		struct cpumask cpus;

		cpumask_copy(&cpus, policy->cpus);
		cpumask_clear_cpu(policy->cpu, &cpus);
		if (!cpumask_empty(&cpus)) {
			if (!dev_pm_opp_of_cpumask_add_table(&cpus))
				priv->have_static_opps = true;
			else
				dev_pm_opp_of_remove_table(cpu_dev);
		} else {
			priv->have_static_opps = true;
		}
	}
	rockchip_cpufreq_adjust_power_scale(cpu_dev);
#else
	if (!dev_pm_opp_of_cpumask_add_table(policy->cpus))
		priv->have_static_opps = true;
#endif

	/*
	 * But we need OPP table to function so if it is not there let's
	 * give platform code chance to provide it for us.
	 */
	ret = dev_pm_opp_get_opp_count(cpu_dev);
	if (ret <= 0) {
		dev_dbg(cpu_dev, "OPP table is not ready, deferring probe\n");
		ret = -EPROBE_DEFER;
		goto out_free_opp;
	}
	nr_opp = ret;

	if (fallback) {
		cpumask_setall(policy->cpus);

		/*
		 * OPP tables are initialized only for policy->cpu, do it for
		 * others as well.
		 */
		ret = dev_pm_opp_set_sharing_cpus(cpu_dev, policy->cpus);
		if (ret)
			dev_err(cpu_dev, "%s: failed to mark OPPs as shared: %d\n",
				__func__, ret);
	}

	ret = dev_pm_opp_init_cpufreq_table(cpu_dev, &freq_table);
	if (ret) {
		dev_err(cpu_dev, "failed to init cpufreq table: %d\n", ret);
		goto out_free_opp;
	}

	priv->cpu_dev = cpu_dev;
	policy->driver_data = priv;
	policy->clk = cpu_clk;
	policy->freq_table = freq_table;

	policy->suspend_freq = dev_pm_opp_get_suspend_opp_freq(cpu_dev) / 1000;

	/* Support turbo/boost mode */
	if (policy_has_boost_freq(policy)) {
		/* This gets disabled by core on driver unregister */
		ret = cpufreq_enable_boost_support();
		if (ret)
			goto out_free_cpufreq_table;
		cpufreq_dt_attr[1] = &cpufreq_freq_attr_scaling_boost_freqs;
	}

	transition_latency = dev_pm_opp_get_max_transition_latency(cpu_dev);
	if (!transition_latency)
		transition_latency = CPUFREQ_ETERNAL;

	policy->cpuinfo.transition_latency = transition_latency;
	policy->dvfs_possible_from_any_cpu = true;

	em_register_perf_domain(policy->cpus, nr_opp, &em_cb);

#ifdef CONFIG_ARCH_ROCKCHIP
	priv->mdevp = kzalloc(sizeof(*priv->mdevp), GFP_KERNEL);
	if (!priv->mdevp)
		goto check_rate_volt;
	priv->mdevp->type = MONITOR_TPYE_CPU;
	priv->mdevp->low_temp_adjust = rockchip_monitor_cpu_low_temp_adjust;
	priv->mdevp->high_temp_adjust = rockchip_monitor_cpu_high_temp_adjust;
	cpumask_copy(&priv->mdevp->allowed_cpus, policy->cpus);
	priv->mdev_info = rockchip_system_monitor_register(cpu_dev,
							   priv->mdevp);
	if (IS_ERR(priv->mdev_info)) {
		kfree(priv->mdevp);
		priv->mdevp = NULL;
		dev_dbg(priv->cpu_dev,
			"running cpufreq without system monitor\n");
		priv->mdev_info = NULL;
	}
check_rate_volt:
	rockchip_cpufreq_check_rate_volt(cpu_dev);
#endif
	return 0;

out_free_cpufreq_table:
	dev_pm_opp_free_cpufreq_table(cpu_dev, &freq_table);
out_free_opp:
	if (priv->have_static_opps)
		dev_pm_opp_of_cpumask_remove_table(policy->cpus);
	kfree(priv);
out_put_regulator:
	if (name)
		dev_pm_opp_put_regulators(opp_table);
out_put_clk:
	clk_put(cpu_clk);

	return ret;
}

static int cpufreq_exit(struct cpufreq_policy *policy)
{
	struct private_data *priv = policy->driver_data;

#ifdef CONFIG_ARCH_ROCKCHIP
	rockchip_cpufreq_suspend(policy);
	rockchip_system_monitor_unregister(priv->mdev_info);
	kfree(priv->mdevp);
	priv->mdevp = NULL;
#endif
	dev_pm_opp_free_cpufreq_table(priv->cpu_dev, &policy->freq_table);
	if (priv->have_static_opps)
		dev_pm_opp_of_cpumask_remove_table(policy->related_cpus);
	if (priv->reg_name)
		dev_pm_opp_put_regulators(priv->opp_table);
#ifdef CONFIG_ARCH_ROCKCHIP
	rockchip_cpufreq_put_opp_info(priv->cpu_dev);
#endif
	clk_put(policy->clk);
	kfree(priv);

	return 0;
}

static struct cpufreq_driver dt_cpufreq_driver = {
	.flags = CPUFREQ_STICKY | CPUFREQ_NEED_INITIAL_FREQ_CHECK |
		 CPUFREQ_IS_COOLING_DEV,
	.verify = cpufreq_generic_frequency_table_verify,
	.target_index = set_target,
	.get = cpufreq_generic_get,
	.init = cpufreq_init,
	.exit = cpufreq_exit,
	.name = "cpufreq-dt",
	.attr = cpufreq_dt_attr,
	.suspend = cpufreq_generic_suspend,
};

static int dt_cpufreq_probe(struct platform_device *pdev)
{
	struct cpufreq_dt_platform_data *data = dev_get_platdata(&pdev->dev);
	int ret;

	/*
	 * All per-cluster (CPUs sharing clock/voltages) initialization is done
	 * from ->init(). In probe(), we just need to make sure that clk and
	 * regulators are available. Else defer probe and retry.
	 *
	 * FIXME: Is checking this only for CPU0 sufficient ?
	 */
	ret = resources_available();
	if (ret)
		return ret;

	if (data) {
		if (data->have_governor_per_policy)
			dt_cpufreq_driver.flags |= CPUFREQ_HAVE_GOVERNOR_PER_POLICY;

		dt_cpufreq_driver.resume = data->resume;
		if (data->suspend)
			dt_cpufreq_driver.suspend = data->suspend;
	}

	ret = cpufreq_register_driver(&dt_cpufreq_driver);
	if (ret)
		dev_err(&pdev->dev, "failed register driver: %d\n", ret);

	return ret;
}

static int dt_cpufreq_remove(struct platform_device *pdev)
{
	cpufreq_unregister_driver(&dt_cpufreq_driver);
	return 0;
}

static struct platform_driver dt_cpufreq_platdrv = {
	.driver = {
		.name	= "cpufreq-dt",
	},
	.probe		= dt_cpufreq_probe,
	.remove		= dt_cpufreq_remove,
};
module_platform_driver(dt_cpufreq_platdrv);

MODULE_ALIAS("platform:cpufreq-dt");
MODULE_AUTHOR("Viresh Kumar <viresh.kumar@linaro.org>");
MODULE_AUTHOR("Shawn Guo <shawn.guo@linaro.org>");
MODULE_DESCRIPTION("Generic cpufreq driver");
MODULE_LICENSE("GPL");
