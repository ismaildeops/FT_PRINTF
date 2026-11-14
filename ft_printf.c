
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

static int	ft_format(va_list *args, char type)
{
	char	c;

	if (type == 'd' || type == 'i')
		return (ft_putnbr(va_arg(*args, int)));
	if (type == 'u')
		return (ft_putnbr_u(va_arg(*args, unsigned int)));
	if (type == 's')
		return (ft_putstr(va_arg(*args, char *)));
	if (type == 'x' || type == 'X')
		return (ft_putnbr_base(va_arg(*args, unsigned int),
				type == 'x' ? "0123456789abcdef" : "0123456789ABCDEF"));
	if (type == 'p')
		return (ft_putptr(va_arg(*args, void *)));
	if (type == 'c')
	{
		c = va_arg(*args, int);
		write(1, &c, 1);
		return (1);
	}
	if (type == '%')
		return (write(1, "%", 1));
	return (-1);
}

static int	ft_handle_format(va_list *args, const char *format, size_t *i)
{
	int	count;

	count = ft_format(args, format[*i + 1]);
	if (count < 0)
	{
		write(1, &format[*i], 1);
		write(1, &format[*i + 1], 1);
		count = 2;
	}
	*i += 2;
	return (count);
}

int	ft_printf(const char *format, ...)
{
	va_list	args;
	size_t	i;
	int		count;

	i = 0;
	count = 0;
	va_start(args, format);
	while (format[i])
	{
		if (format[i] == '%' && format[i + 1])
			count += ft_handle_format(&args, format, &i);
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
