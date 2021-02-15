/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjaimes <cjaimes@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/15 14:18:29 by cjaimes           #+#    #+#             */
/*   Updated: 2021/02/15 14:49:00 by cjaimes          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

void	init_philos(t_philo *philos, t_setup *setup)
{
	int counter;
	int s;

	counter = 0;
	while (counter < setup->philo_num)
	{
		philos[counter] = (t_philo){.number = counter + 1, .th = 0,
		.dinners = 0, .last_dinner_ts = 0, .setup = setup, .is_eating = 0,
		.hands = 0};
		pthread_mutex_init(&(philos[counter].eating), NULL);
		if (setup->eat_cycles)
		{
			pthread_mutex_init(&(philos[counter].has_eaten_enough_times), NULL);
			pthread_mutex_lock(&(philos[counter].has_eaten_enough_times));
		}
		s = 0;
		while (s < 6)
			philos[counter].alerts[s++] = 0;
		philos[counter].left = &(setup->forks[counter]);
		philos[counter].right = philos[counter].number == setup->philo_num ?
		&(setup->forks[0]) : &(setup->forks[counter + 1]);
		counter++;
	}
}
