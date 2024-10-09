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

long	get_time()
{
	struct timeval	current_time;

	gettimeofday(&current_time, NULL);
	return ((current_time.tv_sec * (long)1000) + (current_time.tv_usec / 1000));
}

int	calculate_time(long start_time)
{
	return (get_time() - start_time);
}

void	ft_usleep(int action, t_philo *philo)
{
	long	goal_time;

	goal_time = get_time() + action;
	while(goal_time > get_time() && is_dead(philo) == false)
	{
		//printf("philo %d with goal time:%ld\n", philo->id, goal_time);
		usleep(500);
	}
}

void	write_msg(t_philo *philo, char *msg)
{
	pthread_mutex_lock(&philo->table->write);
	if (is_dead(philo) == false)
		printf("%d %d %s", calculate_time(philo->table->start_time), philo->id, msg);
	pthread_mutex_unlock(&philo->table->write);
}

void	take_forks(t_philo *philo)
{
	pthread_mutex_lock(philo->right_fork);
	if (is_dead(philo) == false)
		write_msg(philo, "has taken a right fork\n");
	pthread_mutex_lock(philo->left_fork);
	if (is_dead(philo) == false)
		write_msg(philo, "has taken a left fork\n");
}

void	drop_forks(t_philo *philo)
{
	pthread_mutex_unlock(philo->right_fork);
	//printf("%ld %d drop right fork\n", get_time(), philo->id);
	//write_msg(philo, "has droped a right fork\n");
	pthread_mutex_unlock(philo->left_fork);
	//printf("%ld %d drop left fork\n", get_time(), philo->id);
	//write_msg(philo, "has droped a left fork\n");
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
			//printf("philo %d: starving time = %ld\n", table->philo->id, get_time() - table->philo[i].last_meal_time);
			pthread_mutex_unlock(&table->philo[i].lock);
			pthread_mutex_lock(&table->end);
			table->dead = 1;
			pthread_mutex_unlock(&table->end);
			printf("%d %d died\n", calculate_time(table->start_time), i + 1);
			//write_msg(table->philo, "died\n");
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
	while(i < table->nb_philo)
	{
		pthread_mutex_lock(&table->philo[i].lock);
		if (table->philo[i].finished == 1)
		{
			count++;
		}
		pthread_mutex_unlock(&table->philo[i].lock);
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

void	*table_observer(void *data)
{
	t_table	*table;

	table = (t_table *)data;
	pthread_mutex_lock(&table->lock);
	pthread_mutex_unlock(&table->lock);
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

// /*Purpose: Each philosopher's routine thread handles the philosopher's actions,
// 	including eating, thinking, and possibly dying.*/

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
	if (philo->id % 2 == 0)
	{
		write_msg(philo, "is thinking\n");
		ft_usleep(philo->table->time_eat / 2, philo);
	}
	while (is_dead(philo) == false)
	{
		take_forks(philo);
		pthread_mutex_lock(&philo->lock);
		philo->last_meal_time = get_time();
		philo->eat_count++;
		if (philo->eat_count >= philo->nb_meal)
		{
			philo->finished = 1;
		}
		pthread_mutex_unlock(&philo->lock);
		if (is_dead(philo) == false)
		{
			write_msg(philo, "is eating\n");
		}
		ft_usleep(philo->table->time_eat, philo);
		drop_forks(philo);
		if (is_dead(philo) == false)
		{
			write_msg(philo, "is sleeping\n");
		}
		ft_usleep(philo->table->time_sleep, philo);
		if (is_dead(philo) == false)
		{
			write_msg(philo, "is thinking\n");
		}
		pthread_mutex_lock(&philo->lock);
		philo->life_time = philo->table->time_die - (get_time() - philo->last_meal_time);
		pthread_mutex_unlock(&philo->lock);
		if (philo->life_time > philo->table->time_eat)
			ft_usleep(philo->table->time_eat, philo);
	}
	return (NULL);
}

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
	int			i;

	i = 0;
	pthread_mutex_lock(&table->lock);
	while (i < table->nb_philo)
	{
		if (pthread_create(&table->philo[i].t_id_philo, NULL, &routine, &table->philo[i]) != 0)
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

bool	one_philo(t_table *table)
{
	table->start_time = get_time();
	//table->philo->life_time = table->time_die + get_time();
	if (pthread_create(&table->t_id_table, NULL, &table_observer, table) != 0)
	{
		return (error_msg("thread creation failed!"), false);
	}
	if (pthread_create(&table->philo->t_id_philo, NULL, &routine, &table->philo[0]) != 0)
	{
		return (error_msg("thread creation failed!"), false);
	}
	if (pthread_detach(table->philo->t_id_philo) != 0)
	{
		return (error_msg("thread detaching failed!"), false);
	}
	if (pthread_join(table->t_id_table, NULL) != 0)
	{
		return (error_msg("thread joining failed!"), false);
	}
	while (table->dead == 0)
	{
		usleep(500);
	}
	return (true);
}

bool	init_philo(t_table *table)
{
	int		i;

	i = 0;
	while (i < table->nb_philo)
	{
		table->philo[i].id = i + 1;
		table->philo[i].table = table;
		table->philo[i].life_time = table->time_die;
		table->philo[i].last_meal_time = get_time();
		// table->philo[i].start_time = get_time();
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

bool	init_table(char *argv[], t_table *table)
{
	int	i;

	i = 0;
	table->nb_philo = ft_atoi(argv[1]);
	table->time_die = ft_atoi(argv[2]);
	table->time_eat = ft_atoi(argv[3]);
	table->time_sleep = ft_atoi(argv[4]);
	if (argv[5])
		table->nb_meals = ft_atoi(argv[5]);
	else
		table->nb_meals = -1;
	if ((table->nb_philo < 1 || table->nb_philo > 200) || (table->time_die < 0 || table->time_die > INT_MAX ) ||
		(table->time_eat < 0 || table->time_eat > INT_MAX) || (table->time_sleep < 0 || table->time_sleep > INT_MAX) ||
		(table->nb_meals && (table->nb_meals < -1 || table->nb_meals > INT_MAX)))
	{
		return (error_msg("invalid arguments!"), false);
	}
	table->dead = 0;
	table->start_flag = 0;
	table->philo = malloc(table->nb_philo * sizeof(t_philo));
	if (table->philo == NULL)
	{
		return (error_msg("memory allocation failed!"), false);
	}
	table->forks = malloc(table->nb_philo * sizeof(pthread_mutex_t));
	if (table->forks == NULL)
	{
		return (error_msg("memory allocation failed!"), false);
	}
	while (i < table->nb_philo)
	{
		pthread_mutex_init(&table->forks[i], NULL);
		i++;
	}
	if (pthread_mutex_init(&table->lock, NULL) != 0)
	{
		return (error_msg("mutex initial failed!"), false);
	}
	if (pthread_mutex_init(&table->write, NULL) != 0)
	{
		return (error_msg("mutex initial failed!"), false);
	}
	if (pthread_mutex_init(&table->end, NULL) != 0)
	{
		return (error_msg("mutex initial failed!"), false);
	}
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
	if (table.nb_philo == 1)
	{
		return (one_philo(&table));
	}
	if (!init_threads(&table))
	{
		clean_up(&table);
		return (0);
	}
	clean_up(&table);
	return (0);
}
