/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucmansa <lucmansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/27 16:43:37 by lucmansa          #+#    #+#             */
/*   Updated: 2025/04/07 19:15:00 by lucmansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosopers.h"

int	ft_atoi(const char *nptr)
{
	int	nbr;
	int	i;
	int	sign;

	nbr = 0;
	i = 0;
	sign = 1;
	while (nptr[i] == ' ' || (nptr[i] >= '\t' && nptr[i] <= '\r'))
		i++;
	if (nptr[i] == '-' || nptr[i] == '+')
		if (nptr[i++] == '-')
			sign *= -1;
	while (nptr[i] >= '0' && nptr[i] <= '9')
		nbr = nbr * 10 + (nptr[i++] - '0');
	return (nbr * sign);
}

int	is_dead(t_philo *philo)
{
	int dead;

	dead  = 0;
	pthread_mutex_lock(&philo->context->mutex_die);
	dead = philo->context->die;
	pthread_mutex_unlock(&philo->context->mutex_die);
	return dead;
}

int	get_time(struct	timeval time)
{
	struct	timeval actual_time;

	gettimeofday(&actual_time, NULL);
	actual_time.tv_sec -= time.tv_sec;
	actual_time.tv_usec -= time.tv_usec;
	return (actual_time.tv_sec * 1000 + actual_time.tv_usec / 1000);
}

int	print(t_philo *philo, char *str)
{
	int	timestamp;

	pthread_mutex_lock(&philo->context->print);
	timestamp = get_time(philo->context->time_start);
	if (is_dead(philo))
		return (pthread_mutex_unlock(&philo->context->print), 1);	
	printf("%i %i %s\n", timestamp, philo->id, str);
	pthread_mutex_unlock(&philo->context->print);
	return (0);
}

int	check_eat(t_philo *philo, int i)
{
	pthread_mutex_lock(&philo->mutex_eat_count);
	if (i == 1)
		philo->eat_count++;
	else
	{
		if (philo->as_count && philo->eat_count >= philo->context->nb_eat)
			return (pthread_mutex_unlock(&philo->mutex_eat_count), 1);
	}
	pthread_mutex_unlock(&philo->mutex_eat_count);
	return (0);
}

int	check_all_eat(t_philo *philo)
{
	int	i;

	i = -1;
	while (++i < philo->context->philo_nb)
		if (check_eat(&philo[i], 0))
			return (0);
	return (1);
}

void	*main_guard(void *arg)
{
	int				i;
	struct	timeval	actual_time;
	t_philo *philos;

	philos = arg;
	i = -1;
	while (!is_dead(philos) && check_all_eat(philos))
	{
		if (++i >= philos->context->philo_nb)
			i = 0;
		if (check_eat(&philos[i], 0))
			break ;
		pthread_mutex_lock(&philos[i].eat_mutex);
		gettimeofday(&actual_time, NULL);
		if (get_time(philos[i].last_eat) > philos->context->time_death)
		{
			print(&philos[i], "died");
			pthread_mutex_lock(&philos->context->mutex_die);
			philos->context->die = 1;
			pthread_mutex_unlock(&philos->context->mutex_die);
		}
		pthread_mutex_unlock(&philos[i].eat_mutex);
		usleep(1);
	}
	return (NULL);
}

int	wait(t_philo *philo, int i)
{
	struct	timeval	start_time;

	gettimeofday(&start_time, NULL);
	while (1)
	{
		if (get_time(start_time) >= i)
			return (0);
		usleep(1);
		if (is_dead(philo))
			return (1);
	}
	return (0);
}

int	eat(t_philo *philo, int n, int b)
{
	pthread_mutex_lock(&philo->context->fork[n]);
	if (print(philo, "has taken a fork"))
		return (pthread_mutex_unlock(&philo->context->fork[n]), 1);
	if (philo->context->philo_nb == 1)
		return (pthread_mutex_unlock(&philo->context->fork[n]), 1);
	pthread_mutex_lock(&philo->context->fork[b]);
	if (print(philo, "has taken a fork"))
		return (pthread_mutex_unlock(&philo->context->fork[n]), pthread_mutex_unlock(&philo->context->fork[b]), 1);
	pthread_mutex_lock(&philo->eat_mutex);
	gettimeofday(&philo->last_eat, NULL);
	pthread_mutex_unlock(&philo->eat_mutex);
	if (print(philo, "is eating"))
		return (pthread_mutex_unlock(&philo->context->fork[n]), pthread_mutex_unlock(&philo->context->fork[b]), 1);
	wait(philo, philo->context->time_eat);
	check_eat(philo, 1);
	pthread_mutex_unlock(&philo->context->fork[b]);
	pthread_mutex_unlock(&philo->context->fork[n]);
	return (0);
}

void	*main_philo(void *arg)
{
	t_philo *philo;
	int		fork_r;
	int		fork_l;

	philo = (t_philo *)arg;
	fork_l = philo->id;
	fork_r = philo->id + 1;
	if (philo->id == philo->context->philo_nb - 1)
		fork_r = 0;
	while (!is_dead(philo) && !check_eat(philo, 0))
	{
		if (print(philo, "is thinking"))
			break;
		if (philo->id % 2 && eat(philo, fork_l, fork_r))
			break;
		else if (!(philo->id % 2) && eat(philo, fork_r, fork_l))
			break;
		if (!print(philo, "is sleeping") && wait(philo, philo->context->time_sleep))
			break;
	}
	return (NULL);
}

int	create_philos(t_philo *philo, t_context *context)
{
	int	i;
	pthread_t	guard;

	i = -1;
	while (++i < context->philo_nb)
	{
		philo[i].context = context;
		philo[i].id = i;
		philo[i].eat_count = 0;
		philo[i].as_count = 0;
		if (philo[i].context->nb_eat)
			philo[i].as_count = 1;
		gettimeofday(&philo[i].last_eat, NULL);
		pthread_mutex_init(&context->fork[i], NULL);
		pthread_mutex_init(&philo[i].eat_mutex, NULL);
		pthread_mutex_init(&philo[i].mutex_eat_count, NULL);
	}
	i = -1;
	while (++i < context->philo_nb)
		pthread_create(&philo[i].thread, NULL, main_philo, &philo[i]);
	pthread_create(&guard, NULL, main_guard, philo);
	i = -1;
	while (++i < context->philo_nb)
		pthread_join(philo[i].thread, NULL);
	pthread_join(guard, NULL);
	return (0);
}

int	main(int argc, char **argv)
{
	t_context 	context;
	t_philo		*philo;

	if (argc < 5)
		return (0);
	context.die = 0;
	context.philo_nb = ft_atoi(argv[1]);
	context.time_death = ft_atoi(argv[2]);
	context.time_eat = ft_atoi(argv[3]);
	context.time_sleep = ft_atoi(argv[4]);
	context.nb_eat = 0;
	if (context.philo_nb < 1 || context.time_death < 1 || context.time_eat < 1 || context.time_sleep < 1)
		return (0);
	context.fork = malloc(context.philo_nb * sizeof(pthread_mutex_t));
	philo = malloc(context.philo_nb * sizeof(t_philo));
	if (argc > 5)
		context.nb_eat = ft_atoi(argv[5]);
	pthread_mutex_init(&context.print, NULL);
	pthread_mutex_init(&context.mutex_die, NULL);
	gettimeofday(&context.time_start, NULL);
	create_philos(philo, &context);
	free(philo);
	free(context.fork);
	return (0);
}
