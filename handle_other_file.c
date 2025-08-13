#include "famine.h"

char *get_extension(char *name_file) {
    char *base = strrchr(name_file, '/');
    if (base == NULL)
        base = name_file;
    else
        base = base + 1;

    int i;
    for (i = strlen(base) - 1; i > 0; i--) {
        if (base[i] == '.')
            return &base[i];
    }
    return NULL;
}

int handle_other_file(char *name_file) {
    int fd = open(name_file, O_WRONLY | O_APPEND);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    char *ext = get_extension(name_file);
    char *payload;

    if (ext != NULL) {
        // printf("extension de %s: %s\n", name_file, ext);
        if (strcmp(ext, ".c") == 0 || strcmp(ext, ".cpp") == 0 || strcmp(ext, ".h") == 0 || strcmp(ext, ".hpp") == 0 || strcmp(ext, ".java") == 0 || strcmp(ext, ".js") == 0)
            payload = "\n// Famine version 0.7 (c)oded jul-0000 by bob\n";
        else if (strcmp(ext, ".py") == 0 || strcmp(ext, ".sh") == 0 || strcmp(ext, ".rb") == 0 || strcmp(ext, ".pl") == 0)
            payload = "\n# Famine version 0.7 (c)oded jul-0000 by bob\n";
        else if (strcmp(ext, ".s") == 0 || strcmp(ext, ".asm") == 0)
            payload = "\n; Famine version 0.7 (c)oded jul-0000 by bob\n";
        else if (strcmp(ext, ".php") == 0)
            payload = "\n// Famine version 0.7 (c)oded jul-0000 by bob\n";
        else
            payload = "\nFamine version 0.7 (c)oded jul-0000 by bob\n";
    }
    else {
        // printf("file %s doesn't have extension\n", name_file);
        payload = "\nFamine version 0.7 (c)oded jul-0000 by bob\n";
    }

    if (write(fd, payload, strlen(payload)) < 0) {
        perror("write");
    }

    close(fd);
    return 0;
}