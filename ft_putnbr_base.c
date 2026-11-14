#include <unistd.h>

int	ft_putnbr_base(unsigned long nbr, char *base)
{
	int	count;

	count = 0;
	if(nbr >= 16)
		count += ft_putnbr_base(nbr / 16, base);
	write(1, &base[nbr % 16], 1);
	return (count + 1);
}
