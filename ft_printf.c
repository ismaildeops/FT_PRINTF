/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ilaakoub <ilaakoub@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/20 08:37:14 by ilaakoub          #+#    #+#             */
/*   Updated: 2026/11/20 08:37:16 by ilaakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static int	handle_format_nbr(char format, va_list *args)
{
	int	count;

	count = 0;
	if (format == 'd' || format == 'i')
		count += ft_putnbr(va_arg(*args, int));
	else if (format == 'u')
		count += ft_putnbr_u(va_arg(*args, unsigned int));
	else if (format == 'x')
		count += ft_putnbr_base(va_arg(*args, unsigned int),
				"0123456789abcdef");
	else if (format == 'X')
		count += ft_putnbr_base(va_arg(*args, unsigned int),
				"0123456789ABCDEF");
	else if (format == 'p')
		count += ft_putptr(va_arg(*args, void *));
	return (count);
}

static int	handle_format_other(char format, va_list *args)
{
	int		count;
	char	c;

	count = 0;
	if (format == 's')
		count += ft_putstr(va_arg(*args, char *));
	else if (format == 'c')
	{
		c = va_arg(*args, int);
		write(1, &c, 1);
		count++;
	}
	else if (format == '%')
	{
		write(1, "%", 1);
		count++;
	}
	return (count);
}

int	handle_format(char format, va_list *args)
{
	int	count;

	count = 0;
	if (format == 'd' || format == 'i' || format == 'u'
		|| format == 'x' || format == 'X' || format == 'p')
		count += handle_format_nbr(format, args);
	else
		count += handle_format_other(format, args);
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
		{
			count += handle_format(format[i + 1], &args);
			i = i + 2;
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
