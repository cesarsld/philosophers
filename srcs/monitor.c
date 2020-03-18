/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjaimes <cjaimes@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/17 11:49:31 by cjaimes           #+#    #+#             */
/*   Updated: 2020/03/18 11:59:02 by cjaimes          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

int		ft_strlen(const char *str)
{
	size_t			res;

	res = 0;
	while (*str)
	{
		if (!(*(str + 1)))
			return (res + 1);
		if (!(*(str + 2)))
			return (res + 2);
		if (!(*(str + 3)))
			return (res + 3);
		if (!(*(str + 4)))
			return (res + 4);
		if (!(*(str + 5)))
			return (res + 5);
		if (!(*(str + 6)))
			return (res + 6);
		if (!(*(str + 7)))
			return (res + 7);
		res += 8;
		str += 8;
	}
	return (res);
}

void	pad_number(int num, int check)
{
	if (num < check)
	{
		write(1, " ", 1);
		return (pad_number(num, check / 10));
	}
}

void	write_msg(int time, int id, const char *action, pthread_mutex_t *writing)
{
	pthread_mutex_lock(writing);
	pad_number(time, 9999999);
	ft_putnbr(time);
	write(1, " ", 1);
	pad_number(id, 999);
	ft_putnbr(id);
	write(1, action, ft_strlen(action));
	pthread_mutex_unlock(writing);
}

void	check_msgs(t_philo *phil, int time)
{
	if (phil->alerts[e_thinking])
	{
		write_msg(time, phil->number, " is thinking\n", &phil->setup->writing);
		phil->alerts[e_thinking] = 0;
	}
	if (phil->alerts[e_fork_left])
	{
		write_msg(time, phil->number, " has taken a fork\n", &phil->setup->writing);
		phil->alerts[e_fork_left] = 0;
	}
	if (phil->alerts[e_fork_right])
	{
		write_msg(time, phil->number, " has taken a fork\n", &phil->setup->writing);
		phil->alerts[e_fork_right] = 0;
	}
	if (phil->alerts[e_eating])
	{
		write_msg(time, phil->number, " is eating\n", &phil->setup->writing);
		phil->alerts[e_eating] = 0;
	}
	if (phil->alerts[e_sleeping])
	{
		write_msg(time, phil->number, " is sleeping\n", &phil->setup->writing);
		phil->alerts[e_sleeping] = 0;
	}
	if (phil->alerts[e_dead])
	{
		write_msg(time, phil->number, " is dead\n", &phil->setup->writing);
		phil->alerts[e_dead] = 0;
		phil->setup->can_stop = 1;
	}

}

void	*monitor_philos(void *phil)
{
	t_philo *philos;
	int		time;
	
	philos = phil;
	while (1)
	{
		time = elapsed_time(philos->setup->start);
		if (time - philos->last_dinner_ts
			> philos->setup->time_to_die)
		{
			set_msg(philos, e_dead, 1);
			write_msg(time, philos->number, " is dead\n", &philos->setup->writing);
			pthread_mutex_unlock(&(philos->setup->is_dead));
			return (NULL);
		}
		check_msgs(philos, time);
		usleep(500);
	}
	return (NULL);
}

// void	*monitor_philos(void *phil)
// {
// 	int		counter;
// 	t_philo *philos;
	
// 	philos = phil;
// 	while (1)
// 	{
// 		counter = 0;
// 		while (counter < philos->setup->philo_num)
// 		{
// 			if (elapsed_time(philos->setup->start) - philos[counter].last_dinner_ts
// 				> philos->setup->time_to_die)
// 			{
// 				set_msg(&(philos[counter]), e_dead, elapsed_time(philos[counter].setup->start));
// 				unlock_forks(&(philos[counter]));
// 				write_msg(philos[counter].alerts[e_dead], philos[counter].number, " is dead\n");
// 				//philos->setup->can_stop = 1;
// 				pthread_mutex_unlock(&(philos->setup->is_dead));
// 				return (NULL);
// 			}
// 			check_msgs(&(philos[counter]));
// 			counter++;
// 		}
// 		usleep(1000);
// 	}
// 	return (NULL);
// }