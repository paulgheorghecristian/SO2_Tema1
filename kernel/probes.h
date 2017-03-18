#ifndef PROBES_H
#define PROBES_H

#include <linux/kprobes.h>

extern struct kretprobe kmalloc_probe;

struct kmalloc_allocated_size{
	long size;
};

#endif
