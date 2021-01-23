#include <syscall.h>
#include <stdio.h>

#define _s

struct meminfo{
    unsigned long addr;
    unsigned int  length;
};

int main()
{
    int num=0;
    struct meminfo *umem;
    umem = (struct meminfo *)malloc(1,sizeof(*umem));

    void *sendbuf;
    sendbuf = malloc(1024,sizeof(int));
    umem.addr = (unsigned long)sendbuf;
    umem.addr = 4*1024;
    num = syscall(327,(unsigned long)umem);
    printf("num:%d\n",num);
   
    return 0;
}