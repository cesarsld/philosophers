/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjaimes <cjaimes@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/20 10:37:07 by cjaimes           #+#    #+#             */
/*   Updated: 2021/02/15 13:51:41 by cjaimes          ###   ########.fr       */
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

int		ft_atoi(const char *input)
{
	int res;

	res = 0;
	while (*input)
		res = res * 10 + *input++ - '0';
	return (res);
}
