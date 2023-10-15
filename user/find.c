#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void
find(char* path, char* file)
{
    int fd;
    char buf[512], *p;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }
    if (fstat(fd, &st) < 0) {
        fprintf(2, "ls: cannot stat %s\n", path);
        close(fd);
        return;
    }
    if (read(fd, &de, sizeof(de)) != sizeof(de)) {
        exit(1);
    }

    switch (st.type) {
    case T_FILE:
        if (strcmp(de.name, file) == 0) {
            printf("%s\n", path);
        }
        break;
    case T_DIR:
        if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
            printf("find: path too long\n");
            break;
        }
        strcpy(buf, path);
        p = buf+strlen(buf);
        *p++ = '/';
        while(read(fd, &de, sizeof(de)) == sizeof(de)){
            if(de.inum == 0 || !strcmp(de.name, ".") || !strcmp(de.name, ".."))
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if(stat(buf, &st) < 0){
                printf("find: cannot stat %s\n", buf);
                continue;
            }
            if (st.type == T_FILE && strcmp(de.name, file) == 0) {
                printf("%s\n", buf);
            }
            else if (st.type == T_DIR) {
                find(buf, file);
            }
        }
        break;
    }
}

int
main(int argc, char* argv[])
{
    if (argc < 3) {
        fprintf(2, "usage: find path file\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}