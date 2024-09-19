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

int	ft_atoi(const char *nptr)
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

long long	get_time()
{
	struct timeval	current_time;

	if (gettimeofday(&current_time, NULL) != 0)
		return (-1);
	return ((current_time.tv_sec * (long long)1000) + (current_time.tv_usec / 1000));
}

int	write_msg(t_philo *philo, char *msg)
{
	if (pthread_mutex_lock(&philo->table->write) != 0)
	{
		return (-1);
	}
	printf("%lld %d %s", get_time(), philo->id, msg);
	//printf("get time:%lld be dead:%lld\n",get_time(), philo->time_be_dead);
	if (pthread_mutex_unlock(&philo->table->write) != 0)
	{
		return (-1);
	}
	return (0);
}

int	take_forks(t_philo *philo)
{
	if (pthread_mutex_lock(philo->right_fork) != 0)
		return (-1);
	write_msg(philo, "has taken a right fork\n");
	if (pthread_mutex_lock(philo->left_fork) != 0)
		return (-1);
	write_msg(philo, "has taken a left fork\n");
	return (0);
}

int	drop_forks(t_philo *philo)
{
	if (pthread_mutex_unlock(philo->left_fork) != 0)
		return (-1);
	if (pthread_mutex_unlock(philo->right_fork) != 0)
		return (-1);
	//sleep
	return (0);
}

int	eat(t_philo *philo)
{
	if (take_forks(philo) == -1)
		return (-1);
	if (pthread_mutex_lock(&philo->lock) != 0)
		return (-1);
	philo->eating = 1;
	philo->eat_count++;
	philo->time_be_dead = get_time() + philo->table->time_die;
	write_msg(philo, "is eating\n");
	if (usleep((philo->table->time_eat) * 1000) != 0)
		return (-1);
	philo->eating = 0;
	if (pthread_mutex_unlock(&philo->lock) != 0)
		return (-1);
	if (drop_forks(philo) == -1)
		return (-1);
	return (0);
}

// /*Purpose: checks the state of the simulation.
// 		It locks the write mutex to safely print
// 		the current state of the simulation (specifically, whether someone is dead).
// */

void	*table_observer(void *data)
{
	t_table	*table;

	table = (t_table *)data;
	// printf("in table observer dead:%d\n", table->dead);
	// printf("in table observer time sleep:%lld\n", table->time_sleep);
	while (table->dead == 0)
	{
		if (pthread_mutex_lock(&table->lock) != 0)
			return ((void *)(-1));
		// if (pthread_mutex_lock(&table->philo->lock) != 0)
		// 	return ((void *)(-1));
		//printf("in table observer\n");
		if (table->philo->eat_count == table->nb_meals)
		{
			//printf("in table observer2222222222\n");
			table->finished++;
			table->philo->eat_count++;
			if (table->finished == table->nb_philo)
			{
				table->dead = 1;
			}
		}
		//printf("get time:%lld be dead:%lld\n",get_time(), table->philo->time_be_dead);
		if (get_time() > table->philo->time_be_dead)
		{
			//printf("time be dead:%lld\n", table->philo->time_be_dead);
			// printf("get time:%lld be dead:%lld\n",get_time(), table->philo->time_be_dead);
			// printf("someone dead!!!!");
			write_msg(table->philo, "deaddddddd\n");
			table->dead = 1;
		}
		// if (pthread_mutex_unlock(&table->philo->lock) != 0)
		// 	return ((void *)(-1));
		if (pthread_mutex_unlock(&table->lock) != 0)
			return ((void *)(-1));
	}
	return ((void *)(0));
}

// /*Purpose: Each philosopher's routine thread handles the philosopher's actions,
// 	including eating, thinking, and possibly dying.*/

void	*routine(void *data)
{
	t_philo	*philo;

	philo = (t_philo *)data;
	// philo->time_be_dead = philo->table->time_die + get_time();
	//printf("start of routine\n");
	while (philo->table->dead == 0)
	{
		//printf("in routine2222222\n");
		//eat
		if (eat(philo) != 0)
			return ((void *)-1);
		//printf("in routine3333333\n");
		//sleep
		write_msg(philo, "is sleeping\n");
		if (usleep((philo->table->time_sleep) * 1000) != 0)
			return ((void *)-1);
		//think
		if (get_time() > philo->time_be_dead)
		{
			philo->table->dead = 1;
		}
	}
	return ((void *)(0));
}

// int	join_function(t_philo *philo, int ret_value, int position, bool is_philo)
// {
// 	if (!is_philo && pthread_join(philo->table->t_id_table, NULL) != 0)
// 	{
// 		return (-1);
// 	}
// 	else if (is_philo && ret_value != 0)
// 	{
// 		while (position >= 1)
// 		{
// 			pthread_join(philo->t_id_philo[position - 1], NULL);
// 			position--;
// 		}
// 		return (-1);
// 	}
// 	if (pthread_join(philo->t_id_philo[position], NULL) != 0)
// 	{
// 		return (-1);
// 	}
// }

// int	init_thread(t_table *table)
// {
// 	int			i;
// 	int			create_value;
// 	int			join_value;

// 	i = 0;
// 	table->start_time = get_time();
// 	create_value = pthread_create(table->t_id_table, NULL, &table_observer, table);
// 	//it is only one if it fails it can not join and do not need join
// 	if (join_function(table->philo, create_value, 0, false) == -1)
// 	{
// 		return (-1);
// 	}
// 	//table->start_time = get_time();
// 	while(i < table->nb_philo)
// 	{
// 		create_value = pthread_create(table->philo[i].t_id_philo, NULL, &routine, &table->philo[i]);
// 		if (join_function(table->philo, create_value, i, true) == -1)
// 		{
// 			return (-1);
// 		}
// 		i++;
// 	}
// }

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
	table->start_time = get_time();
	pthread_mutex_unlock(&table->lock);
	if (pthread_join(table->t_id_table, NULL) != 0)
		return (error_msg("thread joining failed!"), false);
	return (join_threads(table, i));
}

bool	one_philo(t_table *table)
{
	table->start_time = get_time();
	table->philo->time_be_dead = table->time_die + get_time();
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
	// while (table->dead == 0)
	// {
	// 	usleep(500);
	// }
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
		table->philo[i].time_be_dead = table->time_die;
		table->philo[i].eat_count = 0;
		table->philo[i].eating = 0;
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
	if ((table->nb_philo < 1 || table->nb_philo > 200) || (table->time_die < 0) ||
		(table->time_eat < 0) || (table->time_sleep < 0) ||
		(table->nb_meals && table->nb_meals < -1))
	{
		return (error_msg("invalid arguments!"), false);
	}
	table->dead = 0;
	table->finished = 0;
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

int	main(int argc, char *argv[])
{
	t_table	table;

	if (argc != 5 && argc != 6)
	{
		return (error_msg("invalid number of arguments!"), 0);
	}
	if (!init_all(argc, argv, &table))
	{
		return (0);
	}
	if (table.nb_philo == 1)
	{
		return (one_philo(&table));
	}
	// if (!init_threads(&table))
	// {
	// 	return (0);
	// }
	// // exit and clean
	// clean_up();
	return (0);
}
