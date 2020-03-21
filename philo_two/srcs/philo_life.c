/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_life.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjaimes <cjaimes@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/20 14:16:00 by cjaimes           #+#    #+#             */
/*   Updated: 2020/03/21 22:17:04 by cjaimes          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

void	unlock_forks(t_philo *philo)
{
	if (philo->hands)
	{
		sem_post(philo->setup->forks);
		philo->hands--;
	}
	if (philo->hands)
	{
		sem_post(philo->setup->forks);
		philo->hands--;
	}
}

void	lock_forks(t_philo *phil)
{
	if (sem_wait(phil->setup->forks))
	{
		return ;
	}
	phil->alerts[e_fork_left] = 1;
	phil->hands++;
	check_msgs(phil, elapsed_time(phil->setup->start) / 1000);
	sem_wait(phil->setup->forks);
	phil->alerts[e_fork_right] = 1;
	phil->hands++;
	check_msgs(phil, elapsed_time(phil->setup->start) / 1000);
}

void	eat(t_philo *phil)
{
	phil->is_eating = 1;
	phil->last_dinner_ts = elapsed_time(phil->setup->start);
	phil->alerts[e_eating] = 1;
	check_msgs(phil, elapsed_time(phil->setup->start) / 1000);
	wait_us(phil->setup->start, phil->last_dinner_ts, phil->setup->time_to_eat);
	phil->dinners++;
	phil->is_eating = 0;
}

int		check_cycle(t_philo *philo)
{
	if (philo->setup->eat_cycles && philo->dinners >= philo->setup->eat_cycles)
	{
		if (sem_post(philo->has_eaten_enough_times))
			return(1);
		return (1);
	}
	return (0);
}

void	*handle_philosopher(void *hi)
{
	t_philo *phil;
	pthread_t monitor;

	phil = hi;
	pthread_create(&monitor, NULL, &monitor_philos, hi);
	pthread_detach(monitor);
	while (1 && !phil->setup->can_stop)
	{
		phil->alerts[e_thinking] = 1;
		check_msgs(phil, elapsed_time(phil->setup->start) / 1000);
		lock_forks(phil);
		eat(phil);
		unlock_forks(phil);
		if (check_cycle(phil))
			return (NULL);
		phil->alerts[e_sleeping] = 1;
		check_msgs(phil, elapsed_time(phil->setup->start) / 1000);
		sleep_us(phil->setup->time_to_sleep);
	}
	unlock_forks(phil);
	sem_post(phil->has_eaten_enough_times);
	return (NULL);
}