#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// child read data from parent
// parent send data to child
// a process can be child and parent at meantime

void
sieve(int pleft[2])
{
    close(pleft[1]); // as child, it will not write to its parent
    int prime;
    if (read(pleft[0], &prime, sizeof(prime)) != 0) { // make sure there is a prime sent by parent
        printf("prime %d\n", prime);                  // it will be the last process if read return 0
    }
    else return;
    int pright[2];
    pipe(pright);

    if (fork() == 0) {
        sieve(pright);
    }
    else {
        close(pright[0]); // as parent, it will not read from its child
        int num;
        while (read(pleft[0], &num, sizeof(num)) != 0) {
            if (num % prime != 0) {
                write(pright[1], &num, sizeof(num));
            }
        }
        close(pleft[0]); // as child, it has read all data from its parent
        close(pright[1]); // as parent, it has sent all data to its child
        wait(0);
    }
}

int
main(int argc, char* argv[])
{
    int pright[2];
    pipe(pright);

    if (fork() == 0) {
        sieve(pright);
    }
    else {
        close(pright[0]);
        for (int i = 2; i <= 35; i++) {
            write(pright[1], &i, sizeof(i));
        }
        close(pright[1]);
        wait(0);
    }
    exit(0);
}