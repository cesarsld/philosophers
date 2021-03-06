/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_life.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjaimes <cjaimes@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/20 14:16:00 by cjaimes           #+#    #+#             */
/*   Updated: 2021/02/15 15:45:51 by cjaimes          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

void	unlock_forks(t_philo *philo)
{
	if (philo->hands & 0b10)
		pthread_mutex_unlock(philo->left);
	if (philo->hands & 0b1)
		pthread_mutex_unlock(philo->right);
	philo->hands = 0;
}

void	lock_forks(t_philo *phil)
{
	pthread_mutex_lock(phil->left);
	phil->alerts[e_fork_left] = 1;
	phil->hands |= 0b10;
	check_msgs(phil, elapsed_time(phil->setup->start) / 1000);
	pthread_mutex_lock(phil->right);
	phil->alerts[e_fork_right] = 1;
	phil->hands |= 0b1;
	check_msgs(phil, elapsed_time(phil->setup->start) / 1000);
}

void	eat(t_philo *phil)
{
	pthread_mutex_lock(&phil->eating);
	phil->last_dinner_ts = elapsed_time(phil->setup->start);
	phil->alerts[e_eating] = 1;
	check_msgs(phil, elapsed_time(phil->setup->start) / 1000);
	wait_us(phil->setup->start, phil->last_dinner_ts,
		phil->setup->time_to_eat);
	phil->dinners++;
	pthread_mutex_unlock(&phil->eating);
}

int		check_cycle(t_philo *philo)
{
	if (philo->setup->eat_cycles && philo->dinners >= philo->setup->eat_cycles)
	{
		pthread_mutex_unlock(&(philo->has_eaten_enough_times));
		return (1);
	}
	return (0);
}

void	*handle_philosopher(void *hi)
{
	t_philo *phil;

	phil = hi;
	pthread_create(&(phil->mo), NULL, &monitor_philos, hi);
	while (1 && !phil->setup->can_stop)
	{
		lock_forks(phil);
		eat(phil);
		unlock_forks(phil);
		if (check_cycle(phil))
			return (NULL);
		phil->alerts[e_sleeping] = 1;
		check_msgs(phil, elapsed_time(phil->setup->start) / 1000);
		sleep_us(phil->setup->time_to_sleep);
		phil->alerts[e_thinking] = 1;
		check_msgs(phil, elapsed_time(phil->setup->start) / 1000);
	}
	unlock_forks(phil);
	if (phil->setup->eat_cycles)
		pthread_mutex_unlock(&(phil->has_eaten_enough_times));
	return (NULL);
}
