#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cpufreq.h>
#include <linux/thermal.h>
#include <linux/platform_device.h>
#include <linux/of_platform.h>
#include <linux/cpufreq.h>
#include <linux/sys_config.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/cpu.h>
#include <linux/cpumask.h>
#include <linux/sunxi-sid.h>
#include "sunxi-budget-cooling.h"

#ifdef CONFIG_SUNXI_BUDGET_COOLING_VFTBL
struct cpufreq_dvfs_s {
	unsigned int freq;   /* cpu frequency */
	unsigned int volt;   /* voltage for the frequency */
};

static struct cpufreq_dvfs_s c0_vftable_syscfg[16];
static struct cpufreq_dvfs_s c1_vftable_syscfg[16];
static unsigned int c0_table_length;
static unsigned int c1_table_length;

#if defined(CONFIG_ARCH_SUN9IW1) || defined(CONFIG_ARCH_SUN8IW6)
static char *multi_cluster_prefix[] = {"L_LV", "B_LV"};
#else
static char *single_cluster_prefix[] = {"LV"};
#endif

static int sunxi_cpu_budget_vftable_init(void)
{
	u32 val;
	struct cpufreq_dvfs_s *ptable;
	unsigned int *plen;
	unsigned int freq;
	int i, j, k, cluster_nr = 1, ret = -1;
	char vftbl_name[16] = {0};
	char name[16] = {0};
	unsigned int vf_table_type = 0;
	unsigned int vf_table_count = 0;
	char **prefix;
	struct device_node *main_np;
	struct device_node *sub_np;

#if defined(CONFIG_ARCH_SUN9IW1) || defined(CONFIG_ARCH_SUN8IW6)
	cluster_nr = 2;
	prefix = multi_cluster_prefix;
#else
	cluster_nr = 1;
	prefix = single_cluster_prefix;
#endif
	main_np = of_find_node_by_path("/dvfs_table");
	if (!main_np) {
		pr_info("%s:No dvfs table node found\n", __func__);
		ret = -ENODEV;
		goto fail;
	}

	if (of_property_read_u32(main_np, "vf_table_count", &vf_table_count)) {
		pr_info("%s:get vf_table_count failed\n", __func__);
		return -EINVAL;
	}

	if (vf_table_count == 1) {
		pr_info("%s:support only one vf_table\n", __func__);
		/* Now we use the other method to reading the cpufreq data*/
		return 0;
	} else {
		vf_table_type = sunxi_get_soc_bin();
	}

	pr_info("%s:soc_bin = %d\n", __func__, vf_table_type);

	sprintf(vftbl_name, "%s%d", "allwinner,vf_table", vf_table_type);

	sub_np = of_find_compatible_node(main_np, NULL, vftbl_name);
	if (!sub_np) {
		pr_err("No %s node found\n", vftbl_name);
		ret = -ENODEV;
		goto fail;
	}


	for (k = 0; k < cluster_nr; k++) {
		ptable = k ? &c1_vftable_syscfg[0] : &c0_vftable_syscfg[0];
		plen =  k ? &c1_table_length : &c0_table_length;
		sprintf(name, "%s_count", prefix[k]);

		if (of_property_read_u32(sub_np, name, &val)) {
			pr_err("fetch %s from dt failed\n", name);
			ret = -EINVAL;
			goto fail;
		}

		*plen = val;

		if (*plen >= 16) {
			pr_err("%s from dt is out of bounder\n", name);
			goto fail;
		}

		for (i = 1, j = 0; i <= *plen; i++) {
			sprintf(name, "%s%d_freq", prefix[k], i);
			if (of_property_read_u32(sub_np, name, &val)) {
				pr_err("fetch %s from dt failed\n", name);
				ret = -EINVAL;
				goto fail;
			}
			freq = val;

			if (freq) {
				ptable[j].freq = freq / 1000;
				j++;
			}
		}
		*plen = j;
	}
	return 0;
fail:
	return ret;
}

void multi_vf_table_conver(struct sunxi_cpufreq_cooling_table *tbl,
			   int vol_freq_id,
			   int cluster,
			   int cooler_num)
{
	if (cluster == 0) {
		if (vol_freq_id < c0_table_length)
			tbl[cooler_num].cluster_freq[cluster] =
					c0_vftable_syscfg[vol_freq_id].freq;
		else
			tbl[cooler_num].cluster_freq[cluster] =
				c0_vftable_syscfg[c0_table_length - 1].freq;
		return;
	} else if (cluster == 1) {
		if (vol_freq_id < c1_table_length)
			tbl[cooler_num].cluster_freq[cluster] =
					c1_vftable_syscfg[vol_freq_id].freq;
		else
			tbl[cooler_num].cluster_freq[cluster] =
				c1_vftable_syscfg[c1_table_length - 1].freq;
		return;
	}

}

