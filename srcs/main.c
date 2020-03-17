/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjaimes <cjaimes@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/13 20:45:09 by cjaimes           #+#    #+#             */
/*   Updated: 2020/03/17 17:45:25 by cjaimes          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

void	ft_putnbr(int n)
{
	char c;

	if (n == -2147483648)
		write(1, "-2147483648", 11);
	else
	{
		if (n < 0)
		{
			write(1, "-", 1);
			n = -n;
		}
		if (n / 10 > 0)
			ft_putnbr(n / 10);
		c = n % 10 + '0';
		write(1, &c, 1);
	}
}

int	ft_atoi(const char *input)
{
	int res;

	res = 0;
	while (*input)
		res = res * 10  + *input++ - '0';
	return (res);
}

void	set_msg(t_philo *phil, int msg, int time)
{
	phil->alerts[msg] = time;
}

int	elapsed_time(struct timeval start)
{
	struct timeval now;

	gettimeofday(&now, 0);
	return (now.tv_sec - start.tv_sec) * 1000 + (now.tv_usec - start.tv_usec) / 1000;
}

int	unlock_forks(t_philo *philo)
{
	if (philo->hands & 0b1)
		pthread_mutex_unlock(philo->left);
	if (philo->hands & 0b10)
		pthread_mutex_unlock(philo->right);
	philo->hands = 0;
	return (SUCCESS);
}

int	check_dead(t_philo *phil, t_setup *setup)
{
	if (setup->can_stop)
		return (FAILURE);
	if (elapsed_time(setup->start) - phil->last_dinner_ts > setup->time_to_die)
	{
		set_msg(phil, e_dead, elapsed_time(phil->setup->start));
		unlock_forks(phil);
		phil->state = e_dead;
		return (FAILURE);
	}
	return (SUCCESS);
}

int	lock_forks(t_philo *phil)
{
	pthread_mutex_lock(phil->left);
	phil->hands |= 0b1;
	set_msg(phil, e_fork_left, elapsed_time(phil->setup->start));
	pthread_mutex_lock(phil->right);
	phil->hands |= 0b10;
	set_msg(phil, e_fork_right, elapsed_time(phil->setup->start));
	return (SUCCESS);
}

void *handle_philosopher(void *hi)
{
	t_philo *phil;
	t_setup	setup;

	phil = hi;
	setup = *(phil->setup);
	while (!phil->setup->can_stop)
	{
		if (phil->state == e_thinking)
		{
			if (lock_forks(phil) == FAILURE)
				break ;
			phil->last_dinner_ts = elapsed_time(setup.start);
			phil->state = e_eating;
		}
		else if (phil->state == e_eating)
		{
			set_msg(phil, e_eating, /*elapsed_time(phil->setup->start)*/phil->last_dinner_ts );
			usleep(setup.time_to_eat * 1000);
			phil->dinners++;
			unlock_forks(phil);
			phil->state = e_sleeping;
		}
		else if (phil->state == e_sleeping)
		{
			set_msg(phil, e_sleeping, elapsed_time(phil->setup->start));
			usleep(setup.time_to_sleep * 1000);
			phil->state = e_thinking;
			set_msg(phil, e_thinking, elapsed_time(phil->setup->start));
		}
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
		philos[counter].hands = 0;
		philos[counter].setup = setup;
		philos[counter].state = e_thinking;
		s = 0;
		while (s < 6)
			philos[counter].alerts[s++] = -1;
		philos[counter].left = &(setup->forks[counter]);
		if (philos[counter].number == setup->philo_num)
			philos[counter].right = &(setup->forks[0]);
		else
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
	pthread_t	monitor;

	counter = 0;
	setup.can_stop = 0;
	setup.philo_num = ft_atoi(av[1]);
	setup.time_to_die = ft_atoi(av[2]);
	setup.time_to_eat = ft_atoi(av[3]);
	setup.time_to_sleep = ft_atoi(av[4]);
	setup.eat_cycles = ac == 6 ?  ft_atoi(av[5]) : 1;
	setup.philos = malloc(sizeof(pthread_t) * (setup.philo_num));
	philos = malloc(sizeof(t_philo) * setup.philo_num);
	setup.forks = malloc(sizeof(pthread_mutex_t)* (setup.philo_num));
	while (counter < setup.philo_num)
		pthread_mutex_init(&(setup.forks[counter++]), NULL);
	init_philos(philos, &setup);
	gettimeofday(&setup.start, NULL);
	pthread_create(&monitor, NULL, &monitor_philos, philos);
	mult = setup.philo_num / 2;
	//write(1, "c", 1);
	counter = 0;
	while (counter < mult || (counter <= mult && setup.philo_num % 2 == 1))
	{
		//write(1, "a", 1);
		pthread_create(&setup.philos[counter * 2], NULL, &handle_philosopher, &(philos[counter * 2]));
		counter++;
		usleep(20);
	}
	counter = 0;
	while (counter < mult)
	{
		pthread_create(&setup.philos[counter * 2 + 1], NULL, &handle_philosopher, &(philos[counter * 2 + 1]));	
		counter++;
		usleep(20);
	}
	counter = 0;
	while (counter < setup.philo_num && !setup.can_stop)
	{
		pthread_join(setup.philos[counter], 0);
		counter++;
	}
}