#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <sys/user.h>
#include <sys/ptrace.h>
#include <syscall.h>

#define SYSCALLS_SIZE 321
#define COMMAND_BUFF_SIZE 64


struct systemCall
{
    int num;
    int limit;
    int count;
    char* name;
};

struct myCalls {
    int num;
    char* name;
};

struct myCalls syscalls[SYSCALLS_SIZE];

void printMap();
void mapSystemCalls(char* args);
int wait_for_syscall(pid_t child);
void setSyscallNum(struct systemCall *node);