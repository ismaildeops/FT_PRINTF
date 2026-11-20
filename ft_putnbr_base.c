/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_base.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ilaakoub <ilaakoub@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/20 08:37:35 by ilaakoub          #+#    #+#             */
/*   Updated: 2026/11/20 08:37:38 by ilaakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include<unistd.h>
int ft_putnbr_base(unsigned long nbr, char *base)
{
	int	count;
	count = 0;
	if(nbr >= 16)
		count += ft_putnbr_base(nbr / 16, base);
	int n = nbr % 16;
	write(1,&base[n],1);
	return (count + 1);	
}
