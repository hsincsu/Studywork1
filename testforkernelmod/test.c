#include <syscall.h>
#include <stdio.h>

int main()
{
    int num=0;
    num = syscall(335);
    printf("num:%d\n",num);
    return 0;
}