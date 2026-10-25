NAME = libftprintf.a
SRCFILES =  ft_printf.c
OFILES = $(SRCFILES:.c=.o)
CFLAGS = -Wall -Wextra -Werror 

all: $(NAME)

$(NAME):$(OFILES)
	ar rcs $(NAME) $(OFILES)

%.o:%.c
	cc -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OFILES)

fclean:clean	
	rm -f $(NAME)

re: fclean all

.PHONY: clean fclean all re