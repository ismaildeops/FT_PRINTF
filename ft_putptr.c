#include "ft_printf.h"

int	ft_putptr(void *ptr)
{
	unsigned long	address;
	int				count;

	if (ptr == NULL)
		return (write(1, "(nil)", 5));
	address = (unsigned long)ptr;
	write(1, "0x", 2);
	count = ft_putnbr_base(address, "0123456789abcdef");
	return (count + 2);
}
