/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjaimes <cjaimes@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/17 11:49:31 by cjaimes           #+#    #+#             */
/*   Updated: 2020/03/20 11:06:05 by cjaimes          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

void	write_msg(int time, int id, const char *action, pthread_mutex_t *writing)
{
	pthread_mutex_lock(writing);
	ft_putnbr(time);
	write(1, "\t", 1);
	ft_putnbr(id);
	write(1, action, ft_strlen(action));
	pthread_mutex_unlock(writing);
}

void	check_msgs(t_philo *phil, int time)
{
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
	uint64_t		time;
	
	philo = phil;
	while (1)
	{
		time = elapsed_time(philo->setup->start);
		if (!philo->is_eating
			&& time - philo->last_dinner_ts > philo->setup->time_to_die)
		{
			set_msg(philo, e_dead, 1);
			write_msg(time / 1000, philo->number, " is dead\n", &philo->setup->writing);
			pthread_mutex_unlock(&(philo->setup->is_dead));
			return (NULL);
		}
		usleep(1000);
	}
	return (NULL);
}
