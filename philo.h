/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kziari <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/04 11:36:47 by kziari            #+#    #+#             */
/*   Updated: 2024/06/04 11:36:48 by kziari           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <sys/time.h>
# include <pthread.h>
# include <limits.h>
# include <stdbool.h>

struct	s_table;

typedef struct s_philo
{
	int				id;
	pthread_t		*t_id_philo;
	struct s_table	*table;
	int				eat_count;
	int				eating;
	long long		time_be_dead;
	// long long		start_thinking;
	pthread_mutex_t	lock;
	pthread_mutex_t	*right_fork;
	pthread_mutex_t	*left_fork;
}	t_philo;

typedef struct s_table
{
	pthread_t		t_id_table;
	int				nb_philo;
	long long		time_die;
	long long		time_eat;
	long long		time_sleep;
	int				nb_meals;
	int				dead;
	int				finished;
	t_philo			*philo;
	long long		start_time;
	pthread_mutex_t	*forks; 
	pthread_mutex_t	lock;
	pthread_mutex_t	write;
}	t_table;


#endif
