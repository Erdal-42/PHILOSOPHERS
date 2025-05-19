NAME    = philo
CC      = gcc
CFLAGS  = -Werror -Wall -Wextra -pthread

# Default build mode: none
MODE    ?= none

# Source and object folders
SRC_PATH = sources/
OBJ_PATH = objects/

# Source files
SRC     =   main.c \
            parsing.c \
            time.c \
            exit.c \
            init.c \
            philosopher.c \
            output.c \
            monitor.c

# Include and object handling
SRCS    = $(addprefix $(SRC_PATH), $(SRC))
OBJ     = $(SRC:.c=.o)
OBJS    = $(addprefix $(OBJ_PATH), $(OBJ))
INC     = -I ./includes/

# Handle build modes
ifeq ($(MODE), pretty)
    CFLAGS += -D
endif

ifeq ($(MODE), debug)
    CFLAGS += -g -fsanitize=thread
endif

# Build rules
all: $(NAME)

$(OBJ_PATH)%.o: $(SRC_PATH)%.c
	@mkdir -p $(OBJ_PATH)
	$(CC) $(CFLAGS) -c $< -o $@ $(INC)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

clean:
	rm -rf $(OBJ_PATH)

fclean: clean
	rm -f $(NAME)

re: fclean all

# Debug build and run
debug: 
	$(MAKE) MODE=debug

debug_run: 
	$(MAKE) fclean
	$(MAKE) MODE=debug
	./$(NAME) 3 200 100 100

# Valgrind race detector via Helgrind
helgrind: 
	$(MAKE) fclean
	$(MAKE)
	valgrind --tool=helgrind ./$(NAME) 3 200 100 100

.PHONY: all clean fclean re debug debug_run helgrind
