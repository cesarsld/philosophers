/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjaimes <cjaimes@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/17 11:49:31 by cjaimes           #+#    #+#             */
/*   Updated: 2021/02/02 19:06:22 by cjaimes          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

void	set_msg(t_philo *phil, int msg)
{
	phil->alerts[msg] = 1;
}

void	write_msg(int time, int id, const char *action, pthread_mutex_t *writing)
{
	pthread_mutex_lock(writing);
	printf("%d\t%d%s", time, id, action);
	pthread_mutex_unlock(writing);
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
	{
		write_msg(time, phil->number, " is thinking\n", &phil->setup->writing);
		phil->alerts[e_thinking] = 0;
	}
	if (phil->alerts[e_fork_left])
	{
		write_msg(time, phil->number, " has taken a fork\n", &phil->setup->writing);
		phil->alerts[e_fork_left] = 0;
	}
	if (phil->alerts[e_fork_right])
	{
		write_msg(time, phil->number, " has taken a fork\n", &phil->setup->writing);
		phil->alerts[e_fork_right] = 0;
	}
	if (phil->alerts[e_eating])
	{
		write_msg(time, phil->number, " is eating\n", &phil->setup->writing);
		phil->alerts[e_eating] = 0;
	}
	if (phil->alerts[e_sleeping])
	{
		write_msg(time, phil->number, " is sleeping\n", &phil->setup->writing);
		phil->alerts[e_sleeping] = 0;
	}
	if (phil->alerts[e_dead])
	{
		write_msg(time, phil->number, " is dead\n", &phil->setup->writing);
		phil->alerts[e_dead] = 0;
		phil->setup->can_stop = 1;
	}

}

void	*monitor_philos(void *phil)
{
	t_philo *philo;
	u_int64_t		time;
	
	philo = phil;
	while (1 && !philo->setup->can_stop)
	{
		time = elapsed_time(philo->setup->start);
		if (time - philo->last_dinner_ts > philo->setup->time_to_die
			&& !philo->setup->can_stop)
		{
			philo->setup->can_stop = 1;
			philo->setup->somebody_died = 1;
			set_msg(philo, e_dead);
			pthread_mutex_lock(&philo->setup->writing);
			write_msg_unsafe(time / 1000, philo->number, " is dead\n");
			pthread_mutex_unlock(&philo->setup->writing);
			pthread_mutex_unlock(&(philo->setup->is_dead));
			return (NULL);
		}
		usleep(1000);
	}
	return (NULL);
}
