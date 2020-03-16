/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjaimes <cjaimes@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/13 20:45:09 by cjaimes           #+#    #+#             */
/*   Updated: 2020/03/16 17:12:58 by cjaimes          ###   ########.fr       */
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

int	elapsed_time(struct timeval start)
{
	struct timeval now;

	gettimeofday(&now, 0);
	return (now.tv_sec - start.tv_sec) * 1000 + (now.tv_usec - start.tv_usec) / 1000;
}

int	lock_forks(pthread_mutex_t *forks, t_philo *phil, int first, int second)
{
	pthread_mutex_lock(&(forks[first]));
	phil->hands |= 0b1;
	printf("%6d %5d has taken a fork\n", elapsed_time(phil->setup->start), phil->number);
	pthread_mutex_lock(&(forks[second]));
	phil->hands |= 0b10;
	printf("%6d %5d has taken second fork\n", elapsed_time(phil->setup->start), phil->number);
	return (SUCCESS);
}

int	unlock_forks(pthread_mutex_t *forks, int first, int second)
{
	pthread_mutex_unlock(&(forks[first]));
	pthread_mutex_unlock(&(forks[second]));
	return (SUCCESS);
}

int	kill_phil()
{
	printf("%6d %5d has died\n", elapsed_time(setup.start), phil->number);
	if (phil->number == phil->setup->philo_num)
		unlock_forks(setup.forks, 0, phil->id);
	else
		unlock_forks(setup.forks, phil->id, phil->id + 1);
	phil->state = e_dead;
	phil->setup->can_stop = 1;
}

void *handle_philosopher(void *hi)
{
	t_philo *phil;
	t_setup	setup;
	struct timeval now;
	int		elapsed;

	phil = hi;
	setup = *(phil->setup);
	while (!phil->setup->can_stop)
	{
		gettimeofday(&now, 0);
		elapsed = elapsed_time(setup.start);
		if (phil->state == e_thinking)
		{
			if (phil->number == phil->setup->philo_num)
			{
				lock_forks(setup.forks, phil, 0, phil->id);
				// pthread_mutex_lock(&(phil->setup->forks[0]));
				// phil->hands |= 0b1;
				// printf("%6d %5d has taken a fork\n", elapsed_time(setup.start), phil->number);
				// pthread_mutex_lock(&(phil->setup->forks[phil->id]));
				// phil->hands |= 0b10;
				// printf("%6d %5d has taken second fork\n", elapsed_time(setup.start), phil->number);
			}
			else
			{
				lock_forks(setup.forks, phil, phil->id, phil->id + 1);
			}
			if (elapsed_time(setup.start) - phil->last_dinner_ts > setup.time_to_die)
			{
				printf("%6d %5d has died\n", elapsed_time(setup.start), phil->number);
				if (phil->number == phil->setup->philo_num)
					unlock_forks(setup.forks, 0, phil->id);
				else
					unlock_forks(setup.forks, phil->id, phil->id + 1);
				phil->state = e_dead;
				phil->setup->can_stop = 1;
				break;
			}
			else
			{
				phil->last_dinner_ts = elapsed_time(setup.start);
				phil->state = e_eating;
			}
		}
		else if (phil->state == e_eating)
		{
			printf("%6d %5d is eating\n", elapsed_time(setup.start), phil->number);
			usleep(setup.time_to_eat * 1000);
			phil->dinners++;
			if (phil->number == phil->setup->philo_num)
			{
				pthread_mutex_unlock(&(phil->setup->forks[0]));
				pthread_mutex_unlock(&(phil->setup->forks[phil->id]));
			}
			else
			{
				pthread_mutex_unlock(&(phil->setup->forks[phil->id]));
				pthread_mutex_unlock(&(phil->setup->forks[phil->id + 1]));
			}
			phil->state = e_sleeping;
		}
		else if (phil->state == e_sleeping)
		{
			printf("%6d %5d is sleeping\n", elapsed_time(setup.start), phil->number);
			usleep(setup.time_to_sleep * 1000);
			phil->state = e_thinking;
			printf("%6d %5d is thinking\n", elapsed_time(setup.start), phil->number);
		}
	}
	return (NULL);
}

int main(int ac, char **av)
{
	t_setup	setup;
	int		counter;
	t_philo	*philos;
	counter = 0;
	setup.can_stop = 0;
	setup.philo_num = ft_atoi(av[1]);
	setup.time_to_die = ft_atoi(av[2]);
	setup.time_to_eat = ft_atoi(av[3]);
	setup.time_to_sleep = ft_atoi(av[4]);
	setup.eat_cycles = ac == 6 ?  ft_atoi(av[5]) : 1;
	setup.philos = malloc(sizeof(pthread_t) * (setup.philo_num));
	philos = malloc(sizeof(t_philo) * setup.philo_num);
	setup.id = malloc(sizeof(int) * (setup.philo_num));
	setup.forks = malloc(sizeof(pthread_mutex_t)* (setup.philo_num));
	while (counter < setup.philo_num)
		pthread_mutex_init(&(setup.forks[counter++]), NULL);
	counter = 0;
	gettimeofday(&setup.start, NULL);
	while (counter < setup.philo_num)
	{
		philos[counter].number = counter + 1;
		philos[counter].id = counter;
		philos[counter].dinners = 0;
		philos[counter].last_dinner_ts = 0;
		philos[counter].hands = 0;
		setup.id[counter] = counter + 1;
		philos[counter].setup = &setup;
		philos[counter].state = e_thinking;
		pthread_create(&setup.philos[counter], NULL, &handle_philosopher, &(philos[counter]));
		counter++;
	}
	counter = 0;
	while (counter < setup.philo_num)
	{
		pthread_join(setup.philos[counter], 0);
		printf("waiting..\n");
		counter++;
	}
}