#endif

static int is_cpufreq_valid(unsigned int cpu)
{
	struct cpufreq_policy policy;
	return !cpufreq_get_policy(&policy, cpu);
}

int sunxi_cpufreq_update_state(struct sunxi_budget_cooling_device *cooling_device, u32 cluster)
{
	unsigned long flags;
	s32 ret = 0;
	u32 cpuid;
	u32 cooling_state = cooling_device->cooling_state;
	struct cpufreq_policy policy;
	struct sunxi_budget_cpufreq *cpufreq = cooling_device->cpufreq;
	if(NULL == cpufreq)
		return 0;

	spin_lock_irqsave(&cpufreq->lock, flags);

	cpufreq->cluster_freq_limit[cluster] = cpufreq->tbl[cooling_state].cluster_freq[cluster];

	spin_unlock_irqrestore(&cpufreq->lock, flags);

	for_each_cpu(cpuid, &cooling_device->cluster_cpus[cluster]) {
		if (is_cpufreq_valid(cpuid)){
			if((cpufreq_get_policy(&policy, cpuid) == 0) &&
							policy.user_policy.governor){
				ret = cpufreq_update_policy(0);
				break;
			}
		}
	}

	return ret;
}
EXPORT_SYMBOL(sunxi_cpufreq_update_state);

int sunxi_cpufreq_get_roomage(struct sunxi_budget_cooling_device *cooling_device,
				u32 *freq_floor, u32 *freq_roof, u32 cluster)
{
	struct sunxi_budget_cpufreq *cpufreq = cooling_device->cpufreq;
	if(NULL == cpufreq)
		return 0;
	*freq_floor = cpufreq->cluster_freq_floor[cluster];
	*freq_roof = cpufreq->cluster_freq_roof[cluster];
	return 0;
}
EXPORT_SYMBOL(sunxi_cpufreq_get_roomage);

int sunxi_cpufreq_set_roomage(struct sunxi_budget_cooling_device *cooling_device,
				u32 freq_floor, u32 freq_roof, u32 cluster)
{
	unsigned long flags;
	struct sunxi_budget_cpufreq *cpufreq = cooling_device->cpufreq;
	if(NULL == cpufreq)
		return 0;
	spin_lock_irqsave(&cpufreq->lock, flags);

	cpufreq->cluster_freq_floor[cluster] = freq_floor;
	cpufreq->cluster_freq_roof[cluster] = freq_roof;

	spin_unlock_irqrestore(&cpufreq->lock, flags);
	sunxi_cpufreq_update_state(cooling_device, cluster);
	return 0;
}
EXPORT_SYMBOL(sunxi_cpufreq_set_roomage);

static int cpufreq_thermal_notifier(struct notifier_block *nb,
					unsigned long event, void *data)
{
	struct sunxi_budget_cpufreq *cpufreq = container_of(nb ,struct sunxi_budget_cpufreq, notifer);
	struct sunxi_budget_cooling_device *bcd = cpufreq->bcd;
	struct cpufreq_policy *policy = data;
	int cluster = 0;
	unsigned long limit_freq=0,base_freq=0,head_freq=0;
	unsigned long max_freq=0,min_freq=0;

	if (event != CPUFREQ_ADJUST || cpufreq == NOTIFY_INVALID)
		return 0;

	while(cluster < bcd->cluster_num){
		if (cpumask_test_cpu(policy->cpu, &bcd->cluster_cpus[cluster])){
			limit_freq = cpufreq->cluster_freq_limit[cluster];
			base_freq = cpufreq->cluster_freq_floor[cluster];
			head_freq = cpufreq->cluster_freq_roof[cluster];
			break;
		}else
			cluster ++;
	}
	if(cluster == bcd->cluster_num)
		return 0;

	if(limit_freq && limit_freq != INVALID_FREQ)
	{
		max_freq =(head_freq >= limit_freq)?limit_freq:head_freq;
		min_freq = base_freq;
		/* Never exceed policy.max*/
		if (max_freq > policy->max)
			max_freq = policy->max;
		if (min_freq < policy->min)
		{
			min_freq = policy->min;
		}
		min_freq = (min_freq < max_freq)?min_freq:max_freq;
		if (policy->max != max_freq || policy->min != min_freq )
		{
			cpufreq_verify_within_limits(policy, min_freq, max_freq);
			policy->user_policy.max = policy->max;
			pr_info("CPU Budget:update CPU %d cpufreq max to %lu min to %lu\n",policy->cpu,max_freq, min_freq);
		}
	}
	return 0;
}

