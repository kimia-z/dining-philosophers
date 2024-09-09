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

int	check_digit(char *arg)
{
	int	i;

	i = 0;
	if (!arg[i])
		return (-1);
	while (arg[i])
	{
		if (arg[i] < '0' || arg[i] > '9')
			return (-1);
		i++;
	}
	return (0);
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
	// if (pthread_mutex_lock(&philo->table->write) != 0)
	// {
	// 	return (-1);
	// }
	printf("%lld %d %s", get_time(), philo->id, msg);
	// if (pthread_mutex_unlock(&philo->table->write) != 0)
	// {
	// 	return (-1);
	// }
	return (0);
}

int	take_forks(t_philo *philo)
{
	if (pthread_mutex_lock(philo->right_fork) != 0)
		return (-1);
	//msg fork taken
	write_msg(philo, "has taken a fork\n");
	if (pthread_mutex_lock(philo->left_fork) != 0)
		return (-1);
	//msg fork taken
	write_msg(philo, "has taken a fork\n");
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

// void	*table_observer(void *data)
// {
// 	t_philo	*philo;

// 	philo = (t_philo *)data;
// 	while (philo->table->dead == 0)
// 	{
// 		if (pthread_mutex_lock(&philo->lock) != 0)
// 			return ((void *)(-1));
// 		if (philo->eat_count == philo->table->nb_meals)
// 		{
// 			if (pthread_mutex_lock(&philo->table->lock) != 0)
// 				return ((void *)(-1));
// 			philo->table->finished ++;
// 			philo->eat_count++;
// 			if (philo->table->finished == philo->table->nb_philo)
// 			{
// 				philo->table->dead = 1;
// 			}
// 			printf("dead=%d\n", philo->table->dead);
// 			if (pthread_mutex_unlock(&philo->table->lock) != 0)
// 				return ((void *)(-1));
// 		}
// 		if (pthread_mutex_unlock(&philo->lock) != 0)
// 			return ((void *)(-1));
// 	}
// 	// if (pthread_mutex_lock(&philo->table->write) != 0)
// 	// 	return ((void *)(-1));
// 	// printf("dead=%d\n", philo->table->dead);
// 	// if (pthread_mutex_unlock(&philo->table->write) != 0)
// 	// 	return ((void *)(-1));
// 	// while (philo->table->dead == 0)
// 	// {
// 	// 	if (pthread_mutex_lock(&philo->table->lock) != 0)
// 	// 		return ((void *)-1);
// 	// 	if (philo->table->finished == philo->table->nb_philo)
// 	// 	{
// 	// 		philo->table->dead = 1;
// 	// 	}
// 	// 	if (pthread_mutex_unlock(&philo->table->lock) != 0)
// 	// 		return ((void *)(-1));
// 	// }
// 	return ((void *)(0));
// }

// /*Purpose: Each philosopher's routine thread handles the philosopher's actions,
// 	including eating, thinking, and possibly dying.*/

void	*routine(void *data)
{
	printf("injaaaaaa\n");
	t_philo	*philo;

	philo = (t_philo *)data;
	philo->time_be_dead = philo->table->time_die + get_time();
	while (philo->table->dead == 0)
	{
		//eat
		if (eat(philo) != 0)
			return ((void *)-1);
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
// 		while (position >= 0)
// 		{
// 			pthread_join(philo->t_id_philo[position], NULL);
// 			position--;
// 		}
// 		return (-1);
// 	}
// 	if (pthread_join(philo->t_id_philo[position], NULL) != 0)
// 	{
// 		return (-1);
// 	}
// }

// int	init_thread(t_table *table, t_philo *philo)
// {
// 	int			i;
// 	int			create_value;
// 	int			join_value;

// 	i = 0;
// 	//start time
// 	create_value = pthread_create(table->t_id_table, NULL, &table_observer, &philo);
// 	//it is only one if it fails it can not join and do not need join
// 	if (join_function(philo, create_value, 0, false) == -1)
// 	{
// 		return (-1);
// 	}
// 	table->start_time = get_time();
// 	while(i < table->nb_philo)
// 	{
// 		create_value = pthread_create(philo->t_id_philo[i], NULL, &routine, &philo[i]);
// 		if (join_function(philo, create_value, i, true) == -1)
// 		{
// 			return (-1);
// 		}
// 		i++;
// 	}
// }

int	one_philo(t_table *table)
{
	// take a fork
	// wait for the die time
	//die and end
	table->start_time = get_time();
	// write_msg(table->philo, "kar mikone???\n");
	if (pthread_create(&table->philo->t_id_philo[0], NULL, &routine, &table->philo[0]) != 0)
	{
		return (-1);
	}
	if (pthread_join(table->philo->t_id_philo[0], NULL) != 0)
	{
		return (-1);
	}
	while (table->dead == 0)
	{
		usleep(0);
	}
	return (0);
}

int	init_philo(t_table *table, t_philo *philo)
{
	int		i;

	i = 0;
	philo->t_id_philo = malloc (table->nb_philo * sizeof(pthread_t));
	if (philo->t_id_philo == NULL)
	{
		return (-1);
	}
	while (i < table->nb_philo)
	{
		philo[i].id = i + 1;
		philo[i].table = table;
		philo[i].time_be_dead = table->time_die;
		philo[i].eat_count = 0;
		philo[i].eating = 0;
		philo[i].right_fork = &table->forks[i % table->nb_philo];
		philo[i].left_fork = &table->forks[(i + 1) % table->nb_philo];
		pthread_mutex_init(&philo[i].lock, NULL);
		i++;
	}
	return (0);
}

int	init_table(char *argv[], t_table *table)
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
		return (-1);
	}
	table->dead = 0;
	table->finished = 0;
	table->philo = malloc(table->nb_philo * sizeof(t_philo));
	if (table->philo == NULL)
	{
		return (-1);
	}
	table->forks = malloc(table->nb_philo * sizeof(pthread_mutex_t));
	if (table->forks == NULL)
	{
		return (-1);
	}
	while (i < table->nb_philo)
	{
		pthread_mutex_init(&table->forks[i], NULL);
		i++;
	}
	if (pthread_mutex_init(&table->lock, NULL) != 0)
	{
		return (-1);
	}
	if (pthread_mutex_init(&table->write, NULL) != 0)
	{
		return (-1);
	}
	return (0);
}

int	init_all(int argc, char **argv, t_table *table, t_philo *philo)
{
	int	i;
	

	i = 1;
	while (i < argc)
	{
		if (check_digit(argv[i]) == -1)
			return (-1);
		i++;
	}
	if (init_table(argv, table) == -1)
	{
		return (-1);
	}
	if (init_philo(table, philo) == -1)
	{
		return (-1);
	}
	return (0);
}

int	main(int argc, char *argv[])
{
	t_table	table;
	t_philo	philo;

	if (argc != 5 && argc != 6)
	{
		printf("invalid number of arguments!\n");
		return (-1);
	}
	if (init_all(argc, argv, &table, &philo) == -1)
	{
		printf("invalid arguments!\n");
		return (-1);
	}
	if (table.nb_philo == 1)
	{
		return (one_philo(&table));
	}
	// if (init_thread(&table, &philo) == -1)
	// {
	// 	return (-1);
	// }
	// // exit and clean
	// clean_up();
	return (0);
}
