#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>

#include <linux/slab.h>
#include <linux/list.h>

#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#include <asm/uaccess.h>

#include <linux/string.h>
#include <linux/miscdevice.h>

#include "../include/tracer.h"
#include "ktracer.h"

MODULE_DESCRIPTION("Tema 2");
MODULE_AUTHOR("Paul Cristian Gheorghe");
MODULE_LICENSE("GPL");

#define procfs_file_read	"tracer"

DEFINE_HASHTABLE(procs, HASHTABLE_LOG_SIZE);

void add_node_to_hashtable(int pid){
	struct hashtable_entry *new_node;
	int i;

	new_node = kmalloc(sizeof(struct hashtable_entry), GFP_KERNEL);
	new_node->pid = pid;
	for (i = 0; i < NUM_RESULTS; i++) {
		atomic64_set(&new_node->results[i], 0);
	}
	INIT_LIST_HEAD(&new_node->addr_size_assoc_list_head);
	hash_add(procs, &new_node->hnode, new_node->pid);
}

void del_node_from_hashtable(int pid){
	struct hashtable_entry *current_node;
	struct hlist_node *i;

	hash_for_each_possible_safe(procs, current_node, i, hnode, pid) {
		if (current_node->pid != pid) {
			continue;
		}
		hash_del(&current_node->hnode);
		kfree(current_node);
		return;
	}
}

static int tracer_open(struct inode *in, struct file *filp)
{
	printk(LOG_LEVEL "The tracer device was opened!\n");
	return 0;
}

static int tracer_release(struct inode *in, struct file *filp)
{
	printk(LOG_LEVEL "The tracer device was closed!\n");
	return 0;
}

static long tracer_ioctl (struct file *file, unsigned int cmd,
	unsigned long pid)
{
	int ret = 0;

	printk(LOG_LEVEL "Process %ld ", pid);
	switch (cmd) {
		case TRACER_ADD_PROCESS:
			add_node_to_hashtable(pid);
			break;
		case TRACER_REMOVE_PROCESS:
			del_node_from_hashtable(pid);
			break;
		default:
			return -ENOTTY;
	}
	return ret;
}

static const struct file_operations tracer_fops = {
	.owner = THIS_MODULE,
	.open = tracer_open,
	.release = tracer_release,
	.unlocked_ioctl = tracer_ioctl
};

static struct miscdevice tracer_device = {
	.minor = TRACER_DEV_MINOR,
	.name = TRACER_DEV_NAME,
	.fops = &tracer_fops
};

static struct proc_dir_entry *proc_tracer;

static int tracer_proc_show(struct seq_file *m, void *v)
{
	struct hashtable_entry *i;
	int bkt;

	seq_puts(m,"PID  kmalloc kfree kmalloc_mem kfree_mem sched up down lock unlock\n");
	hash_for_each(procs, bkt, i, hnode){
		char buf[100];
		sprintf(buf,"%d %lld %lld %lld %lld %lld %lld %lld %lld %lld\n",
			i->pid,
			atomic64_read(&i->results[KMALLOC_CALLS]),
			atomic64_read(&i->results[KFREE_CALLS]),
			atomic64_read(&i->results[KMALLOC_MEM]),
			atomic64_read(&i->results[KFREE_MEM]),
			atomic64_read(&i->results[SCHED_CALLS]),
			atomic64_read(&i->results[UP_CALLS]),
			atomic64_read(&i->results[DOWN_CALLS]),
			atomic64_read(&i->results[LOCK_CALLS]),
			atomic64_read(&i->results[UNLOCK_CALLS]));
		seq_puts(m, buf);
	}

	return 0;
}

static int tracer_read_open(struct inode *inode, struct  file *file)
{
	return single_open(file, tracer_proc_show, NULL);
}

static const struct file_operations r_fops = {
	.owner		= THIS_MODULE,
	.open		= tracer_read_open,
	.read		= seq_read,
	.release	= single_release
};

static int tracer_init(void)
{
	hash_init(procs);

	proc_tracer = proc_create(procfs_file_read, 0000, NULL, &r_fops);
	if (!proc_tracer) {
		return -ENOMEM;
	}

	if (misc_register(&tracer_device)) {
		return -EINVAL;
	}

	if (register_kretprobe(&kmalloc_probe) < 0) {
		return -1;
	}

	if (register_jprobe(&kfree_probe) < 0) {
		return -1;
	}

	if (register_jprobe(&schedule_probe) < 0) {
		return -1;
	}

	if (register_jprobe(&up_probe) < 0) {
		return -1;
	}

	if (register_jprobe(&down_probe) < 0) {
		return -1;
	}

	if (register_jprobe(&lock_probe) < 0) {
		return -1;
	}

	if (register_jprobe(&unlock_probe) < 0) {
		return -1;
	}

	return 0;
}

static void tracer_exit(void)
{
	int bkt;
	struct hashtable_entry *current_node;
	struct hlist_node *i;

	hash_for_each_safe(procs, bkt, i, current_node, hnode){
		hash_del(&current_node->hnode);
		kfree(current_node);
	}
	proc_remove(proc_tracer);
	misc_deregister(&tracer_device);

	unregister_kretprobe(&kmalloc_probe);
	unregister_jprobe(&kfree_probe);
	unregister_jprobe(&schedule_probe);
	unregister_jprobe(&up_probe);
	unregister_jprobe(&down_probe);
	unregister_jprobe(&lock_probe);
	unregister_jprobe(&unlock_probe);
}

module_init(tracer_init);
module_exit(tracer_exit);
