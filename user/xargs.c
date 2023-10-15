#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

#define MAXLEN 20

char**
getargs(int argc)
{
    char ch;
    char** argsv = (char**)malloc(sizeof(char*) * MAXARG);
    int j = argc - 1;
    while (read(0, &ch, 1) == 1) {
        char* line = (char*)malloc(MAXLEN);
        int i = 0;
        if (ch != '\n') {
            line[i++] = ch;
        }
        do {
            read(0, &ch, 1);
            if (ch != '\n') {
                line[i++] = ch;
                if (i > MAXLEN - 1) {
                    fprintf(2, "too many args\n");
                    exit(1);
                }
            }
        } while (ch != '\n');
        line[i] = 0;
        argsv[j++] = line;
    }
    argsv[j] = 0;
    return argsv;
}

int
main(int argc, char* argv[])
{
    char** argsv = getargs(argc);
    int pid = fork();
    if (pid < 0) {
        fprintf(2, "fork error\n");
        exit(1);
    }
    if (pid == 0) {
        for (int i = 0; i < argc - 1; i++) {
            argsv[i] = argv[i + 1];
        }
        exec(argsv[0], argsv);
    }
    else {
        wait(0);
        for (int i = argc - 1; argsv[i]; i++) {
            free(argsv[i]);
        }
        free(argsv);
    }
    exit(0);
}