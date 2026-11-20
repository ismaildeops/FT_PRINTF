/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ilaakoub <ilaakoub@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/20 09:38:07 by ilaakoub          #+#    #+#             */
/*   Updated: 2026/11/20 09:38:09 by ilaakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ft_putnbr(int n)
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
