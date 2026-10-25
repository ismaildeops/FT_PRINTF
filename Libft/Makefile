NAME = libft.a
SRCF = ft_putnbr_fd.c ft_strlen.c ft_putstr_fd.c
OFIL = $(SRCF:.c=.o)

all: $(NAME)

$(NAME):$(OFIL)
	ar rcs $(NAME) $(OFIL)

%.o:%.c
	cc -c $< -o $@
clean:
	rm -f $(OFIL)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re