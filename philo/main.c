/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kziari <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/04 11:31:51 by kziari            #+#    #+#             */
/*   Updated: 2024/06/04 11:31:53 by kziari           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	error_msg(char *msg)
{
	printf("%s\n", msg);
}

void	write_msg(t_philo *philo, char *msg)
{
	pthread_mutex_lock(&philo->table->write);
	if (is_dead(philo) == false)
		printf("%d %d %s",
			calculate_time(philo->table->start_time), philo->id, msg);
	pthread_mutex_unlock(&philo->table->write);
}

// void	write_msg2(t_philo *philo, char *msg)
// {
// 	int time;
	
// 	pthread_mutex_lock(&philo->table->write);
// 	time = calculate_time(philo->table->start_time);
// 	if (is_dead(philo) == false)
// 	{
// 		printf("%d %d %s", time, philo->id, msg);
// 		printf("%d %d %s",time, philo->id, msg);	
// 	}		
// 	pthread_mutex_unlock(&philo->table->write);
// }

void	clear_data(t_table *table)
{
	if (table->forks)
		free(table->forks);
	if (table->philo)
		free(table->philo);
}

void	clean_up(t_table *table)
{
	int	i;

	i = 0;
	if (table != NULL)
	{
		pthread_mutex_destroy(&table->end);
		pthread_mutex_destroy(&table->write);
		pthread_mutex_destroy(&table->lock);
		while (i < table->nb_philo)
		{
			pthread_mutex_destroy(&table->philo[i].lock);
			pthread_mutex_destroy(&table->forks[i]);
			i++;
		}
	}
	clear_data(table);
}

int	main(int argc, char *argv[])
{
	t_table	table;

	if (argc != 5 && argc != 6)
	{
		return (error_msg("invalid number of arguments!"), 0);
	}
	if (!init_all(argc, argv, &table))
	{
		clear_data(&table);
		return (0);
	}
	if (!init_threads(&table))
	{
		clean_up(&table);
		return (0);
	}
	clean_up(&table);
	return (0);
}
