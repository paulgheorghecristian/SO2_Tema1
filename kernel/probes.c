#include "probes.h"

int kmalloc_handler(struct kretprobe_instance *ri, struct pt_regs *regs) {

}

int kmalloc_entry_handler(struct kretprobe_instance *ri, struct pt_regs *regs) {
	struct kmalloc_allocated_size *kmalloc_size;

	kmalloc_size = (kmalloc_allocated_size*) ri->data;
	kmalloc_size->size = regs->ax;
}

struct kretprobe kmalloc_probe {
	.kp = {
		.symbol_name = "__kmalloc"
	},
	.entry_handler = kmalloc_entry_handler,
	.handler = kmalloc_handler,
	.maxactive = 20,
	.data_size = sizeof(struct kmalloc_allocated_size)
};
