/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjaimes <cjaimes@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/13 20:46:47 by cjaimes           #+#    #+#             */
/*   Updated: 2020/03/21 22:39:35 by cjaimes          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILOSOPHERS_H
# define PHILOSOPHERS_H
#include <pthread.h>
# include <stdio.h>
# include <unistd.h>
# include <stdlib.h>
# include <sys/time.h>
# include <semaphore.h>
# include <fcntl.h>
# include <sys/stat.h>
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
	sem_t			*forks;
	sem_t			*is_dead;
	sem_t			*writing;
	struct timeval	start;
	int				philo_num;
	uint64_t		time_to_die;
	uint64_t		time_to_eat;
	uint64_t		time_to_sleep;
	int				eat_cycles;
	int				somebody_died;
	int				can_stop;
}					t_setup;

/*
** alerts order: 0 fork left, 1 fork right
** 2 eating, 3 sleeping, 4 thinking, 5 ded
*/
typedef struct		s_philosophers
{
	int				number;
	sem_t			*has_eaten_enough_times;
	int				alerts[6];
	uint8_t			hands;
	t_setup			*setup;
	uint64_t		last_dinner_ts;
	int				dinners;
	int				is_eating;
}					t_philo;


void				ft_putnbr(int n);
int					ft_atoi(const char *input);
int					ft_strlen(const char *str);
char				*ft_strcpy(char *dst, const char *src);
char				*ft_strcat(char *s1, const char *s2);

void				check_msgs(t_philo *phil, int time);
void				set_msg(t_philo *phil, int msg);
void				*monitor_philos(void *stp);
int					unlock_forks(t_philo *philo);

void				*handle_philosopher(void *hi);

void 				wait_us(struct timeval t, uint64_t start, uint64_t dur);
void 				sleep_us(uint64_t dur);
uint64_t			elapsed_time(struct timeval start);

#endif