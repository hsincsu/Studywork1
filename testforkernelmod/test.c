#include <syscall.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/uio.h>


#define __NR_mycall 327

struct meminfo{
    unsigned long addr;
    unsigned long length;
};

struct scatterlist {
        unsigned long   page_link;
        unsigned int    offset;
        unsigned int    length;
        unsigned long   dma_address;
        unsigned int    dma_length;

};


struct sg_table {
        struct scatterlist *sgl;        /* the list */
        unsigned int nents;             /* number of mapped entries */
        unsigned int orig_nents;        /* original size of list */
};

struct km_umem
{
    unsigned long address;
    size_t        length;
    int           page_shift;
    int           npages;
    struct sg_table sg_head;
};


int main()
{
    int num=0;
    struct meminfo *umem;
    struct km_umem *umem2;
    umem = (struct meminfo *)malloc(1*sizeof(*umem));
    umem2 = (struct km_umem *)malloc(1*sizeof(*umem2));

    void *sendbuf;
    sendbuf = calloc(1,8192*sizeof(int));
    umem->addr = (unsigned long)sendbuf;
    umem->length = 8192*sizeof(int);
    printf("umem addr:0x%p, uemm2 addr:0x%lx\n,sendbuf:0x%lx",umem,(uintptr_t)umem2,umem->addr);
    num = syscall(__NR_mycall,umem->addr,umem->length,(uintptr_t)umem2);
    printf("num:%d\n",num);
    printf("umem2->addr,umem2->length:0x%lx,0x%lx\n",umem2->address,umem2->length);
   
    return 0;
}