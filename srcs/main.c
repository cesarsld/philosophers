/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjaimes <cjaimes@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/13 20:45:09 by cjaimes           #+#    #+#             */
/*   Updated: 2020/03/20 10:38:32 by cjaimes          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

void	set_msg(t_philo *phil, int msg, int time)
{
	phil->alerts[msg] = time;
}

uint64_t	elapsed_time(struct timeval start)
{
	struct timeval now;

	gettimeofday(&now, 0);
	return (now.tv_sec - start.tv_sec) * 1000000 + (now.tv_usec - start.tv_usec);
}

void	unlock_forks(t_philo *philo)
{
	pthread_mutex_unlock(philo->left);
	pthread_mutex_unlock(philo->right);
}

void	lock_forks(t_philo *phil)
{
	pthread_mutex_lock(phil->left);
	phil->alerts[e_fork_left] = 1;
	pthread_mutex_lock(phil->right);
	phil->alerts[e_fork_right] = 1;
}

void *handle_philosopher(void *hi)
{
	t_philo *phil;
	pthread_t monitor;

	phil = hi;
	pthread_create(&monitor, NULL, &monitor_philos, hi);
	pthread_detach(monitor);
	while (1)
	{
		phil->alerts[e_thinking] = 1;
		lock_forks(phil);
		phil->last_dinner_ts = elapsed_time(phil->setup->start);
		phil->alerts[e_eating] = 1;
		usleep(phil->setup->time_to_eat * 1000);
		phil->dinners++;
		unlock_forks(phil);
		phil->alerts[e_sleeping] = 1;
		usleep(phil->setup->time_to_sleep * 1000);
	}
	return (NULL);
}

void	init_philos(t_philo *philos, t_setup *setup)
{
	int counter;
	int s;

	counter = 0;
	while (counter < setup->philo_num)
	{
		philos[counter].number = counter + 1;
		philos[counter].id = counter;
		philos[counter].dinners = 0;
		philos[counter].last_dinner_ts = 0;
		philos[counter].setup = setup;
		philos[counter].state = e_thinking;
		s = 0;
		while (s < 6)
			philos[counter].alerts[s++] = 0;
		philos[counter].left = &(setup->forks[counter]);
		//if (philos[counter].number == setup->philo_num)
		//	philos[counter].right = &(setup->forks[0]);
		//else
			philos[counter].right = &(setup->forks[counter + 1]);
		counter++;
	}
}

int main(int ac, char **av)
{
	t_setup		setup;
	int			counter;
	t_philo		*philos;
	int			mult;

	counter = 0;
	setup.can_stop = 0;
	setup.philo_num = ft_atoi(av[1]);
	setup.time_to_die = ft_atoi(av[2]);
	setup.time_to_eat = ft_atoi(av[3]);
	setup.time_to_sleep = ft_atoi(av[4]);
	setup.eat_cycles = ac == 6 ?  ft_atoi(av[5]) : 1;
	setup.philos = malloc(sizeof(pthread_t) * (setup.philo_num));
	philos = malloc(sizeof(t_philo) * setup.philo_num);
	//remove one fork since this is testting witht one philosopher
	setup.forks = malloc(sizeof(pthread_mutex_t)* (setup.philo_num + 1));
	pthread_mutex_init(&(setup.is_dead), NULL);
	pthread_mutex_init(&(setup.writing), NULL);
	pthread_mutex_lock(&(setup.is_dead));
	while (counter < setup.philo_num + 1)
		pthread_mutex_init(&(setup.forks[counter++]), NULL);
	init_philos(philos, &setup);
	gettimeofday(&setup.start, NULL);
	mult = setup.philo_num / 2;
	counter = 0;
	while (counter < mult || (counter <= mult && setup.philo_num % 2 == 1))
	{
		pthread_create(&setup.philos[counter * 2], NULL, &handle_philosopher, &(philos[counter * 2]));
		pthread_detach(setup.philos[counter * 2]);
		counter++;
	}
	counter = 0;
	usleep(5);
	while (counter < mult)
	{
		pthread_create(&setup.philos[counter * 2 + 1], NULL, &handle_philosopher, &(philos[counter * 2 + 1]));	
		pthread_detach(setup.philos[counter * 2 + 1]);
		counter++;
	}
	pthread_mutex_lock(&(setup.is_dead));
	pthread_mutex_unlock(&(setup.is_dead));
	return (0);
}