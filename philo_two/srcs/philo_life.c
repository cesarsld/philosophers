/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_life.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjaimes <cjaimes@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/20 14:16:00 by cjaimes           #+#    #+#             */
/*   Updated: 2021/02/17 14:40:06 by cjaimes          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

int		unlock_forks(t_philo *philo)
{
	if (sem_post(philo->setup->forks))
		return (philo->setup->can_stop = 1);
	if (sem_post(philo->setup->forks))
		return (philo->setup->can_stop = 1);
	return (0);
}

int		lock_forks(t_philo *phil)
{
	if (sem_wait(phil->setup->forks))
		return (phil->setup->can_stop = 1);
	phil->alerts[e_fork_left] = 1;
	check_msgs(phil, elapsed_time(phil->setup->start) / 1000);
	if (sem_wait(phil->setup->forks))
		return (phil->setup->can_stop = 1);
	phil->alerts[e_fork_right] = 1;
	check_msgs(phil, elapsed_time(phil->setup->start) / 1000);
	return (0);
}

void	eat(t_philo *phil)
{
	lock_forks(phil);
	sem_wait(phil->eating);
	phil->last_dinner_ts = elapsed_time(phil->setup->start);
	phil->alerts[e_eating] = 1;
	check_msgs(phil, elapsed_time(phil->setup->start) / 1000);
	wait_us(phil->setup->start, phil->last_dinner_ts, phil->setup->time_to_eat);
	phil->dinners++;
	sem_post(phil->eating);
	unlock_forks(phil);
}

int		check_cycle(t_philo *philo)
{
	if (philo->setup->eat_cycles && philo->dinners >= philo->setup->eat_cycles)
	{
		if (sem_post(philo->has_eaten_enough_times))
			return (philo->setup->can_stop = 1);
		return (1);
	}
	return (0);
}

void	*handle_philosopher(void *hi)
{
	t_philo *phil;

	phil = hi;
	if (pthread_create(&(phil->mo), NULL, &monitor_philos, hi))
		return ((void *)1);
	while (1 && !phil->setup->can_stop)
	{
		eat(phil);
		if (check_cycle(phil))
			return (NULL);
		phil->alerts[e_sleeping] = 1;
		check_msgs(phil, elapsed_time(phil->setup->start) / 1000);
		sleep_us(phil->setup->time_to_sleep);
		phil->alerts[e_thinking] = 1;
		check_msgs(phil, elapsed_time(phil->setup->start) / 1000);
	}
	if (unlock_forks(phil))
		return ((void *)1);
	if (phil->has_eaten_enough_times && sem_post(phil->has_eaten_enough_times))
		return ((void *)1);
	return (NULL);
}
