/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjaimes <cjaimes@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/13 20:45:09 by cjaimes           #+#    #+#             */
/*   Updated: 2020/03/20 19:41:22 by cjaimes          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

void	init_philos(t_philo *philos, t_setup *setup)
{
	int counter;
	int s;

	counter = 0;
	while (counter < setup->philo_num)
	{
		philos[counter].number = counter + 1;
		philos[counter].dinners = 0;
		philos[counter].last_dinner_ts = 0;
		philos[counter].setup = setup;
		philos[counter].is_eating = 0;
		philos[counter].hands = 0;
		if (setup->eat_cycles)
		{
			pthread_mutex_init(&(philos[counter].has_eaten_enough_times), NULL);
			pthread_mutex_lock(&(philos[counter].has_eaten_enough_times));
		}
		s = 0;
		while (s < 6)
			philos[counter].alerts[s++] = 0;
		philos[counter].left = &(setup->forks[counter]);
		if (philos[counter].number == setup->philo_num)
			philos[counter].right = &(setup->forks[0]);
		else
			philos[counter].right = &(setup->forks[counter + 1]);
		counter++;
	}
}

void	init_setup(t_setup *setup, int ac, char **av)
{
	setup->can_stop = 0;
	setup->somebody_died = 0;
	setup->philo_num = ft_atoi(av[1]);
	setup->time_to_die = ft_atoi(av[2]) * 1000;
	setup->time_to_eat = ft_atoi(av[3]) * 1000;
	setup->time_to_sleep = ft_atoi(av[4]) * 1000;
	setup->eat_cycles = ac == 6 ?  ft_atoi(av[5]) : 0;
	pthread_mutex_init(&(setup->is_dead), NULL);
	pthread_mutex_init(&(setup->writing), NULL);
	pthread_mutex_lock(&(setup->is_dead));
}

void	launch_philos(t_setup setup, t_philo *philos)
{
	int counter;
	int mult;
	pthread_t th;
	
	mult = setup.philo_num / 2;
	counter = 0;
	while (counter < mult || (counter <= mult && setup.philo_num % 2 == 1))
	{
		pthread_create(&th, NULL, &handle_philosopher, &(philos[counter * 2]));
		pthread_detach(th);
		counter++;
	}
	counter = 0;
	usleep(5);
	while (counter < mult)
	{
		pthread_create(&th, NULL, &handle_philosopher, &(philos[counter * 2 + 1]));	
		pthread_detach(th);
		counter++;
	}
}

void	wait_all_philo_eat_cycles(t_philo *philos)
{
	int counter;

	counter = 0;
	while (counter < philos->setup->philo_num)
	{
		pthread_mutex_lock(&(philos[counter].has_eaten_enough_times));
		pthread_mutex_unlock(&(philos[counter].has_eaten_enough_times));
		counter++;
	}
	pthread_mutex_unlock(&philos->setup->writing);
	pthread_mutex_lock(&(philos->setup->writing));
	if (!philos->setup->somebody_died)
		write(1, "Everyone has eaten enough times.\n", 33);
	pthread_mutex_unlock(&(philos->setup->writing));
	pthread_mutex_unlock(&(philos->setup->is_dead));
}

void	clean(t_setup *setup, t_philo *philos)
{
	int counter;

	counter = 0;
	free(philos);
	while (counter < setup->philo_num)
		pthread_mutex_destroy(&(setup->forks[counter++]));
	free(setup->forks);
}

int		main(int ac, char **av)
{
	t_setup		setup;
	int			counter;
	t_philo		*philos;

	counter = 0;
	init_setup(&setup, ac, av);
	philos = malloc(sizeof(t_philo) * setup.philo_num);
	setup.forks = malloc(sizeof(pthread_mutex_t)* (setup.philo_num));
	while (counter < setup.philo_num)
		pthread_mutex_init(&(setup.forks[counter++]), NULL);
	init_philos(philos, &setup);
	gettimeofday(&setup.start, NULL);
	launch_philos(setup, philos);
	if (setup.eat_cycles)
		wait_all_philo_eat_cycles(philos);
	pthread_mutex_lock(&(setup.is_dead));
	pthread_mutex_unlock(&(setup.is_dead));
	clean(&setup, philos);
	write(1, "Simulation has ended.\n", 22);
	return (0);
}