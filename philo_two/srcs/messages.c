/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   messages.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjaimes <cjaimes@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/12 16:16:24 by cjaimes           #+#    #+#             */
/*   Updated: 2021/02/15 13:13:04 by cjaimes          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

void	msg_think(int time, t_philo *phil)
{
	write_msg(time, phil->number, " is thinking\n", phil->setup->writing);
	phil->alerts[e_thinking] = 0;
}

void	msg_eat(int time, t_philo *phil)
{
	write_msg(time, phil->number, " is eating\n", phil->setup->writing);
	phil->alerts[e_eating] = 0;
}

void	msg_slp(int time, t_philo *phil)
{
	write_msg(time, phil->number, " is sleeping\n", phil->setup->writing);
	phil->alerts[e_sleeping] = 0;
}

void	msg_left_fork(int time, t_philo *phil)
{
	write_msg(time, phil->number, " has taken a fork\n", phil->setup->writing);
	phil->alerts[e_fork_left] = 0;
}

void	msg_right_fork(int time, t_philo *phil)
{
	write_msg(time, phil->number, " has taken a fork\n", phil->setup->writing);
	phil->alerts[e_fork_right] = 0;
}
