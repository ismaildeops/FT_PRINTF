NAME = libftprintf.a
src = ft_printf.c ft_putnbr_base.c ft_putptr.c 
obj = $(src:.c=.o)
CC = cc 
CFLAGS = -Wall -Werror -Wextra
AR = ar
ARFLAGS = rcs
RM = rm -f

all: $(NAME)

$(NAME):$(obj)
	$(AR) $(ARFLAGS) $(NAME) $(obj)

%.o:%.c ft_printf.h
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	$(RM) $(obj)

fclean:clean 
	$(RM) $(NAME)

re:fclean all 

.PHONY: all clean fclean re