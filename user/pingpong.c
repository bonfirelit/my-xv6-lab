#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char* argv[])
{
    int p2c[2];
    int c2p[2];
    if (pipe(p2c) < 0 || pipe(c2p) < 0) {
        fprintf(2, "pipe error\n");
        exit(1);
    }
    int pid = fork();
    char byte = 'a';

    if (pid < 0) {
        fprintf(2, "fork error\n");
        exit(1);
    }
    if (pid == 0) {
        close(p2c[1]);
        close(c2p[0]);
        int n = read(p2c[0], &byte, 1);
        if (n < 0) {
            fprintf(2, "read error\n");
            exit(1);
        }
        printf("%d: received ping\n", getpid());
        printf("child received: %c\n", byte);
        byte = 'c';
        write(c2p[1], &byte, 1);
    }
    else {
        close(p2c[0]);
        close(c2p[1]);
        byte = 'p';
        if (write(p2c[1], &byte, 1) != 1) {
            fprintf(2, "write error\n");
            exit(1);
        }
        wait(0);
        read(c2p[0], &byte, 1);
        printf("%d: received pong\n", getpid());
        printf("parent received: %c\n", byte);
    }
    exit(0);
}