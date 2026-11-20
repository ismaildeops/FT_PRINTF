/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ilaakoub <ilaakoub@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/20 08:36:58 by ilaakoub          #+#    #+#             */
/*   Updated: 2026/11/20 08:37:01 by ilaakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PRINTF
# define PRINTF

int ft_putnbr_base(unsigned long nbr, char *base);
int ft_putptr(void *ptr);
int	ft_printf(const char *format, ...);
int	ft_putnbr_u(unsigned int n);
int	ft_putnbr(int n);
int ft_putptr(void *ptr);
int	ft_putstr(char *s);

#include<unistd.h>
#include<stdarg.h>

#endif
