#include <usys.h>
#include <uio.h>
#include <string.h>

#define LEN_CMD  100
#define MAX_ARGC 20


int getcmd(char *cmd){
    int len;
    printf("$ ");
    if ((len = gets(cmd)) < 0) return -1;
    return len;
}

int parse(char *argv[], int *argc, char *cmd){
    char *pcmd;

    pcmd = cmd;
    while (*pcmd != '\0'){
        if (*pcmd == ' ' || *pcmd == '\t'){
            pcmd++;
            continue; 
        }
        if (*argc >= MAX_ARGC) return -1;
        argv[(*argc)++] = pcmd++;

        while (*pcmd != '\t' && *pcmd != ' ' && *pcmd != '\0') pcmd++;
    }
    argv[*argc] = 0;

    pcmd = cmd;
    while (*pcmd != '\0'){
        if (*pcmd == ' ' || *pcmd == '\t'){
            *pcmd = '\0';
        }
        pcmd++;
    }
    return 1;
}

void welcome(){
    struct stat st;
    int fd;
    char logo[150] = "";

    if ((fd = _open("/logo.txt", O_RONLY)) < 0){
        puts("Fail to open /logo.txt\n");
        goto bad;
    }
    if (_fstat(fd, &st) < 0){
        puts("Fail to getstat of /logo.txt\n");
    }
    if (_read(fd, logo, st.size) < 0){
        puts("Fail to read /logo.txt\n");
        goto bad;
    }
    puts(logo);

bad:
    _close(fd);
}

int main(int _argc, char **_argv){
    char cmd[LEN_CMD];
    int argc, pid;
    char *argv[MAX_ARGC];
    int len;

    welcome();
    for (;;){
        argc = 0;
        memset(cmd, 0, sizeof(cmd));
        memset(argv, 0, sizeof(argv));

        len = getcmd(cmd);
        if (len < 0) break;
        if (len > LEN_CMD) {
            printf("\ncommand to long\n");
            continue;
        }

        if (parse(argv, &argc, cmd) < 0){
            printf("parse fail, too many arguments\n");
        };

        if (argc == 0) continue;

        /* build-in command cd */
        if (strcmp(argv[0], "cd") == 0){
            if (argc < 2){
                printf("cd: missing directory.\n");
            } else {
                if (_chdir(argv[1]) < 0){
                    printf("cd: can not change directroy to %s\n", argv[1]);
                }
            }
        } else {
            pid = _fork();
            if (pid == 0){
                if (_exec(argv[0], argv) < 0){
                    printf("argv: can not exec %s\n", argv[0]);
                    _exit();
                }
            } else if (pid > 0){
                if (_wait() < 0){
                    printf("sh: wait %d return -1\n", pid);
                    goto bad;
                }
            } else {
                printf("sh: fork fail");
                continue;
            }
        }
    }

bad:
    _exit();
}