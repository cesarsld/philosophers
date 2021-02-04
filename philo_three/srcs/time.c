/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjaimes <cjaimes@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/20 12:36:49 by cjaimes           #+#    #+#             */
/*   Updated: 2021/02/04 14:39:24 by cjaimes          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

uint64_t	elapsed_time(struct timeval start)
{
	struct timeval now;

	gettimeofday(&now, 0);
	return (now.tv_sec - start.tv_sec) * 1000000 + (now.tv_usec - start.tv_usec);
}

void		wait_us(struct timeval t, u_int64_t start, u_int64_t dur)
{
	u_int64_t target;

	target = start + dur;
	while (elapsed_time(t) < target)
		usleep(1000);
}

void		sleep_us(u_int64_t dur)
{
	struct timeval now;
	gettimeofday(&now, 0);
	while (elapsed_time(now) < dur)
		usleep(1000);
}
