/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjaimes <cjaimes@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/13 20:45:09 by cjaimes           #+#    #+#             */
/*   Updated: 2021/02/16 13:50:28 by cjaimes          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

void	init_setup(t_setup *setup, int ac, char **av)
{
	int err;

	err = 0;
	setup->can_stop = 0;
	setup->somebody_died = 0;
	if (ac <= 5)
	{
		printf("usage: ./philo_one  number_of_philosopher time_to_die time");
		printf("_to_eat time_to_sleep [eat_cycles]\n");
		exit(1);
	}
	if ((setup->philo_num = ft_atoi(av[1])) <= 1)
	{
		printf("You must use at least 2 philosophers\n");
		exit(1);
	}
	setup->time_to_die = ft_atoi(av[2]) * 1000;
	setup->time_to_eat = ft_atoi(av[3]) * 1000;
	setup->time_to_sleep = ft_atoi(av[4]) * 1000;
	setup->eat_cycles = ac == 6 ? ft_atoi(av[5]) : 0;
	pthread_mutex_init(&(setup->is_dead), NULL);
	pthread_mutex_init(&(setup->writing), NULL);
	pthread_mutex_lock(&(setup->is_dead));
}

int		launch_philos(t_setup setup, t_philo *philos)
{
	int counter;
	int mult;

	mult = setup.philo_num / 2;
	counter = 0;
	while (counter < mult || (counter <= mult && setup.philo_num % 2 == 1))
	{
		if (pthread_create(&(philos[counter * 2].th), NULL,
		&handle_philosopher, &(philos[counter * 2])))
			return (1);
		counter++;
	}
	counter = 0;
	usleep(50);
	while (counter < mult)
	{
		if (pthread_create(&(philos[counter * 2 + 1].th), NULL,
		&handle_philosopher, &(philos[counter * 2 + 1])))
			return (1);
		counter++;
	}
	counter = 0;
	while (counter < setup.philo_num)
		pthread_join(philos[counter++].th, NULL);
	return (0);
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
		counter = write(1, "Everyone has eaten enough times.\n", 33);
	philos->setup->can_stop = 1;
	pthread_mutex_unlock(&(philos->setup->is_dead));
}

void	clean(t_setup *setup, t_philo *philos)
{
	int counter;

	counter = 0;
	while (counter < setup->philo_num)
	{
		pthread_join(philos[counter].mo, NULL);
		pthread_mutex_destroy(&(setup->forks[counter]));
		pthread_mutex_destroy(&(philos[counter].eating));
		pthread_mutex_destroy(&(philos[counter++].has_eaten_enough_times));
	}
	pthread_mutex_destroy(&(setup->is_dead));
	pthread_mutex_unlock(&(philos->setup->writing));
	pthread_mutex_destroy(&(setup->writing));
	free(philos);
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
	setup.forks = malloc(sizeof(pthread_mutex_t) * (setup.philo_num));
	while (counter < setup.philo_num)
		pthread_mutex_init(&(setup.forks[counter++]), NULL);
	init_philos(philos, &setup);
	gettimeofday(&setup.start, NULL);
	if (launch_philos(setup, philos))
		return (1);
	if (setup.eat_cycles)
		wait_all_philo_eat_cycles(philos);
	pthread_mutex_lock(&(setup.is_dead));
	pthread_mutex_unlock(&(setup.is_dead));
	clean(&setup, philos);
	ac = write(1, "Simulation has ended.\n", 22);
	return (0);
}
