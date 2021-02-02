/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjaimes <cjaimes@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/20 10:37:07 by cjaimes           #+#    #+#             */
/*   Updated: 2021/02/02 19:00:40 by cjaimes          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

int		ft_strlen(const char *str)
{
	size_t			res;

	res = 0;
	while (*str)
	{
		if (!(*(str + 1)))
			return (res + 1);
		if (!(*(str + 2)))
			return (res + 2);
		if (!(*(str + 3)))
			return (res + 3);
		if (!(*(str + 4)))
			return (res + 4);
		if (!(*(str + 5)))
			return (res + 5);
		if (!(*(str + 6)))
			return (res + 6);
		if (!(*(str + 7)))
			return (res + 7);
		res += 8;
		str += 8;
	}
	return (res);
}

void	ft_putnbr(int n)
{
	char	c;
	int		a;

	a = 0;
	if (n == -2147483648)
		a = write(1, "-2147483648", 11);
	else
	{
		if (n < 0)
		{
			a = write(1, "-", 1);
			n = -n;
		}
		if (n / 10 > 0)
			ft_putnbr(n / 10);
		c = n % 10 + '0';
		a = write(1, &c, 1);
	}
}

int	ft_atoi(const char *input)
{
	int res;

	res = 0;
	while (*input)
		res = res * 10  + *input++ - '0';
	return (res);
}