#ifndef PRINTF
# define PRINTF

int ft_putnbr_base(unsigned long nbr, char *base);
int ft_putptr(void *ptr);
int	ft_printf(const char *format, ...);

#include<unistd.h>
#include<stdarg.h>

#endif