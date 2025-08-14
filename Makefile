COLOR_RESET = \033[0m
COLOR_RED = \033[91m
COLOR_GREEN = \033[92m
COLOR_YELLOW = \033[93m
COLOR_BOLD = \033[1m

MAKEFLAGS += --no-print-directory

NAME		= famine
SRC			= famine.c handle_32.c handle_64.c utils.c handle_other_file.c
OBJ_DIR		= obj
OBJS		= $(OBJ_DIR)/famine.o $(OBJ_DIR)/handle_32.o $(OBJ_DIR)/handle_64.o $(OBJ_DIR)/utils.o $(OBJ_DIR)/handle_other_file.o
CC			= cc
CFLAGS		= -g3 #-Wall -Wextra -Werror
CHFLAGS		= -I include

RM			= rm -f
DIR_DUP		= mkdir -p $(@D)

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
	@$(RM) $(NAME)

file_test:
	mkdir -p /tmp/test
	echo "#include <stdio.h>\nint main() {\n\tprintf(\"Bonjour je suis jacob\");\n\treturn 0;\n}" > /tmp/test/prog.c
	cc /tmp/test/prog.c -o /tmp/test/a1
	cc -m32 /tmp/test/prog.c -o /tmp/test/a2
	echo "Bonjour je suis Jacob" > /tmp/test/test.txt
	mkdir -p /tmp/test/bonjour
	echo "Bonjour je suis Jean" > /tmp/test/bonjour/test2.txt
	cc /tmp/test/prog.c -o /tmp/test/bonjour/a3
	cc -m32 /tmp/test/prog.c -o /tmp/test/bonjour/a4

re: fclean all