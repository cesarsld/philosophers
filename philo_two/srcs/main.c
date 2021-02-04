/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjaimes <cjaimes@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/13 20:45:09 by cjaimes           #+#    #+#             */
/*   Updated: 2021/02/04 13:50:27 by cjaimes          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

char	*make_philo_name(int id, char *dest)
{
	int i;

	i = 11;
	*dest = 0;
	ft_strcpy(dest, "philosopher");
	while (id)
	{
		dest[i++] = (id % 10) + '0';
		id /= 10;
	}
	dest[i] = 0;
	return (dest);
}

int	init_philos(t_philo *philos, t_setup *setup)
{
	int counter;
	int s;
	char name[50];

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
			sem_unlink(make_philo_name(counter, name));
			if ((philos[counter].has_eaten_enough_times =
				sem_open(make_philo_name(counter, name), O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED)
				return (1);
			if (sem_wait(philos[counter].has_eaten_enough_times))
				return (1);
		}
		s = 0;
		while (s < 6)
			philos[counter].alerts[s++] = 0;
		counter++;
	}
	return (0);
}

int		init_setup(t_setup *setup, int ac, char **av)
{
	setup->can_stop = 0;
	setup->somebody_died = 0;
	setup->philo_num = ft_atoi(av[1]);
	setup->time_to_die = ft_atoi(av[2]) * 1000;
	setup->time_to_eat = ft_atoi(av[3]) * 1000;
	setup->time_to_sleep = ft_atoi(av[4]) * 1000;
	setup->eat_cycles = ac == 6 ?  ft_atoi(av[5]) : 0;
	if ((setup->forks = sem_open("Forks", O_CREAT | O_EXCL, 0644, setup->philo_num)) == SEM_FAILED)
		return (1);
	if ((setup->is_dead  = sem_open("is_dead", O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED)
		return (1);
	if ((setup->writing  = sem_open("writing", O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED)
		return (1);
	if (sem_wait(setup->is_dead))
		return (1);
	return (0);
}

int	launch_philos(t_setup setup, t_philo *philos)
{
	int counter;
	//pthread_t th;
	
	counter = 0;
	while (counter < setup.philo_num)
	{
		if (pthread_create(&(philos[counter].th), NULL, &handle_philosopher, &(philos[counter])))
			return (1);
		counter++;
		usleep(20);
	}
	counter = 0;
	while (counter < setup.philo_num)
		pthread_join(philos[counter++].th, NULL);
	return (0);
}

int	wait_all_philo_eat_cycles(t_philo *philos)
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

void	clean(t_setup *setup, t_philo *philos)
{
	int counter;
	char name[50];

	counter = 0;
	while (counter < setup->philo_num)
	{
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
}

int		main(int ac, char **av)
{
	t_setup		setup;
	t_philo		*philos;

	sem_unlink("Forks");
	sem_unlink("is_dead");
	sem_unlink("writing");
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
	if (sem_wait(philos->setup->is_dead))
		return (1);
	if (sem_post(philos->setup->is_dead))
		return (1);
	if (sem_post(philos->setup->writing))
	 	return (1);
	clean(&setup, philos);
	printf("Simulation has ended.\n");
	return (0);
}