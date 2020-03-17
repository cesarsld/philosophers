/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjaimes <cjaimes@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/17 11:49:31 by cjaimes           #+#    #+#             */
/*   Updated: 2020/03/17 17:43:33 by cjaimes          ###   ########.fr       */
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

void	write_msg(int time, int id, const char *action)
{
	//pad_number(time, 999999999);
	ft_putnbr(time);
	write(1, " ", 1);
	//pad_number(id, 999);
	ft_putnbr(id);
	write(1, action, ft_strlen(action));
}

void	check_msgs(t_philo *phil)
{
	if (phil->alerts[e_thinking] >= 0)
	{
		write_msg(phil->alerts[e_thinking], phil->number, " is thinking\n");
		phil->alerts[e_thinking] = -1;
	}
	if (phil->alerts[e_fork_left] >= 0)
	{
		write_msg(phil->alerts[e_fork_left], phil->number, " has taken a fork\n");
		phil->alerts[e_fork_left] = -1;
	}
	if (phil->alerts[e_fork_right] >= 0)
	{
		write_msg(phil->alerts[e_fork_right], phil->number, " has taken a fork\n");
		phil->alerts[e_fork_right] = -1;
	}
	if (phil->alerts[e_eating] >= 0)
	{
		write_msg(phil->alerts[e_eating], phil->number, " is eating\n");
		phil->alerts[e_eating] = -1;
	}
	if (phil->alerts[e_sleeping] >= 0)
	{
		write_msg(phil->alerts[e_sleeping], phil->number, " is sleeping\n");
		phil->alerts[e_sleeping] = -1;
	}
	if (phil->alerts[e_dead] >= 0)
	{
		write_msg(phil->alerts[e_dead], phil->number, " is dead\n");
		phil->alerts[e_dead] = -1;
		phil->setup->can_stop = 1;
	}

}

// int	check_dead(t_philo *phil, t_setup *setup)
// {
// 	if (setup->can_stop)
// 		return (FAILURE);
// 	if (elapsed_time(setup->start) - phil->last_dinner_ts > setup->time_to_die)
// 	{
// 		set_msg(phil, e_dead, elapsed_time(phil->setup->start));
// 		unlock_forks(phil);
// 		phil->state = e_dead;
// 		return (FAILURE);
// 	}
// 	return (SUCCESS);
// }

void	*monitor_philos(void *phil)
{
	int		counter;
	t_philo *philos;
	
	philos = phil;
	while (!philos->setup->can_stop)
	{
		counter = 0;
		while (counter < philos->setup->philo_num)
		{
			if (elapsed_time(philos->setup->start) - philos[counter].last_dinner_ts
				> philos->setup->time_to_die)
			{
				set_msg(&(philos[counter]), e_dead, elapsed_time(philos[counter].setup->start));
				unlock_forks(&(philos[counter]));
				write_msg(philos[counter].alerts[e_dead], philos[counter].number, " is dead\n");
				philos->setup->can_stop = 1;
				return (NULL);
			}
			check_msgs(&(philos[counter]));
			counter++;
		}
		usleep(1000);
	}
	return (NULL);
}