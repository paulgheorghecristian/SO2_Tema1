#ifndef PROBES_H
#define PROBES_H

#include "commons.h"

struct kmalloc_allocated_size {
	long size;
};

void add_to_result(int pid, enum results result, long value);

#endif