/**
 * sunxi_cpufreq_cooling_parse - parse the cpufreq limit value and fill in struct sunxi_cpufreq_cooling_table
 */
static struct sunxi_cpufreq_cooling_table *
sunxi_cpufreq_cooling_parse(struct device_node *np, u32 tbl_num, u32 cluster_num)
{
	struct sunxi_cpufreq_cooling_table *tbl;
	u32 i, j, ret = 0;
	char name[32];
	u32 value = 0;

#if defined(CONFIG_SUNXI_BUDGET_COOLING_VFTBL)
	if (sunxi_cpu_budget_vftable_init()) {
		pr_err("cooling_dev:do the vftable init fail.");
		return NULL;
	}
#endif

	tbl = kzalloc(tbl_num * sizeof(*tbl), GFP_KERNEL);
	if (IS_ERR_OR_NULL(tbl)) {
		pr_err("cooling_dev: not enough memory for cpufreq cooling table\n");
		return NULL;
	}
	for (i = 0; i < tbl_num; i++) {
		sprintf(name, "state%d", i);
		for (j = 0; j < cluster_num; j++) {

#if defined(CONFIG_SUNXI_BUDGET_COOLING_VFTBL)
			if (of_property_read_u32_index(np, (const char *)&name,
							(j * 2), &value)) {
				pr_err("node %s get failed!\n", name);
				ret = -EBUSY;
			}
			multi_vf_table_conver(tbl, value, j, i);
			pr_err("tbl[%d].cluster_freq[%d] = %u\n", i, j,
					tbl[i].cluster_freq[j]);
#else
			if (of_property_read_u32_index(np, (const char *)&name,
					(j * 2), &(tbl[i].cluster_freq[j]))) {
				pr_err("node %s get failed!\n", name);
				ret = -EBUSY;
			}
#endif
		}
	}
	if (ret) {
		kfree(tbl);
		tbl = NULL;
	}
	return tbl;
}

struct sunxi_budget_cpufreq *
sunxi_cpufreq_cooling_register(struct sunxi_budget_cooling_device *bcd)
{
	struct sunxi_budget_cpufreq *cpufreq;
	struct sunxi_cpufreq_cooling_table *tbl;
	struct device_node *np = bcd->dev->of_node;
	u32 cluster;

	cpufreq = kzalloc(sizeof(*cpufreq), GFP_KERNEL);
	if (IS_ERR_OR_NULL(cpufreq)) {
		pr_err("cooling_dev: not enough memory for cpufreq cooling data\n");
		goto fail;
	}

	tbl = sunxi_cpufreq_cooling_parse(np, bcd->state_num, bcd->cluster_num);
	if (!tbl) {
		kfree(cpufreq);
		goto fail;
	}
	cpufreq->tbl_num = bcd->state_num;
	cpufreq->tbl = tbl;
	spin_lock_init(&cpufreq->lock);
	for (cluster = 0; cluster < bcd->cluster_num; cluster++) {
			cpufreq->cluster_freq_limit[cluster] = cpufreq->tbl[0].cluster_freq[cluster];
			cpufreq->cluster_freq_roof[cluster] = cpufreq->cluster_freq_limit[cluster];
	}

	cpufreq->notifer.notifier_call=cpufreq_thermal_notifier;
	cpufreq_register_notifier(&(cpufreq->notifer),
						CPUFREQ_POLICY_NOTIFIER);
	cpufreq->bcd = bcd;
	pr_info("CPU freq cooling register Success\n");
	return cpufreq;
fail:
	return NULL;
}
EXPORT_SYMBOL(sunxi_cpufreq_cooling_register);

void sunxi_cpufreq_cooling_unregister(struct sunxi_budget_cooling_device *bcd)
{
	if(!bcd->cpufreq)
		return;
	cpufreq_unregister_notifier(&(bcd->cpufreq->notifer),
						CPUFREQ_POLICY_NOTIFIER);
	kfree(bcd->cpufreq->tbl);
	kfree(bcd->cpufreq);
	bcd->cpufreq = NULL;
	return;
}
EXPORT_SYMBOL(sunxi_cpufreq_cooling_unregister);

