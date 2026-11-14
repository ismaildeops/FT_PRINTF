
#include "ft_printf.h"

static int	ft_putstr(char *s)
{
	size_t	s_len;
	size_t	i;

	s_len = 0;
	i = 0;
	if (!s)
		s = "(null)";
	while (s[s_len])
		s_len++;
	while (s[i])
	{
		write(1, &s[i], 1);
		i++;
	}
	return (s_len);
}

static int	ft_putnbr(int n)
{
	long	m;
	char	c;
	int		count;

	m = n;
	count = 0;
	if (m < 0)
	{
		write(1, "-", 1);
		count++;
		m = -m;
	}
	if (m >= 10)
		count += ft_putnbr(m / 10);
	c = m % 10 + '0';
	write(1, &c, 1);
	return (count + 1);
}

static int	ft_putnbr_u(unsigned int n)
{
	char	c;
	int		count;

	count = 0;
	if (n >= 10)
		count += ft_putnbr_u(n / 10);
	c = n % 10 + '0';
	write(1, &c, 1);
	return (count + 1);
}

int	ft_printf(const char *format, ...)
{
	va_list	args;
	size_t	i;
	int		count;
	char	c;

	i = 0;
	count = 0;
	va_start(args, format);
	while (format[i])
	{
		if (format[i] == '%' && format[i + 1])
		{
			if (format[i + 1] == 'd' || format[i + 1] == 'i')
				count += ft_putnbr(va_arg(args, int));
			else if (format[i + 1] == 'u')
				count += ft_putnbr_u(va_arg(args, unsigned int));
			else if (format[i + 1] == 's')
				count += ft_putstr(va_arg(args, char *));
			else if (format[i + 1] == 'x')
				count += ft_putnbr_base(va_arg(args, unsigned int), "0123456789abcdef");
			else if (format[i + 1] == 'X')
				count += ft_putnbr_base(va_arg(args, unsigned int), "0123456789ABCDEF");
			else if (format[i + 1] == 'p' && format[i + 1])
				count += ft_putptr(va_arg(args, void *));
			else if (format[i + 1] == 'c')
			{
				c = va_arg(args, int);
				write(1, &c, 1);
				count++;
			}
			else if (format[i + 1] == '%')
			{
				write(1, "%", 1);
				count++;
			}
			else
			{
				write(1, &format[i], 1);
				write(1, &format[i + 1], 1);
				count += 2;
			}
			i += 2;
		}
		else
		{
			write(1, &format[i], 1);
			count++;
			i++;
		}
	}
	va_end(args);
	return (count);
}
