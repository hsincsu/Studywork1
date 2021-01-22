#ifndef __K_MMAP_H__
#define __K_MMAP_H__


#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/dma-mapping.h>
#include <slab.h>
#include <linux/sched.h>
#include <linux/pid.h>
#include <linux/kallsyms.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/mmu_context.h>
#include <linux/workqueue.h>
#include <linux/mm_types.h>
#include <linux/sched/mm.h>
#include <linux/sched/task.h>
#include <linux/ioctl.h>
#include <linux/types.h>

struct km_init
{
    unsigned long       pid;
    unsigned long       addr_start;
    int                 len;

};

struct km_umem
{
    unsigned long address;
    size_t        length;
    int           page_shift;
    int           npages;
    struct sg_table sg_head;
};

static inline unsigned long km_umem_offset(struct km_umem *umem)
{
    return umem->address & (BIT(umem->page_shift) - 1);
}

static inline unsigned long km_umem_start(struct km_umem *umem)
{
    return umem->address - km_umem_offset(umem);
}

static inline unsigned long km_umem_end(struct km_umem *umem)
{
    return ALIGN(umem->address + umem->length, BIT(umem->page_shift));
}

static inline size_t km_umem_num_pages(struct km_umem *umem)
{
    return(km_umem_end(umem) - km_umem_start(umem)) >> umem->page_shift;
}


unsigned int clear_and_return_cr0(void);
void setback_cr0(unsigned int val);
static int sys_mycall(void);

#define DEBUG_TRACE 1

#if DEBUG_TRACE
    #define TRACE_PRINT(fmt, args...)  pr_alert("TRACE_PRINT: [%s,%d]" fmt, __func__, __LINE__, ## args)
#else
    #define TRACE_PRINT(x...)          do { } while (0)
#endif

#endif