/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjaimes <cjaimes@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/17 11:49:31 by cjaimes           #+#    #+#             */
/*   Updated: 2021/02/17 13:23:19 by cjaimes          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

void	set_msg(t_philo *phil, int msg)
{
	phil->alerts[msg] = 1;
}

void	write_msg(int time, int id, const char *action, sem_t *writing)
{
	sem_wait(writing);
	printf("%d\t%d%s", time, id, action);
	sem_post(writing);
}

void	write_msg_unsafe(int time, int id, const char *action)
{
	printf("%d\t%d%s", time, id, action);
}

void	check_msgs(t_philo *phil, int time)
{
	if (phil->setup->can_stop)
		return ;
	if (phil->alerts[e_thinking])
		msg_think(time, phil);
	if (phil->alerts[e_fork_left])
		msg_left_fork(time, phil);
	if (phil->alerts[e_fork_right])
		msg_right_fork(time, phil);
	if (phil->alerts[e_eating])
		msg_eat(time, phil);
	if (phil->alerts[e_sleeping])
		msg_slp(time, phil);
	if (phil->alerts[e_dead])
	{
		write_msg(time, phil->number, " died\n", phil->setup->writing);
		phil->alerts[e_dead] = 0;
		phil->setup->can_stop = 1;
	}
}

void	*monitor_philos(void *phil)
{
	t_philo		*philo;
	u_int64_t	time;

	philo = phil;
	while (!philo->setup->can_stop)
	{
		sem_wait(philo->eating);
		time = elapsed_time(philo->setup->start);
		if ((time - philo->last_dinner_ts > philo->setup->time_to_die
			&& !philo->setup->can_stop) && (!philo->setup->eat_cycles ||
			philo->dinners < philo->setup->eat_cycles))
		{
			philo->setup->can_stop = 1;
			philo->setup->somebody_died = 1;
			sem_wait(philo->setup->writing);
			write_msg_unsafe(time / 1000, philo->number, " died\n");
			sem_post(philo->setup->writing);
			sem_post(philo->setup->is_dead);
			sem_post(philo->eating);
			return (NULL);
		}
		sem_post(philo->eating);
		usleep(1000);
	}
	return (NULL);
}
