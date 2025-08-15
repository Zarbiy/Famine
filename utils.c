#include "famine.h"

void exec_cmd(char *path, int type) {
    char cmd[1024] = "\0";
    char *str;
    if (type == 0) {
        str = "strings";
        snprintf(cmd, sizeof(cmd), "%s %s | grep %s", str, path, WORD);
        write(1, cmd, strlen(cmd));
        write(1, "\n", 1);
        system(cmd);
        system(path);
    }
    else if (type == 1) {
        str = "cat";
        snprintf(cmd, sizeof(cmd), "%s %s | grep %s", str, path, WORD);
        write(1, cmd, strlen(cmd));
        write(1, "\n", 1);
        system(cmd);
        snprintf(cmd, sizeof(cmd), "%s %s", str, path);
        // system(cmd);
    }
    else
        return ;
    write(1, "\n----------\n", 12);
}
