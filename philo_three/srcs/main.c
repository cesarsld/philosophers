/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjaimes <cjaimes@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/13 20:45:09 by cjaimes           #+#    #+#             */
/*   Updated: 2021/02/16 14:15:33 by cjaimes          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

int		launch_philo(t_setup *setup, t_philo *philo, int id)
{
	pid_t pid;

	if ((pid = fork()) == -1)
		return ((setup->can_stop = 1));
	if (pid)
		setup->philo_pid[id] = pid;
	else
		handle_philosopher(philo);
	return (0);
}

int		launch_philos(t_setup *setup, t_philo *philos)
{
	int counter;

	counter = 0;
	while (counter < setup->philo_num)
	{
		if (launch_philo(setup, &(philos[counter]), counter))
			return (1);
		counter++;
		usleep(100);
	}
	return (0);
}

int		wait_all_philo_eat_cycles(t_philo *philos)
{
	int counter;
	int status;
	int exit_status;

	counter = 0;
	while (counter < philos->setup->philo_num)
	{
		waitpid(-1, &status, 0);
		if ((WIFEXITED(status) || WIFSIGNALED(status)))
		{
			exit_status = WEXITSTATUS(status);
			if (exit_status == 2)
			{
				while (counter < philos->setup->philo_num)
					kill(philos->setup->philo_pid[counter++], SIGTERM);
				break ;
			}
			else
				counter++;
		}
		sem_wait(philos[counter].has_eaten_enough_times);
		sem_post(philos[counter].has_eaten_enough_times);
		counter++;
	}
	return (0);
}

void	clean(t_setup *setup, t_philo *philos)
{
	int		counter;
	char	name[50];

	counter = 0;
	while (counter < setup->philo_num)
	{
		sem_close(philos[counter].eating);
		sem_close(philos[counter++].has_eaten_enough_times);
		sem_unlink(make_philo_name(counter, name));
	}
	free(setup->philo_pid);
	free(philos);
	sem_close(setup->forks);
	sem_close(setup->is_dead);
	sem_close(setup->writing);
	sem_unlink("Forks");
	sem_unlink("is_dead");
	sem_unlink("writing");
}

int		main(int ac, char **av)
{
	t_setup		setup;
	t_philo		*philos;

	if (ac < 5)
	{
		printf("usage: ./philo_two  number_of_philosopher time_to_die time");
		printf("_to_eat time_to_sleep [eat_cycles]\n");
		return (1);
	}
	if (init_setup(&setup, ac, av))
		return (0);
	if (!(philos = malloc(sizeof(t_philo) * setup.philo_num)))
		return (0);
	if (init_philos(philos, &setup))
		return (0);
	gettimeofday(&setup.start, NULL);
	if (launch_philos(&setup, philos))
		return (1);
	if (wait_all_philo_eat_cycles(philos))
		return (1);
	clean(&setup, philos);
	printf("Simulation has ended.\n");
	return (0);
}
