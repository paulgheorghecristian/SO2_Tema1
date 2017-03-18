#ifndef COMMONS_H
#define COMMONS_H

#include <linux/kprobes.h>
#include <linux/hashtable.h>
#define HASHTABLE_LOG_SIZE 16

enum results{
	KMALLOC_CALLS = 0,
	KFREE_CALLS,
	KMALLOC_MEM,
	KFREE_MEM,
	SCHED_CALLS,
	UP_CALLS,
	DOWN_CALLS,
	LOCK_CALLS,
	UNLOCK_CALLS,
	NUM_RESULTS
};

struct hashtable_entry{
	int pid;
	atomic64_t results[NUM_RESULTS];
	struct hlist_node hnode;
};

extern struct kretprobe kmalloc_probe;
extern DECLARE_HASHTABLE(procs, HASHTABLE_LOG_SIZE);

#endif
