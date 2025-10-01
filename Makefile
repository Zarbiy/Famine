COLOR_RESET = \033[0m
COLOR_RED = \033[91m
COLOR_GREEN = \033[92m
COLOR_YELLOW = \033[93m
COLOR_BOLD = \033[1m

MAKEFLAGS += --no-print-directory

NAME		= famine
SRC			= famine.c handle_32.c handle_64.c utils.c handle_other_file.c conditions.c
OBJ_DIR		= obj
OBJS		= $(OBJ_DIR)/famine.o $(OBJ_DIR)/handle_32.o $(OBJ_DIR)/handle_64.o $(OBJ_DIR)/utils.o $(OBJ_DIR)/handle_other_file.o $(OBJ_DIR)/conditions.o
CC			= cc
CFLAGS		= -g3# -Wall -Wextra -Werror
CHFLAGS		= -I include

RM			= rm -f
DIR_DUP		= mkdir -p $(@D)

NAME_USER = bob
FOLDER = /tmp/test

all: $(NAME)

$(NAME): $(OBJS) 
	@$(CC) $(OBJS) -o $(NAME)
	@printf "$(COLOR_RED)$(COLOR_BOLD)Compilation réussie !$(COLOR_RESET)\n"

$(OBJ_DIR)/%.o: %.c famine.h
	@$(DIR_DUP)
	@$(CC) $(CFLAGS) $(CHFLAGS) -c -o $@ $<

clean:
	@$(RM) $(OBJS)

fclean: clean
	@$(RM) $(NAME) famine.o test.o prog

file_test:
	mkdir -p $(FOLDER)
	printf '#include <stdio.h>\nint main() {\n\tprintf("Bonjour je suis jacob\\n");\n\treturn 0;\n}\n' > $(FOLDER)/prog.c
	cc $(FOLDER)/prog.c -o $(FOLDER)/a1
	cc -m32 $(FOLDER)/prog.c -o $(FOLDER)/a2
	echo "Bonjour je suis Jacob" > $(FOLDER)/test.txt
	mkdir -p $(FOLDER)/bonjour
	echo "Bonjour je suis Jean" > $(FOLDER)/bonjour/test2.txt
	cc $(FOLDER)/prog.c -o $(FOLDER)/bonjour/a3
	cc -m32 $(FOLDER)/prog.c -o $(FOLDER)/bonjour/a4
	cp /usr/bin/ls $(FOLDER)

asm_test:
	nasm -felf64 famine.asm -o famine.o
	gcc -c test.c -o test.o
	gcc test.o famine.o -no-pie -o prog

add_user:
	sudo adduser $(NAME_USER)

delete_user:
	sudo deluser --remove-home $(NAME_USER)

re: fclean all

# sudo nano /etc/systemd/system/mon_service.service
# sudo systemctl enable mon_service.service

# set follow-fork-mode child