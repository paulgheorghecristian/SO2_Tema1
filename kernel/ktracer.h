#ifndef K_TRACER_H
#define K_TRACER_H

#include <asm/ioctl.h>
#include <linux/types.h>
#define LOG_LEVEL KERN_DEBUG

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

#endif /* K_TRACER_H */
