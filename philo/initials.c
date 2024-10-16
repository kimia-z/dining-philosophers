/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initials.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kziari <kziari@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 12:25:10 by kziari            #+#    #+#             */
/*   Updated: 2024/10/14 12:25:13 by kziari           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static bool	init_philo(t_table *table)
{
	int		i;

	i = 0;
	while (i < table->nb_philo)
	{
		table->philo[i].id = i + 1;
		table->philo[i].table = table;
		table->philo[i].life_time = table->time_die;
		table->philo[i].last_meal_time = get_time();
		table->philo[i].eat_count = 0;
		table->philo[i].finished = 0;
		table->philo[i].nb_meal = table->nb_meals;
		table->philo[i].right_fork = &table->forks[i % table->nb_philo];
		table->philo[i].left_fork = &table->forks[(i + 1) % table->nb_philo];
		if (pthread_mutex_init(&(table->philo[i]).lock, NULL) != 0)
		{
			return (error_msg("mutex initial failed!"), false);
		}
		i++;
	}
	return (true);
}

static bool	init_memory_mutex(t_table *table)
{
	int	i;

	i = 0;
	table->philo = malloc(table->nb_philo * sizeof(t_philo));
	if (table->philo == NULL)
		return (error_msg("memory allocation failed!"), false);
	table->forks = malloc(table->nb_philo * sizeof(pthread_mutex_t));
	if (table->forks == NULL)
		return (error_msg("memory allocation failed!"), false);
	while (i < table->nb_philo)
	{
		pthread_mutex_init(&table->forks[i], NULL);
		i++;
	}
	if (pthread_mutex_init(&table->lock, NULL) != 0)
		return (error_msg("mutex initial failed!"), false);
	if (pthread_mutex_init(&table->write, NULL) != 0)
		return (error_msg("mutex initial failed!"), false);
	if (pthread_mutex_init(&table->end, NULL) != 0)
		return (error_msg("mutex initial failed!"), false);
	return (true);
}

static bool	init_table(char *argv[], t_table *table)
{
	table->nb_philo = ft_atoi(argv[1]);
	table->time_die = ft_atoi(argv[2]);
	table->time_eat = ft_atoi(argv[3]);
	table->time_sleep = ft_atoi(argv[4]);
	if (argv[5])
		table->nb_meals = ft_atoi(argv[5]);
	else
		table->nb_meals = -1;
	if ((table->nb_philo < 1 || table->nb_philo > 200)
		|| (table->time_die < 0 || table->time_die > INT_MAX)
		|| (table->time_eat < 0 || table->time_eat > INT_MAX)
		|| (table->time_sleep < 0 || table->time_sleep > INT_MAX)
		|| (table->nb_meals
			&& (table->nb_meals < -1 || table->nb_meals > INT_MAX)))
	{
		return (error_msg("invalid arguments!"), false);
	}
	table->dead = 0;
	table->start_flag = 0;
	if (init_memory_mutex(table) == false)
		return (false);
	return (true);
}

bool	init_all(int argc, char **argv, t_table *table)
{
	int	i;

	i = 1;
	while (i < argc)
	{
		if (!check_digit(argv[i]))
			return (false);
		i++;
	}
	if (!init_table(argv, table))
	{
		return (false);
	}
	if (!init_philo(table))
	{
		return (false);
	}
	return (true);
}
