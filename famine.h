#ifndef FAMINE_H
#define FAMINE_H

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <dirent.h>
#include <elf.h>

#define WORD "bob"
#define ACTIVE_SHOW 1
#define START_FOLDER "/home/zarbiy/Documents/test"
#define EXEC_FOLDER "/home/zarbiy/Documents/Famine"

//handle_other_file.c
int handle_other_file(char *name_file);

// handle_32.c
int handle_32(unsigned char *file);

// handle_64.c
int handle_64(unsigned char *file);

// utils.c
void exec_cmd(char *path, int type);

#endif