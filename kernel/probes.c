#include "probes.h"

void add_to_result(int pid, enum results result, long value)
{
	struct hashtable_entry *current_entry;

	hash_for_each_possible(procs, current_entry, hnode, pid) {
		if (current_entry->pid != pid)
			continue;

		atomic64_add(value, &current_entry->results[result]);
		break;
	}
}

static int kmalloc_handler(struct kretprobe_instance *ri, struct pt_regs *regs)
{
	struct kmalloc_allocated_size *kmalloc_size;
	long returned_address;
	struct hashtable_entry *current_node;
	struct addr_size_assoc_list_node *new_list_node;

	kmalloc_size = (struct kmalloc_allocated_size *) ri->data;
	returned_address = regs_return_value(regs);
	add_to_result(current->pid, KMALLOC_MEM, kmalloc_size->size);

	hash_for_each_possible(procs, current_node, hnode, current->pid) {
		if (current_node->pid != current->pid)
			continue;

		new_list_node = kmalloc(sizeof(struct addr_size_assoc_list_node), GFP_ATOMIC);
		if (new_list_node == NULL)
			return 0;

		new_list_node->size = kmalloc_size->size;
		new_list_node->address = returned_address;
		list_add(&new_list_node->list, &current_node->addr_size_assoc_list_head);
		break;
	}
	return 0;
}

static int kmalloc_entry_handler(struct kretprobe_instance *ri, struct pt_regs *regs)
{
	struct kmalloc_allocated_size *kmalloc_size;

	add_to_result(current->pid, KMALLOC_CALLS, 1);

	kmalloc_size = (struct kmalloc_allocated_size *) ri->data;
	kmalloc_size->size = regs->ax;

	return 0;
}

static void kfree_handler(const void *objp)
{
	long arg_address = (long)objp;
	struct hashtable_entry *current_node;
	struct addr_size_assoc_list_node *current_list_node;

	hash_for_each_possible(procs, current_node, hnode, current->pid) {
		if (current_node->pid != current->pid)
			continue;

		list_for_each_entry(current_list_node, &current_node->addr_size_assoc_list_head, list) {
			if (current_list_node->address == arg_address) {
				add_to_result(current->pid, KFREE_MEM, current_list_node->size);
				add_to_result(current->pid, KFREE_CALLS, 1);
				jprobe_return();
				return;
			}
		}
	}
	jprobe_return();
}

asmlinkage void __sched schedule_handler(void)
{
	add_to_result(current->pid, SCHED_CALLS, 1);
	jprobe_return();
}

static void up_handler(struct semaphore *sem)
{
	add_to_result(current->pid, UP_CALLS, 1);
	jprobe_return();
}

static int down_handler(struct semaphore *sem)
{
	add_to_result(current->pid, DOWN_CALLS, 1);
	jprobe_return();
	return 0;
}

static void __sched lock_handler(struct mutex *m, unsigned int subclass)
{
	add_to_result(current->pid, LOCK_CALLS, 1);
	jprobe_return();
}



static void __sched unlock_handler(struct mutex *m)
{
	add_to_result(current->pid, UNLOCK_CALLS, 1);
	jprobe_return();
}

struct kretprobe kmalloc_probe = {
	.kp = {
		.symbol_name = "__kmalloc"
	},
	.entry_handler = kmalloc_entry_handler,
	.handler = kmalloc_handler,
	.maxactive = 20,
	.data_size = sizeof(struct kmalloc_allocated_size)
};

struct jprobe kfree_probe = {
	.kp = {
		.symbol_name = "kfree"
	},
	.entry = kfree_handler
};

struct jprobe schedule_probe = {
	.kp = {
		.symbol_name = "schedule"
	},
	.entry = schedule_handler
};

struct jprobe up_probe = {
	.kp = {
		.symbol_name = "up"
	},
	.entry = up_handler
};

struct jprobe down_probe = {
	.kp = {
		.symbol_name = "down_interruptible"
	},
	.entry = down_handler
};

struct jprobe lock_probe = {
	.kp = {
		.symbol_name = "mutex_lock_nested"
	},
	.entry = lock_handler
};

struct jprobe unlock_probe = {
	.kp = {
		.symbol_name = "mutex_unlock"
	},
	.entry = unlock_handler
};
