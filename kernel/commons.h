#ifndef COMMONS_H
#define COMMONS_H

#include <linux/kprobes.h>
#include <linux/hashtable.h>
#include <linux/list.h>
#include <linux/slab.h>
#define HASHTABLE_LOG_SIZE 16

enum results {
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

struct addr_size_assoc_list_node {
	long size, address;
	struct list_head list;
};

struct hashtable_entry {
	int pid;
	atomic64_t results[NUM_RESULTS];
	struct list_head addr_size_assoc_list_head;
	struct hlist_node hnode;
};

extern struct kretprobe kmalloc_probe;
extern struct jprobe kfree_probe;
extern struct jprobe schedule_probe;
extern struct jprobe up_probe;
extern struct jprobe down_probe;
extern struct jprobe lock_probe;
extern struct jprobe unlock_probe;
extern DECLARE_HASHTABLE(procs, HASHTABLE_LOG_SIZE);

#endif
