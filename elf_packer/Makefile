
# ---------------------------------- NAME ------------------------------------ #

NAME = woody_woodpacker

# ------------------------------- DIRECTORIES -------------------------------- #

HDRS_DIR	= headers/
SRCS_DIR	= sources/
OBJS_DIR	= objects/

# ---------------------------------- FILES ----------------------------------- #

INCLUDE = -I $(HDRS_DIR) 
SRCS = $(wildcard $(SRCS_DIR)*.c)
OBJS += $(addprefix $(OBJS_DIR), $(notdir $(SRCS:.c=.o)))

# -------------------------------- COMPILATE --------------------------------- #

CC		= cc
CFLAGS	= -g
# CFLAGS	= -Wall -Werror -Wextra -g
# -g -fsanitize=address
RM		= rm -rf
MD		= mkdir -p 

# --------------------------------- RULES ------------------------------------ #

$(OBJS_DIR)%.o : $(SRCS_DIR)%.c $(HDRS_DIR)*.h
	$(MD) $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

all: $(NAME)

$(NAME): | .MAKE_MAN

.MAKE_MAN: $(HDRS_DIR)*.h $(OBJS)
		touch .MAKE_MAN
		rm -f .MAKE_BONUS
		$(CC) $(CFLAGS) $(OBJS) $(CFLAGS) -o $(NAME)
		
clean:
	$(RM) $(OBJS_DIR) $(BONUS_OBJS_DIR)
	$(RM) .MAKE_MAN .MAKE_BONUS .MAKE_LIBS

fclean: clean
	$(RM) $(LIBS) $(NAME) 

re: fclean
	make all

.PHONY:		all clean fclean re bonus