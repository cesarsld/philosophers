/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjaimes <cjaimes@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/13 20:46:47 by cjaimes           #+#    #+#             */
/*   Updated: 2021/02/15 13:00:49 by cjaimes          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILOSOPHERS_H
# define PHILOSOPHERS_H
#include <pthread.h>
# include <stdio.h>
# include <unistd.h>
# include <stdlib.h>
# include <sys/time.h>
# include <sys/wait.h>
# include <stdbool.h>
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
	pthread_mutex_t	*forks;
	pthread_mutex_t	is_dead;
	pthread_mutex_t	writing;
	struct timeval	start;
	int				philo_num;
	u_int64_t		time_to_die;
	u_int64_t		time_to_eat;
	u_int64_t		time_to_sleep;
	int				eat_cycles;
	int				somebody_died;
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
	pthread_t		th;
	pthread_t		mo;
	pthread_mutex_t	eating;
	pthread_mutex_t	*left;
	pthread_mutex_t	*right;
	pthread_mutex_t has_eaten_enough_times;
	int				alerts[6];
	u_int8_t		hands : 2;
	t_setup			*setup;
	u_int64_t		last_dinner_ts;
	int				dinners;
	int				is_eating;
}					t_philo;


void				ft_putnbr(int n);
int					ft_atoi(const char *input);
int					ft_strlen(const char *str);

void				check_msgs(t_philo *phil, int time);
void				set_msg(t_philo *phil, int msg);
void				*monitor_philos(void *stp);
void				unlock_forks(t_philo *philo);

void				*handle_philosopher(void *hi);

void 				wait_us(struct timeval t, u_int64_t start, u_int64_t dur);
void 				sleep_us(u_int64_t dur);
u_int64_t			elapsed_time(struct timeval start);

void				msg_think(int time, t_philo *phil);
void				msg_eat(int time, t_philo *phil);
void				msg_slp(int time, t_philo *phil);
void				msg_left_fork(int time, t_philo *phil);
void				msg_right_fork(int time, t_philo *phil);
void				write_msg(int time, int id, const char *action, pthread_mutex_t *writing);

#endif
