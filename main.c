/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucmansa <lucmansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/27 16:43:37 by lucmansa          #+#    #+#             */
/*   Updated: 2025/04/01 18:00:56 by lucmansa         ###   ########.fr       */
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
	pthread_mutex_lock(&philo->context->mutex_die);
	if (philo->context->die)
		return (pthread_mutex_unlock(&philo->context->mutex_die), 1);
	return (pthread_mutex_unlock(&philo->context->mutex_die), 0);
}

void	*main_guard(void *arg)
{
	int				i;
	struct	timeval	actual_time;
	t_philo *philo;

	philo = arg;
	i = 0;
	pthread_mutex_lock(&philo[0].context->init);
	pthread_mutex_unlock(&philo[0].context->init);
	while (!is_dead(philo))
	{
		if (i > philo->context->philo_nb)
			i = 0;
		pthread_mutex_lock(&philo[i].last_eat_m);
		gettimeofday(&actual_time, NULL);
		printf("%li\n", (((actual_time.tv_sec * 1000) + (actual_time.tv_usec / 1000)) - ((philo[i].last_eat.tv_sec * 1000) + (philo[i].last_eat.tv_usec / 1000))));
		if (((actual_time.tv_sec * 1000) + (actual_time.tv_usec / 1000)) - ((philo[i].last_eat.tv_sec * 1000) + (philo[i].last_eat.tv_usec / 1000)) > philo->context->time_death)
		{
			pthread_mutex_lock(&philo->context->mutex_die);
			philo->context->die = 1;
			pthread_mutex_unlock(&philo->context->mutex_die);
			printf("philo %i is dead\n", i);
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

int	print(t_philo *philo, char *str)
{
	if (is_dead(philo))
		return (1);
	pthread_mutex_lock(&philo->context->print);
	printf("philosophers %i %s\n", philo->id, str);
	pthread_mutex_unlock(&philo->context->print);
	return (0);
}

int	eat(t_philo *philo, int n, int b)
{
	pthread_mutex_lock(&philo->context->fork[n].fork);
	if (is_dead(philo))
		return (1);
	pthread_mutex_lock(&philo->context->fork[b].fork);
	if (is_dead(philo))
		return (1);
	pthread_mutex_lock(&philo->last_eat_m);
	gettimeofday(&philo->last_eat, NULL);
	pthread_mutex_unlock(&philo->last_eat_m);
	print(philo, "eat");
	wait(philo, philo->context->time_eat);
	pthread_mutex_unlock(&philo->context->fork[n].fork);
	pthread_mutex_unlock(&philo->context->fork[b].fork);
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
	pthread_mutex_lock(&philo->context->init);
	pthread_mutex_unlock(&philo->context->init);
	while (1)
	{
		if (print(philo, "think"))
			break;
		if (philo->id % 2)
		{
			if (eat(philo, fork_l, fork_r))
				break;
		}
		else
			if (eat(philo, fork_r, fork_l))
				break;
		if (print(philo, "sleep"))
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

	gettimeofday(&context.time_start, NULL);
	memset(&context, 0, sizeof(t_context));
	context.philo_nb = ft_atoi(argv[1]);
	context.time_death = ft_atoi(argv[2]);
	context.time_eat = ft_atoi(argv[3]);
	context.time_sleep= ft_atoi(argv[4]);
	context.fork = malloc(context.philo_nb * sizeof(t_fork));
	philo = malloc(context.philo_nb * sizeof(t_philo));
	pthread_mutex_init(&context.init, NULL);
	pthread_mutex_init(&context.print, NULL);
	pthread_mutex_init(&context.mutex_die, NULL);
	pthread_mutex_lock(&context.init);
	i = -1;
	while (++i < context.philo_nb)
	{
		philo[i].context = &context;
		philo[i].id = i;
		pthread_mutex_init(&context.fork[i].fork, NULL);
		pthread_mutex_init(&philo[i].last_eat_m, NULL);
		pthread_create(&philo[i].thread, NULL, main_philo, &philo[i]);
	}
	pthread_create(&guard, NULL, main_guard, philo);
	pthread_mutex_unlock(&context.init);
	i = -1;
	while (++i < context.philo_nb)
		pthread_join(philo[i].thread, NULL);
	pthread_join(guard, NULL);
}