/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_life.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjaimes <cjaimes@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/20 14:16:00 by cjaimes           #+#    #+#             */
/*   Updated: 2021/02/11 23:10:54 by cjaimes          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

int	unlock_forks(t_philo *philo)
{
	if (philo->hands)
	{
		if (sem_post(philo->setup->forks))
			return (philo->setup->can_stop = 1);
		philo->hands--;
	}
	if (philo->hands)
	{
		if (sem_post(philo->setup->forks))
			return (philo->setup->can_stop = 1);
		philo->hands--;
	}
	return (0);
}

int	lock_forks(t_philo *phil)
{
	if (sem_wait(phil->setup->forks))
		return (phil->setup->can_stop = 1);
	phil->alerts[e_fork_left] = 1;
	phil->hands++;
	check_msgs(phil, elapsed_time(phil->setup->start) / 1000);
	if (sem_wait(phil->setup->forks))
		return (phil->setup->can_stop = 1);
	phil->alerts[e_fork_right] = 1;
	phil->hands++;
	check_msgs(phil, elapsed_time(phil->setup->start) / 1000);
	return (0);
}

void	eat(t_philo *phil)
{
	sem_wait(phil->eating);
	phil->last_dinner_ts = elapsed_time(phil->setup->start);
	phil->alerts[e_eating] = 1;
	check_msgs(phil, elapsed_time(phil->setup->start) / 1000);
	wait_us(phil->setup->start, phil->last_dinner_ts, phil->setup->time_to_eat);
	phil->dinners++;
	sem_post(phil->eating);
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

void	handle_philosopher(void *hi)
{
	t_philo *phil;

	phil = hi;
	if (pthread_create(&(phil->mo), NULL, &monitor_philos, hi))
		exit(1);
	while (1 && !phil->setup->can_stop)
	{
		if (lock_forks(phil))
			exit(1);
		eat(phil);
		if (unlock_forks(phil))
			exit(1);
		if (check_cycle(phil)){
			if (sem_post(phil->has_eaten_enough_times))
				exit(1);
			exit(0);
		}
		phil->alerts[e_sleeping] = 1;
		check_msgs(phil, elapsed_time(phil->setup->start) / 1000);
		sleep_us(phil->setup->time_to_sleep);
		phil->alerts[e_thinking] = 1;
		check_msgs(phil, elapsed_time(phil->setup->start) / 1000);
	}
	pthread_join(phil->mo, NULL);
	if (unlock_forks(phil))
		exit(1);
	if (sem_post(phil->has_eaten_enough_times))
		exit(1);
	if (phil->setup->somebody_died)
		exit(2);
	exit(0);
}