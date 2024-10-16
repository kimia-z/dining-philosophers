/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kziari <kziari@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 12:26:25 by kziari            #+#    #+#             */
/*   Updated: 2024/10/14 12:26:27 by kziari           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

bool	join_threads(t_table *table, int num_threads)
{
	int	j;

	j = 0;
	while (j < num_threads)
	{
		if (pthread_join(table->philo[j].t_id_philo, NULL) != 0)
			return (error_msg("thread joining failed!"), false);
		j++;
	}
	if (num_threads == table->nb_philo)
		return (true);
	return (error_msg("thread joining failed!"), false);
}

bool	init_threads(t_table *table)
{
	int	i;

	i = 0;
	pthread_mutex_lock(&table->lock);
	while (i < table->nb_philo)
	{
		if (pthread_create(&table->philo[i].t_id_philo,
				NULL, &routine, &table->philo[i]) != 0)
			return (join_threads(table, i));
		i++;
	}
	if (pthread_create(&table->t_id_table, NULL, &table_observer, table) != 0)
		return (error_msg("thread creation failed!"), false);
	table->start_flag = 1;
	table->start_time = get_time();
	pthread_mutex_unlock(&table->lock);
	if (pthread_join(table->t_id_table, NULL) != 0)
		return (error_msg("thread joining failed!"), false);
	return (join_threads(table, i));
}
