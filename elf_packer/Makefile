
# ---------------------------------- NAME ------------------------------------ #

NAME = woody_woodpacker
DUMMY = woody

# ------------------------------- DIRECTORIES -------------------------------- #

HDRS_DIR	= headers/
SRCS_DIR	= sources/
OBJS_DIR	= objects/

# ---------------------------------- FILES ----------------------------------- #

INCLUDE = -I $(HDRS_DIR) 
SRCS_C = $(wildcard $(SRCS_DIR)*.c)
SRCS_S = $(wildcard $(SRCS_DIR)*.s)
OBJS += $(addprefix $(OBJS_DIR), $(notdir $(SRCS_C:.c=.o)))
STUB = $(OBJS_DIR)stub.bin

# -------------------------------- COMPILATE --------------------------------- #

CC		= gcc
CFLAGS	= -g
DEBUG   = 1
# CFLAGS	= -Wall -Werror -Wextra -g
# -g -fsanitize=address
RM		= rm -rf
MD		= mkdir -p 

# --------------------------------- RULES ------------------------------------ #

$(OBJS_DIR)%.o : $(SRCS_DIR)%.c $(HDRS_DIR)*.h
	$(MD) $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@ -DDEBUG=$(DEBUG)

$(OBJS_DIR)%.o : $(SRCS_DIR)%.c $(HDRS_DIR)*.h
	$(MD) $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@ -DDEBUG=$(DEBUG)


all: $(NAME)

$(NAME): | .MAKE_MAN

$(STUB):$(SRCS_S)
	$(MD) $(dir $@)
	nasm -f bin $(SRCS_DIR)stub.s -o $(OBJS_DIR)stub.bin
	cd $(OBJS_DIR) && xxd -i stub.bin > ../$(HDRS_DIR)stub.h

.MAKE_MAN: $(STUB) $(HDRS_DIR)*.h $(OBJS)
	touch .MAKE_MAN
	rm -f .MAKE_BONUS
	$(CC) $(CFLAGS) $(INCLUDE) $(OBJS) $(CFLAGS) -o $(NAME)

clean:
	$(RM) $(OBJS_DIR) $(BONUS_OBJS_DIR)
	$(RM) .MAKE_MAN .MAKE_BONUS .MAKE_LIBS 

fclean: clean
	$(RM) $(LIBS) $(NAME) $(DUMMY)

re: fclean
	make all

.PHONY:		all clean fclean re bonus