/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjaimes <cjaimes@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/13 20:46:47 by cjaimes           #+#    #+#             */
/*   Updated: 2020/03/16 17:06:13 by cjaimes          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILOSOPHERS_H
# define PHILOSOPHERS_H
#include <pthread.h>
# include <stdio.h>
# include <unistd.h>
# include <stdlib.h>
# include <sys/time.h>
# define SUCCESS 0
# define FAILURE 1

typedef enum		e_philo_states
{
	e_eating,
	e_sleeping,
	e_thinking,
	e_dead
}					t_philo_states;

typedef struct		s_setup
{
	pthread_t		*philos;
	pthread_mutex_t	*forks;
	struct timeval	start;
	int				*id;
	int				philo_num;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				eat_cycles;
	int				can_stop;
}					t_setup;

typedef struct		s_philosophers
{
	int				number;
	int				id;
	t_philo_states	state;
	t_setup			*setup;
	uint8_t			hands : 2;
	int				last_dinner_ts;
	int				dinners;
}					t_philo;

#endif
