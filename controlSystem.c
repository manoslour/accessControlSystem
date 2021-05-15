#include "controlSystem.h"

int main(int argc, char *argv[]){

    FILE *fptr;
    int i, size;
    size_t len = 0;
    char *line = NULL;
    char* sequence[25];
    char* linesArray[25];

    fptr = fopen(argv[1], "r");
    if (fptr == NULL) {
        printf("Error in reading file\n");
        exit(1);
    }

    i = 0;
    while (getline(&line, &len, fptr) != -1) {
        line[strcspn(line, "\n")] = 0; //Remove trailing newline character 
        linesArray[i] = strdup(line);
        i++;
    }

    size = i-1;
    struct systemCall sysCalls[size];

    i = 0;
    char* token = strtok(linesArray[0], " ");
    while (token != NULL) {
        sequence[i] = token;
        printf("sequence[%d] = %s\n", i, sequence[i]);
        token = strtok(NULL, " ");
        i++;
    }

    for(i = 0; i < size; i++){
        token = strtok(linesArray[i+1], " ");
        sysCalls[i].name = strdup(token);
        token = strtok(NULL, " ");
        sysCalls[i].limit = strdup(token); 
        printf("Syscalls[%d]: name = %s | limit = %s\n", i, sysCalls[i].name, sysCalls[i].limit);        
    }

    free(line);
    fclose(fptr);
    
    return 0;
}