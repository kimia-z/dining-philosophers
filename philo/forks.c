/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   forks.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kziari <kziari@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 12:28:41 by kziari            #+#    #+#             */
/*   Updated: 2024/10/14 12:28:43 by kziari           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	take_forks(t_philo *philo)
{
	if (philo->id % 2 == 0)
	{
		pthread_mutex_lock(philo->right_fork);
		if (is_dead(philo) == false)
			write_msg(philo, "has taken a fork\n");
		pthread_mutex_lock(philo->left_fork);
		if (is_dead(philo) == false)
			write_msg(philo, "has taken a fork\n");
	}
	else
	{
		pthread_mutex_lock(philo->left_fork);
		if (is_dead(philo) == false)
			write_msg(philo, "has taken a fork\n");
		pthread_mutex_lock(philo->right_fork);
		if (is_dead(philo) == false)
			write_msg(philo, "has taken a fork\n");
	}
}

void	drop_forks(t_philo *philo)
{
	pthread_mutex_unlock(philo->left_fork);
	pthread_mutex_unlock(philo->right_fork);
}


// void	take_forks(t_philo *philo)
// {
// 	if (philo->id % 2 == 0)
// 	{
// 		if (pthread_mutex_lock(philo->right_fork) == 0)
// 		{
// 			if (pthread_mutex_lock(philo->left_fork) == 0)
// 			{
// 				write_msg2(philo, "has taken a fork\n");
// 			}
// 			else
// 			{
// 				pthread_mutex_unlock(philo->right_fork);
// 			}
// 		}
// 	}
// 	else
// 		if (pthread_mutex_lock(philo->left_fork) == 0)
// 		{
// 			if (pthread_mutex_lock(philo->right_fork) == 0)
// 			{
// 				write_msg2(philo, "has taken a fork\n");
// 			}
// 			else
// 			{
// 				pthread_mutex_unlock(philo->left_fork);
// 			}
// 		}
// }

// void	drop_forks(t_philo *philo)
// {
// 	pthread_mutex_unlock(philo->left_fork);
// 	pthread_mutex_unlock(philo->right_fork);
// }
