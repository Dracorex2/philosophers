/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucmansa <lucmansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/27 16:43:37 by lucmansa          #+#    #+#             */
/*   Updated: 2025/04/04 15:13:19 by lucmansa         ###   ########.fr       */
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
	int dead = 0;
	pthread_mutex_lock(&philo->context->mutex_die);
	if (philo->context->die)
		dead = 1;
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

void	*main_guard(void *arg)
{
	int				i;
	struct	timeval	actual_time;
	t_philo *philo;

	philo = arg;
	i = 0;
	while (!is_dead(philo))
	{
		if (i >= philo->context->philo_nb)
			i = 0;
		pthread_mutex_lock(&philo[i].last_eat_m);
		gettimeofday(&actual_time, NULL);
		if (get_time(philo[i].last_eat) > philo->context->time_death)
		{
			print(&philo[i], "died");
			pthread_mutex_lock(&philo->context->mutex_die);
			philo->context->die = 1;
			pthread_mutex_unlock(&philo->context->mutex_die);
		}
		pthread_mutex_unlock(&philo[i].last_eat_m);
		i++;
	}
	return (NULL);
}

int	wait(t_philo *philo, int i)
{
	struct	timeval	actual_time;
	struct	timeval	start_time;

	gettimeofday(&start_time, NULL);
	actual_time = start_time;
	while (((actual_time.tv_sec * 1000) + (actual_time.tv_usec / 1000)) -
		((start_time.tv_sec * 1000) + (start_time.tv_usec / 1000)) < i)
	{
		gettimeofday(&actual_time, NULL);
		if (is_dead(philo))
			return (1);
	}
	return (0);
}

int	eat(t_philo *philo, int n, int b)
{
	pthread_mutex_lock(&philo->context->fork[n]);
	pthread_mutex_lock(&philo->context->fork[b]);
	pthread_mutex_lock(&philo->last_eat_m);
	gettimeofday(&philo->last_eat, NULL);
	pthread_mutex_unlock(&philo->last_eat_m);
	if (print(philo, "is eating"))
		return (pthread_mutex_unlock(&philo->context->fork[n]), pthread_mutex_unlock(&philo->context->fork[b]), 1);
	wait(philo, philo->context->time_eat);
	pthread_mutex_unlock(&philo->context->fork[n]);
	pthread_mutex_unlock(&philo->context->fork[b]);
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
	gettimeofday(&philo->last_eat, NULL);
	while (1)
	{
		if (print(philo, "is thinking"))
			break;
		if (philo->id % 2)
		{
			if (eat(philo, fork_l, fork_r))
				break;
		}
		else
		{
			if (eat(philo, fork_r, fork_l))
				break;
		}
		if (print(philo, "is sleeping"))
			break;
		if (wait(philo, philo->context->time_sleep))
			break;
	}
	return (NULL);
}

int	main(int argc, char **argv)
{
	t_context 	context;
	int			i;
	t_philo		*philo;
	pthread_t	guard;

	memset(&context, 0, sizeof(t_context));
	context.philo_nb = ft_atoi(argv[1]);
	context.time_death = ft_atoi(argv[2]);
	context.time_eat = ft_atoi(argv[3]);
	context.time_sleep= ft_atoi(argv[4]);
	context.fork = malloc(context.philo_nb * sizeof(pthread_mutex_t));
	philo = malloc(context.philo_nb * sizeof(t_philo));
	pthread_mutex_init(&context.print, NULL);
	pthread_mutex_init(&context.mutex_die, NULL);
	i = -1;
	gettimeofday(&context.time_start, NULL);
	while (++i < context.philo_nb)
	{
		printf("init %i\n", i);
		philo[i].context = &context;
		philo[i].id = i;
		pthread_mutex_init(&context.fork[i], NULL);
		pthread_mutex_init(&philo[i].last_eat_m, NULL);
		pthread_create(&philo[i].thread, NULL, main_philo, &philo[i]);
	}
	pthread_create(&guard, NULL, main_guard, philo);
	i = -1;
	while (++i < context.philo_nb)
		pthread_join(philo[i].thread, NULL);
	pthread_join(guard, NULL);
}
