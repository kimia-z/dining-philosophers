/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kziari <kziari@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 15:02:21 by kziari            #+#    #+#             */
/*   Updated: 2024/10/14 15:02:24 by kziari           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	*table_observer(void *data)
{
	t_table	*table;

	table = (t_table *)data;
	if (table->nb_meals == -1)
	{
		while (is_starve(table) == false)
		{
			usleep(500);
		}
	}
	else
	{
		while (is_starve(table) == false && is_all_full(table) == false)
		{
			usleep(500);
		}
	}
	return (NULL);
}

void	one_philo(t_philo *philo)
{
	pthread_mutex_lock(philo->right_fork);
	write_msg(philo, "has taken a fork\n");
	ft_usleep(philo->table->time_die, philo);
	pthread_mutex_unlock(philo->right_fork);
}

void	routine_helper(t_philo *philo)
{
	take_forks(philo);
	pthread_mutex_lock(&philo->lock);
	philo->last_meal_time = get_time();
	philo->eat_count++;
	if (philo->eat_count >= philo->nb_meal)
		philo->finished = 1;
	pthread_mutex_unlock(&philo->lock);
	if (is_dead(philo) == false)
		write_msg(philo, "is eating\n");
	ft_usleep(philo->table->time_eat, philo);
	drop_forks(philo);
	if (is_dead(philo) == false)
		write_msg(philo, "is sleeping\n");
	ft_usleep(philo->table->time_sleep, philo);
	if (is_dead(philo) == false)
		write_msg(philo, "is thinking\n");
	pthread_mutex_lock(&philo->lock);
	philo->life_time = philo->table->time_die
		- (get_time() - philo->last_meal_time);
	pthread_mutex_unlock(&philo->lock);
	if (philo->life_time > philo->table->time_eat)
		ft_usleep(philo->table->time_eat, philo);
}

void	*routine(void *data)
{
	t_philo	*philo;

	philo = (t_philo *)data;
	pthread_mutex_lock(&philo->table->lock);
	if (philo->table->start_flag == 0)
	{
		pthread_mutex_unlock(&philo->table->lock);
		return (NULL);
	}
	pthread_mutex_unlock(&philo->table->lock);
	if (philo->table->nb_philo == 1)
		one_philo(philo);
	else
	{
		if (philo->id % 2 == 0)
		{
			write_msg(philo, "is thinking\n");
			ft_usleep(philo->table->time_eat / 2, philo);
		}
		while (is_dead(philo) == false)
		{
			routine_helper(philo);
		}
	}
	return (NULL);
}
