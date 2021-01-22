#include <stdio.h>

int main()
{
    int num=0;
    num = syscall();
    printf("num:%d\n",num);
    return 0;
}