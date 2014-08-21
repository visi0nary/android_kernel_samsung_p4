#ifdef __KERNEL__
#ifndef _ASM_POWERPC_IRQ_H
#define _ASM_POWERPC_IRQ_H

/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <linux/threads.h>
#include <linux/list.h>
#include <linux/radix-tree.h>

#include <asm/types.h>
#include <linux/atomic.h>


/* Define a way to iterate across irqs. */
#define for_each_irq(i) \
	for ((i) = 0; (i) < NR_IRQS; ++(i))

extern atomic_t ppc_n_lost_interrupts;

/* This number is used when no interrupt has been assigned */
#define NO_IRQ			(0)

/* This is a special irq number to return from get_irq() to tell that
 * no interrupt happened _and_ ignore it (don't count it as bad). Some
 * platforms like iSeries rely on that.
 */
#define NO_IRQ_IGNORE		((unsigned int)-1)

/* Total number of virq in the platform */
#define NR_IRQS		CONFIG_NR_IRQS

/* Number of irqs reserved for the legacy controller */
#define NUM_ISA_INTERRUPTS	16

/* Same thing, used by the generic IRQ code */
#define NR_IRQS_LEGACY		NUM_ISA_INTERRUPTS

/* This type is the placeholder for a hardware interrupt number. It has to
 * be big enough to enclose whatever representation is used by a given
 * platform.
 */
typedef unsigned long irq_hw_number_t;

/* Interrupt controller "host" data structure. This could be defined as a
 * irq domain controller. That is, it handles the mapping between hardware
 * and virtual interrupt numbers for a given interrupt domain. The host
 * structure is generally created by the PIC code for a given PIC instance
 * (though a host can cover more than one PIC if they have a flat number
 * model). It's the host callbacks that are responsible for setting the
 * irq_chip on a given irq_desc after it's been mapped.
 */
struct irq_host;
struct radix_tree_root;

/* Functions below are provided by the host and called whenever a new mapping
 * is created or an old mapping is disposed. The host can then proceed to
 * whatever internal data structures management is required. It also needs
 * to setup the irq_desc when returning from map().
 */
struct irq_host_ops {
	/* Match an interrupt controller device node to a host, returns
	 * 1 on a match
	 */
	int (*match)(struct irq_host *h, struct device_node *node);

	/* Create or update a mapping between a virtual irq number and a hw
	 * irq number. This is called only once for a given mapping.
	 */
	int (*map)(struct irq_host *h, unsigned int virq, irq_hw_number_t hw);

	/* Dispose of such a mapping */
	void (*unmap)(struct irq_host *h, unsigned int virq);

	/* Translate device-tree interrupt specifier from raw format coming
	 * from the firmware to a irq_hw_number_t (interrupt line number) and
	 * type (sense) that can be passed to set_irq_type(). In the absence
	 * of this callback, irq_create_of_mapping() and irq_of_parse_and_map()
	 * will return the hw number in the first cell and IRQ_TYPE_NONE for
	 * the type (which amount to keeping whatever default value the
	 * interrupt controller has for that line)
	 */
	int (*xlate)(struct irq_host *h, struct device_node *ctrler,
		     const u32 *intspec, unsigned int intsize,
		     irq_hw_number_t *out_hwirq, unsigned int *out_type);
};

struct irq_host {
	struct list_head	link;

	/* type of reverse mapping technique */
	unsigned int		revmap_type;
#define IRQ_HOST_MAP_LEGACY     0 /* legacy 8259, gets irqs 1..15 */
#define IRQ_HOST_MAP_NOMAP	1 /* no fast reverse mapping */
#define IRQ_HOST_MAP_LINEAR	2 /* linear map of interrupts */
#define IRQ_HOST_MAP_TREE	3 /* radix tree */
	union {
		struct {
			unsigned int size;
			unsigned int *revmap;
		} linear;
		struct radix_tree_root tree;
	} revmap_data;
	struct irq_host_ops	*ops;
	void			*host_data;
	irq_hw_number_t		inval_irq;

	/* Optional device node pointer */
	struct device_node	*of_node;
};

struct irq_data;
extern irq_hw_number_t irqd_to_hwirq(struct irq_data *d);
extern irq_hw_number_t virq_to_hw(unsigned int virq);
extern bool virq_is_host(unsigned int virq, struct irq_host *host);

/**
 * irq_early_init - Init irq remapping subsystem
 */
extern void irq_early_init(void);

static __inline__ int irq_canonicalize(int irq)
{
	return irq;
}

extern int distribute_irqs;

struct irqaction;
struct pt_regs;

#define __ARCH_HAS_DO_SOFTIRQ

#if defined(CONFIG_BOOKE) || defined(CONFIG_40x)
/*
 * Per-cpu stacks for handling critical, debug and machine check
 * level interrupts.
 */
extern struct thread_info *critirq_ctx[NR_CPUS];
extern struct thread_info *dbgirq_ctx[NR_CPUS];
extern struct thread_info *mcheckirq_ctx[NR_CPUS];
extern void exc_lvl_ctx_init(void);
#else
#define exc_lvl_ctx_init()
#endif

/*
 * Per-cpu stacks for handling hard and soft interrupts.
 */
extern struct thread_info *hardirq_ctx[NR_CPUS];
extern struct thread_info *softirq_ctx[NR_CPUS];

extern void irq_ctx_init(void);
extern void call_do_softirq(struct thread_info *tp);
extern int call_handle_irq(int irq, void *p1,
			   struct thread_info *tp, void *func);
extern void do_IRQ(struct pt_regs *regs);

int irq_choose_cpu(const struct cpumask *mask);

#endif /* _ASM_IRQ_H */
#endif /* __KERNEL__ */
