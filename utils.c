#include "famine.h"

void print_tab(char **tab) {
    printf("\nAll file modifie:\n");
    int i = 0;
    while (tab[i]) {
        printf("%s\n", tab[i]);
        i++;
    }
    return ;
}

void free_tab(char **tab, int index) {
    for (int i = 0; i < index; i++)
        free(tab[i]);
    free(tab);
}

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
