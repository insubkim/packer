
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
CFLAGS	= -Wall -Werror -Wextra -g
# -g -fsanitize=address
RM		= rm -rf
MD		= mkdir -p 

# --------------------------------- RULES ------------------------------------ #

$(OBJS_DIR)%.o : $(SRCS_DIR)%.c $(HDRS_DIR)*.h
	$(MD) $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

all: $(NAME)

$(NAME): | MAKE_MAN

MAKE_MAN: MAKE_LIBS $(HDRS_DIR)*.h $(OBJS)
		touch MAKE_MAN
		rm -f MAKE_BONUS
		
clean:
	make clean -C $(LIBFT_DIR)
	$(RM) $(OBJS_DIR) $(BONUS_OBJS_DIR)
	$(RM) MAKE_MAN MAKE_BONUS MAKE_LIBS

fclean:
	make clean
	make clean -C $(MLX_DIR)
	$(RM) $(LIBS) $(NAME) 

re:
	make fclean
	make all

.PHONY:		all clean fclean re bonus