#include "k-mmap.h"

MODULE_VERSION("0.0.1");
MODULE_AUTHOR("HS");
MODULE_LICENSE("GPL");

/*char dev definition*/
#define KM_GET_PHYADDR (unsigned int) 0
#if 0
#define KERNEL_CM_RECV (unsigned int) 1
#define PRINT_MAC	   (unsigned int) 6

#define CONFIG_PBU	   (unsigned int) 3
#define PRINT_PGU	   (unsigned int) 4
#define PRINT_PHD	   (unsigned int) 5

#define DESTROY_ADDR   (unsigned int) 7
#define GET_REG        (unsigned int) 8
#endif

static int major;
static struct class *class;
static struct class_device *km_device;


/*system call function*/
#define __NR_syscall 335 /*add system call 335*/
int orig_cr0;
unsigned long *sys_call_table = 0;
static int (*anything_saved)(void);


static int km_mmap(struct file *file, struct vm_area_struct *vma)
{
    TRACE_PRINT();

}

static int km_get_phyaddr(struct km_umem *umem)
{
    unsigned long addr = umem->address;
    size_t size  = umem->length;
    unsigned long npages;
    unsigned long cur_base;
    int ret;
    struct scatterlist *sg, *sg_list_start;
    struct page **page_list;
    int i;

    TRACE_PRINT();

    if((addr + size) < addr || PAGE_ALIGN(addr+size) < (addr+size))
        return ERR_PTR(-EINVAL);

    page_list = (struct page **)__get_free_page(GFP_KERNEL);
    if(!page_list)
    {
        ret = -ENOMEM;
        return ERR_PTR(ret);
    }

    npages = km_umem_num_pages(umem);
    if(npages == 0 || napges > UINT_MAX){
        ret = -EINVAL;
        return ERR_PTR(ret);
    }
    
    ret = sg_alloc_table(&umem->sg_head,npages,GFP_KERNEL);
    if(ret)
        return ERR_PTR(ret);

    sg_list_start = umem->sg_head.sgl;
    cur_base = addr & PAGE_MASK;
    while(napges){
           ret =  get_user_pages_fast(cur_base,min_t(unsigned long,npages,PAGE_SIZE/sizeof(struct page *)),0,page_list);
           if(ret < 0)
           {
               ret = -EINVAL;
               return ERR_PTR(ret);
           }

           umem->npages += ret;
           cur_base     += ret * PAGE_SIZE；
           npages       -= ret;

           for_each_sg(sg_list_start,sg,ret,i){
               sg_set_page(sg,page_list[i],PAGE_SIZE,0);
           }
           sg_list_start = sg;
    }
    
    ret = 0;
    free_page((unsigned long) page_list);
    return ret;

}

static long km_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    TRACE_PRINT();
    struct km_init info_user;
    struct km_umem *umem;

    struct pid *pid;
    struct task_struct *task;
    struct mm_struct *mm;
    struct vm_area_struct *vma;
    struct page *page;

    int ret;

    umem = kzalloc(sizeof(*umem),GFP_KERNEL);
    switch(cmd){
        case KM_GET_PHYADDR:
            copy_from_user(&info_user,(const void __user *)arg,sizeof(struct km_init));
            umem->address = info_user.addr_start;
            umem->length  = info_user.len;
            umem->page_shift = PAGE_SHIFT;
            ret = km_get_phyaddr(umem);
            if(ret)
            {
                kfree(umem);
            }
        

    }
    return 0;

}

static struct file_operations km_ops = {
    .owner          = THIS_MODULE,
    .mmap           = km_mmap,
    .llseek         = default_llseek,
    .fsync          = noop_fsync,
    .unlocked_ioctl   = km_ioctl,
};

/*setting cr0 register's 17th zero*/

unsigned int clear_and_return_cr0(void)	
{
   	unsigned int cr0 = 0;
   	unsigned int ret;
    /* 前者用在32位系统。后者用在64位系统，本系统64位 */
    //asm volatile ("movl %%cr0, %%eax" : "=a"(cr0));	
   	asm volatile ("movq %%cr0, %%rax" : "=a"(cr0));	/* 将cr0寄存器的值移动到rax寄存器中，同时输出到cr0变量中 */
    ret = cr0;
	cr0 &= 0xfffeffff;	/* 将cr0变量值中的第17位清0，将修改后的值写入cr0寄存器 */
	//asm volatile ("movl %%eax, %%cr0" :: "a"(cr0));
	asm volatile ("movq %%rax, %%cr0" :: "a"(cr0));	/* 读取cr0的值到rax寄存器，再将rax寄存器的值放入cr0中 */
	return ret;
}


void setback_cr0(unsigned int val)
{
    asm volatile ("movq %%rax, %%cr0" :: "a"(val));
}

static int sys_mycall(void)
{
    int ret = 12345;
    printk("syscall success!");
    return ret;
}

static int __init km_init(void)
{
    TRACE_PRINT();

    /*create chrdev for ioctl*/
    major = register_chrdev(0,"km-dev",&km_ops);
    class = class_create(THIS_MODULE,"km-dev");

    km_device = (struct class_device *)device_create(class,NULL,MKDEV(major,0),NULL,"km-dev");

    /*add system call process*/
    sys_call_table = (unsigned long *)kallsyms_lookup_name("sys_call_table");
    printk("sys_call_table:0x%p\n",sys_call_table);
    anything_saved = (int(*)(void))(sys_call_table[__NR_syscall]);
    orig_cr0 = clear_and_return_cr0();
    sys_call_table[__NR_syscall] = (unsigned long)&sys_mycall;
    setback_cr0(orig_cr0);

    return 0;
}

static void __exit km_exit(void)
{
    TRACE_PRINT();
    /*destroy chrdev*/
    device_unregister((void *)km_device);
    class_destroy((struct class *)class);

    unregister_chrdev(major,"km-dev");

    /*add syscall */
    orig_cr0 = clear_and_return_cr0();
        sys_call_table[__NR_syscall] = (unsigned long)anything_saved;
        setback_cr0(orig_cr0);
    printk("My syscall exit...\n");
}

module_init(km_init);
module_exit(km_exit);
