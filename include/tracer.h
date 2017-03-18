/*
 * SO2 kprobe based tracer header file
 * 2013, Operating Systems 2 - Ixia Challenge
 *
 * this is shared with user space
 */

#ifndef TRACER_H__
#define TRACER_H__ 1

#include <asm/ioctl.h>
#include <linux/types.h>
#ifndef __KERNEL__
#include <sys/types.h>
#endif /* __KERNEL__ */

#define TRACER_DEV_MINOR 42
#define TRACER_DEV_NAME "tracer"

#define TRACER_ADD_PROCESS	_IOW(_IOC_WRITE, 42, pid_t)
#define TRACER_REMOVE_PROCESS	_IOW(_IOC_WRITE, 43, pid_t)

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

#endif /* TRACER_H_ */
