#include "csapp.h"
#include "stdio.h"

int main()
{
    int fd1,fd2;
    char c;

    fd1 = open("foobar.txt", O_RDONLY, 0);
    fd2 = open("foobar.txt", O_RDONLY, 0);

    printf("fd1 = %d, fd2 = %d\n", fd1, fd2);
/*
    if (fork()  == 0)
    {
	read(fd1, &c, 1);
	exit(0);
    }
*/
    //wait(NULL);
    read(fd1, &c, 1);
    read(fd1, &c, 1);
    printf("old c: %c\n", c);
    dup2(fd2, fd1);
    read(fd1, &c, 1);
    printf("c = %c\n",c);
    exit(0);
}
