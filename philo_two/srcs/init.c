/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjaimes <cjaimes@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/15 14:43:20 by cjaimes           #+#    #+#             */
/*   Updated: 2021/02/15 15:54:21 by cjaimes          ###   ########.fr       */
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

char	*make_eating_name(int id, char *dest)
{
	int i;

	i = 6;
	*dest = 0;
	ft_strcpy(dest, "eating");
	while (id)
	{
		dest[i++] = (id % 10) + '0';
		id /= 10;
	}
	dest[i] = 0;
	return (dest);
}

int		setup_philo_sems(t_philo *philo, t_setup *setup, int counter)
{
	char	name[50];

	if ((philo->eating =
		sem_open(make_eating_name(counter, name),
		O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED)
		return (1);
	if (setup->eat_cycles)
	{
		sem_unlink(make_philo_name(counter, name));
		if ((philo->has_eaten_enough_times =
			sem_open(make_philo_name(counter, name),
			O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED)
			return (1);
		if (sem_wait(philo->has_eaten_enough_times))
			return (1);
	}
	return (0);
}

int		init_philos(t_philo *philos, t_setup *setup)
{
	int		counter;
	int		s;
	char	name[50];

	counter = 0;
	while (counter < setup->philo_num)
	{
		philos[counter] = (t_philo){.number = counter + 1, .dinners = 0,
		.last_dinner_ts = 0, .setup = setup, .is_eating = 0, .hands = 0};
		sem_unlink(make_eating_name(counter, name));
		if (setup_philo_sems(&(philos[counter]), setup, counter))
			return (1);
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
	if ((setup->philo_num = ft_atoi(av[1])) <= 1)
	{
		printf("You must use at least 2 philosophers\n");
		return (1);
	}
	setup->philo_num = ft_atoi(av[1]);
	setup->time_to_die = ft_atoi(av[2]) * 1000;
	setup->time_to_eat = ft_atoi(av[3]) * 1000;
	setup->time_to_sleep = ft_atoi(av[4]) * 1000;
	setup->eat_cycles = ac == 6 ? ft_atoi(av[5]) : 0;
	if ((setup->forks = sem_open("Forks",
		O_CREAT | O_EXCL, 0644, setup->philo_num)) == SEM_FAILED)
		return (1);
	if ((setup->is_dead = sem_open("is_dead",
		O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED)
		return (1);
	if ((setup->writing = sem_open("writing",
		O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED)
		return (1);
	if (sem_wait(setup->is_dead))
		return (1);
	return (0);
}
