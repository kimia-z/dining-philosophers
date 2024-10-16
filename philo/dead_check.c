/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dead_check.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kziari <kziari@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 12:27:14 by kziari            #+#    #+#             */
/*   Updated: 2024/10/14 12:27:15 by kziari           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

bool	is_dead(t_philo *philo)
{
	pthread_mutex_lock(&philo->table->end);
	if (philo->table->dead == 1)
	{
		pthread_mutex_unlock(&philo->table->end);
		return (true);
	}
	pthread_mutex_unlock(&philo->table->end);
	return (false);
}

bool	is_starve(t_table *table)
{
	int	i;

	i = 0;
	while (i < table->nb_philo)
	{
		pthread_mutex_lock(&table->philo[i].lock);
		if (get_time() - table->philo[i].last_meal_time >= table->time_die)
		{
			pthread_mutex_unlock(&table->philo[i].lock);
			pthread_mutex_lock(&table->end);
			table->dead = 1;
			pthread_mutex_unlock(&table->end);
			printf("%d %d died\n", calculate_time(table->start_time), i + 1);
			return (true);
		}
		else
			pthread_mutex_unlock(&table->philo[i].lock);
		i++;
	}
	return (false);
}

bool	is_all_full(t_table *table)
{
	int	i;
	int	count;

	i = 0;
	count = 0;
	while (i < table->nb_philo)
	{
		pthread_mutex_lock(&table->philo[i].lock);
		if (table->philo[i].finished == 1)
		{
			count++;
		}
		pthread_mutex_unlock(&table->philo[i].lock);
		i++;
	}
	if (count == table->nb_philo)
	{
		pthread_mutex_lock(&table->end);
		table->dead = 1;
		pthread_mutex_unlock(&table->end);
		return (true);
	}
	return (false);
}
