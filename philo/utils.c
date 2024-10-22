/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kziari <kziari@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 12:29:12 by kziari            #+#    #+#             */
/*   Updated: 2024/10/14 12:29:15 by kziari           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

bool	check_digit(char *arg)
{
	int	i;

	i = 0;
	if (!arg[i])
		return (error_msg("invalid argument!"), false);
	while (arg[i])
	{
		if (arg[i] < '0' || arg[i] > '9')
			return (error_msg("invalid argument!"), false);
		i++;
	}
	return (true);
}

long	ft_atoi(const char *nptr)
{
	int		i;
	int		sign;
	long	result;

	result = 0;
	sign = 1;
	i = 0;
	while ((nptr[i] > 8 && nptr[i] < 14) || nptr[i] == 32)
	{
		i++;
	}
	if (nptr[i] == '-' || nptr[i] == '+')
	{
		if (nptr[i] == '-')
			sign = -1;
		i++;
	}
	while (nptr[i] >= '0' && nptr[i] <= '9')
	{
		result = (result * 10) + (nptr[i] - '0');
		i++;
	}
	return (sign * (int)result);
}

long	get_time(void)
{
	struct timeval	current_time;

	gettimeofday(&current_time, NULL);
	return ((current_time.tv_sec * (long)1000) + (current_time.tv_usec / 1000));
}

int	calculate_time(long start_time)
{
	return (get_time() - start_time);
}

void	ft_usleep(int action)
{
	long	goal_time;

	goal_time = get_time() + action;
	while (goal_time > get_time())
	{
		usleep(500);
	}
}
