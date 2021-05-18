#include "controlSystem.h"

int main(int argc, char *argv[]){

    FILE *fptr;
    size_t len = 0;
    pid_t child_pid;
    char *line = NULL;
    struct systemCall *sequence[25];
    char *linesArray[25];
    int i, size, status, syscall, retval;

    if (argc < 2) {
        printf("Too few arguments\n");
        exit(1);
    }

    /*
    char *arguments[argc+1];
    memcpy(arguments, argv, argc * sizeof(char));
    arguments[argc] = NULL;
    */

    mapSystemCalls(argv[2]);

    // Read instructions file
    fptr = fopen(argv[1], "r");
    if (fptr == NULL) {
        printf("Error in reading file\n");
        exit(1);
    }

    // Store file's info
    i = 0;
    while (getline(&line, &len, fptr) != -1) {
        line[strcspn(line, "\n")] = 0; //Remove trailing newline character 
        linesArray[i] = strdup(line);
        i++;
    }
    
    size = i-1;
    i = 0;
    struct systemCall *sysCalls[size];

    // Store sequence from file's first line
    char* token = strtok(linesArray[0], " ");
    while (token != NULL) {
        sequence[i] = malloc(sizeof(struct systemCall));
        sequence[i]->name = token;
        setSyscallNum(sequence[i]);
        printf("sequence[%d].num = %d\n", i, sequence[i]->num);
        token = strtok(NULL, " ");
        i++;
    }

    // Store syscalls and their allowed call times
    for (i = 0; i < size; i++) {
        token = strtok(linesArray[i+1], " ");
        sysCalls[i] = malloc(sizeof(struct systemCall));
        sysCalls[i]->name = strdup(token);
        sysCalls[i]->count = 0;
        token = strtok(NULL, " ");
        sysCalls[i]->limit = atoi(strdup(token));
        setSyscallNum(sysCalls[i]);
        printf("Syscalls[%d]: name = %s | limit = %d | num = %d | count = %d\n", i, sysCalls[i]->name, sysCalls[i]->limit, sysCalls[i]->num, sysCalls[i]->count);        
    }

    child_pid = fork();
    char* arg = strdup(argv[3]);
    char *const cmd[] = {arg, NULL};
    struct user_regs_struct regs;

    switch (child_pid) {
        case 0:
            ptrace(PTRACE_TRACEME);
            kill(getpid(), SIGSTOP);
            execvp(argv[3], cmd);
            printf("This wont be printed if execvp is succesful\n");
            break;
        case -1:
            perror("Fork failed\n");
            exit(EXIT_FAILURE);
            break;
        default:
            waitpid(child_pid, &status, 0);
            ptrace(PTRACE_SETOPTIONS, child_pid, 0, PTRACE_O_TRACESYSGOOD);
            while(1) {
                if(wait_for_syscall(child_pid) != 0)
                    break;
                ptrace(PTRACE_GETREGS, child_pid, 0, &regs);
                syscall = regs.orig_eax;
                printf("syscall(%d)\n", syscall);
                //increase call num called
                for(i = 0; i < size; i++){
                    if(syscall == sysCalls[i]->num){
                        printf("Found syscall: %s | %d\n", sysCalls[i]->name, sysCalls[i]->num);
                        sysCalls[i]->count += 1;
                        printf("count = %d\n", sysCalls[i]->count);
                        if(sysCalls[i]->count > sysCalls[i]->limit){
                            printf("Allowed limit of consecutive system calls exceeded\n");
                            sysCalls[i]->count = 0; //Reset count
                            sleep(2);
                            printf("Resumimg..\n");
                        }
                        break;
                    }
                }
                if(wait_for_syscall(child_pid) != 0)
                    break;
            }
            break;
    }    

    //printMap();

    free(line);
    fclose(fptr);
    
    return 0;
}