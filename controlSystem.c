#include "controlSystem.h"

int wait_for_syscall(pid_t child) {
    
    int status;
    while(1) {
        ptrace(PTRACE_SYSCALL, child, 0, 0);
        waitpid(child, &status, 0);
        if (WIFSTOPPED(status) && WSTOPSIG(status) & 0x80)
            return 0;
        if (WIFEXITED(status))
            return 1;
    }
}

void mapSystemCalls(char* file){
    
    FILE *fptr;
    size_t len = 0;
    int i;
    char *line = NULL;
    char* token, *word;

    fptr = fopen(file, "r");
    if (fptr == NULL) {
        printf("Error in reading file\n");
        exit(1);
    }

    i = 0;
    while (getline(&line, &len, fptr) != -1) {
        word = strdup(line);
        token = strtok(word, " ");
        syscalls[i].name = token;
        token = strtok(NULL, "\n");
        syscalls[i].num = atoi(token);
        i++;
    }

    fclose(fptr);
}

void printMap() {
    
    printf("\n----SYSTEM CALLS MAP----\n\n");
    for(int i = 0; i < SYSCALLS_SIZE; i++){
        printf("%s(%d)\n", syscalls[i].name, syscalls[i].num);
    }
}

void setSyscallNum(struct systemCall *node) {
    
    printf("Input call name = %s\n", node->name);
    for(int i = 0; i < SYSCALLS_SIZE; i++) {
        if (strcmp(node->name, syscalls[i].name) == 0){
            printf("syscall found\n");
            node->num = syscalls[i].num;
            return;
        }
    }
}