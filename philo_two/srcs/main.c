/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjaimes <cjaimes@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/13 20:45:09 by cjaimes           #+#    #+#             */
/*   Updated: 2021/02/16 14:15:15 by cjaimes          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

int		launch_philos(t_setup setup, t_philo *philos)
{
	int counter;

	counter = 0;
	while (counter < setup.philo_num)
	{
		if (pthread_create(&(philos[counter].th), NULL,
			&handle_philosopher, &(philos[counter])))
			return (1);
		counter++;
		usleep(20);
	}
	counter = 0;
	while (counter < setup.philo_num)
		pthread_join(philos[counter++].th, NULL);
	return (0);
}

int		wait_all_philo_eat_cycles(t_philo *philos)
{
	int counter;

	counter = 0;
	while (counter < philos->setup->philo_num)
	{
		if (sem_wait(philos[counter].has_eaten_enough_times))
			return (1);
		if (sem_post(philos[counter].has_eaten_enough_times))
			return (1);
		counter++;
	}
	if (sem_post(philos->setup->writing))
		return (1);
	if (sem_wait(philos->setup->writing))
		return (1);
	if (!philos->setup->somebody_died)
		printf("Everyone has eaten enough times.\n");
	philos->setup->can_stop = 1;
	if (sem_post(philos->setup->is_dead))
		return (1);
	return (0);
}

int		clean(t_setup *setup, t_philo *philos)
{
	int		counter;
	char	name[50];

	if (sem_wait(philos->setup->is_dead))
		return (1);
	if (sem_post(philos->setup->is_dead))
		return (1);
	if (sem_post(philos->setup->writing))
		return (1);
	counter = 0;
	while (counter < setup->philo_num)
	{
		sem_close(philos[counter].eating);
		pthread_join(philos[counter].mo, NULL);
		sem_unlink(make_philo_name(counter, name));
		sem_close(philos[counter++].has_eaten_enough_times);
	}
	free(philos);
	sem_unlink("Forks");
	sem_unlink("is_dead");
	sem_unlink("writing");
	sem_close(setup->forks);
	sem_close(setup->is_dead);
	sem_close(setup->writing);
	return (0);
}

int		main(int ac, char **av)
{
	t_setup		setup;
	t_philo		*philos;

	sem_unlink("Forks");
	sem_unlink("is_dead");
	sem_unlink("writing");
	if (ac < 5)
		return (print_error_usage());
	if (init_setup(&setup, ac, av))
		return (0);
	if (!(philos = malloc(sizeof(t_philo) * setup.philo_num)))
		return (0);
	if (init_philos(philos, &setup))
		return (0);
	gettimeofday(&setup.start, NULL);
	if (launch_philos(setup, philos))
		return (1);
	if (setup.eat_cycles)
		if (wait_all_philo_eat_cycles(philos))
			return (1);
	if (clean(&setup, philos))
		return (1);
	printf("Simulation has ended.\n");
	return (0);
}
