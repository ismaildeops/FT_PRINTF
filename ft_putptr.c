/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putptr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ilaakoub <ilaakoub@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/20 08:37:51 by ilaakoub          #+#    #+#             */
/*   Updated: 2026/11/20 08:37:52 by ilaakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int ft_putptr(void *ptr)
{
	if (ptr == NULL)
	{
    	write(1, "(nil)", 5);
    	return (5);
	}
	unsigned long address;
	address = (unsigned long)ptr;
	write(1, "0x", 2);
	int count = ft_putnbr_base(address , "0123456789abcdef");
	return (count + 2);
}
