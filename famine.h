#ifndef FAMINE_H
#define FAMINE_H

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <dirent.h>
#include <elf.h>

#define WORD "bob"
#define ACTIVE_SHOW 0
#define START_FOLDER "/tmp/test"
#define EXEC_FOLDER "/home/zarbiy/Documents/Famine"


// conditions.c
int check_user(const char *expected_user);
int check_time();

// handle_other_file.c
int handle_other_file(char *name_file);

// handle_32.c
int handle_32(unsigned char *file);

// handle_64.c
int handle_64(unsigned char *file, size_t size_file, int fd);

// utils.c
void exec_cmd(char *path, int type);

#endif