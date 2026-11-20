/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putstr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ilaakoub <ilaakoub@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/20 09:38:19 by ilaakoub          #+#    #+#             */
/*   Updated: 2026/11/20 09:38:21 by ilaakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ft_putstr(char *s)
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
