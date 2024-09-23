/*
*    ------ BEGIN LICENSE ATTRIBUTION ------
*    
*    Portions of this file have been appropriated or derived from the following project(s) and therefore require attribution to the original licenses and authors.
*    
*    Repository: https://github.com/OpenMPDK/SMDK
*    Source File: lib/linux-5.18-rc3-smdk/kernel/events/callchain.c
*    Licenses:
*      GNU General Public License v2.0 only
*      SPDXId: GPL-2.0-only
*    
*    Auto-attribution by Threatrix, Inc.
*    
*    ------ END LICENSE ATTRIBUTION ------
*/
truct callchain_cpus_entries {
	struct rcu_head			rcu_head;
	struct perf_callchain_entry	*cpu_entries[];
};

int sysctl_perf_event_max_stack __read_mostly = PERF_MAX_STACK_DEPTH;
int sysctl_perf_event_max_contexts_per_stack __read_mostly = PERF_MAX_CONTEXTS_PER_STACK;

static inline size_t perf_callchain_entry__sizeof(void)
{
	return (sizeof(struct perf_callchain_entry) +
		sizeof(__u64) * (sysctl_perf_event_max_stack +
				 sysctl_perf_event_max_contexts_per_stack));
}

static DEFINE_PER_CPU(int, callchain_recursion[PERF_NR_CONTEXTS]);
static atomic_t nr_callchain_events;
static DEFINE_MUTEX(callchain_mutex);
static struct callchain_cpus_entries *callchain_cpus_entries;


__weak void perf_callchain_kernel(struct perf_callchain_entry_ctx *entry,
				  struct pt_regs *regs)
{
}

__weak void perf_callchain_user(struct perf_callchain_entry_ctx *entry,
				struct pt_regs *regs)
{
}

static void release_callchain_buffers_rcu(struct rcu_head *head)
{
	struct callchain_cpus_entries *entries;
	int cpu;

	entries = container_of(head, struct callchain_cpus_entries, rcu_head);

	for_each_possible_cpu(cpu)
		kfree(entries->cpu_entries[cpu]);

	kfree(entries);
}

static void release_callchain_buffers(void)
{
	struct callchain_cpus_entries *entries;

	entries = callchain_cpus_entries;
	RCU_INIT_POINTER(callchain_cpus_entries, NULL);
	call_rcu(&entries->rcu_head, release_callchain_buffers_rcu);
}


