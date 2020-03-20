/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjaimes <cjaimes@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/13 20:46:47 by cjaimes           #+#    #+#             */
/*   Updated: 2020/03/20 10:56:32 by cjaimes          ###   ########.fr       */
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
	e_fork_left,
	e_fork_right,
	e_eating,
	e_sleeping,
	e_thinking,
	e_dead,
}					t_philo_states;

typedef struct		s_setup
{
	pthread_t		*philos;
	pthread_mutex_t	*forks;
	pthread_mutex_t	is_dead;
	pthread_mutex_t	writing;
	struct timeval	start;
	int				philo_num;
	uint64_t				time_to_die;
	uint64_t				time_to_eat;
	uint64_t				time_to_sleep;
	int				eat_cycles;
	int				can_stop;
}					t_setup;

typedef struct		s_msg
{
	int				time;
	t_philo_states	type;
}					t_msg;

/*
** alerts order: 0 fork left, 1 fork right
** 2 eating, 3 sleeping, 4 thinking, 5 ded
*/
typedef struct		s_philosophers
{
	int				number;
	int				id;
	pthread_mutex_t	*left;
	pthread_mutex_t	*right;
	int				alerts[6];
	t_philo_states	state;
	t_setup			*setup;
	uint64_t		last_dinner_ts;
	int				dinners;
	int				is_eating;
}					t_philo;


void	ft_putnbr(int n);
int	ft_atoi(const char *input);
int		ft_strlen(const char *str);

void	check_msgs(t_philo *phil, int time);

void	*monitor_philos(void *stp);
uint64_t	elapsed_time(struct timeval start);
void	set_msg(t_philo *phil, int msg, int time);
void	unlock_forks(t_philo *philo);

#endif
