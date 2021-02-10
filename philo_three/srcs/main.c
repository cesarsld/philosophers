/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjaimes <cjaimes@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/13 20:45:09 by cjaimes           #+#    #+#             */
/*   Updated: 2021/02/10 15:56:43 by cjaimes          ###   ########.fr       */
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
	if (!(setup->philo_pid = malloc(sizeof(int) * setup->philo_num)))
		return (1);
	sem_unlink("Forks");
	sem_unlink("is_dead");
	sem_unlink("writing");
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


int launch_philo(t_setup *setup, t_philo *philo, int id)
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

int	launch_philos(t_setup *setup, t_philo *philos)
{
	int counter;
	
	counter = 0;
	while (counter < setup->philo_num)
	{
		if (launch_philo(setup, &(philos[counter]), counter))
			return (1);
		counter++;
		usleep(10);
	}
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
	if (sem_post(philos->setup->writing))
		return (1);
	if (sem_post(philos->setup->is_dead))
		return (1);
	return (0);
}

int	wait_all_philo_eat_cycles_2(t_philo *philos)
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
				{
					kill(philos->setup->philo_pid[counter++], SIGTERM);
				}
				break;
			}
			else
				counter++;
		}
		if (sem_wait(philos[counter].has_eaten_enough_times))
			return (1);
		if (sem_post(philos[counter].has_eaten_enough_times))
			return (1);
		counter++;
	}
	// if (sem_post(philos->setup->writing))
	// 	return (1);
	// if (sem_wait(philos->setup->writing))
	// 	return (1);
	// if (sem_post(philos->setup->writing))
	// 	return (1);
	// if (sem_post(philos->setup->is_dead))
	// 	return (1);
	return (0);
}

void	clean(t_setup *setup, t_philo *philos)
{
	int counter;
	char name[50];
	// int status;

	counter = 0;
	while (counter < setup->philo_num)
	{
		//pthread_join(philos[counter].mo, NULL);
		//waitpid((pid_t)setup->philo_pid[counter], &status, 0);
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

	if (init_setup(&setup, ac, av))
		return (0);
	if (!(philos = malloc(sizeof(t_philo) * setup.philo_num)))
		return (0);
	if (init_philos(philos, &setup))
		return (0);
	gettimeofday(&setup.start, NULL);
	if (launch_philos(&setup, philos))
		return (1);
	//if (setup.eat_cycles)
		if (wait_all_philo_eat_cycles_2(philos))
			return (1);

	// if (sem_wait(philos->setup->is_dead))
	// 	return (1);
	// if (sem_post(philos->setup->is_dead))
	// 	return (1);
	clean(&setup, philos);
	printf("Simulation has ended.\n");
	return (0);
}