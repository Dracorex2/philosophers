/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   manage_philo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucmansa <lucmansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 14:34:27 by lucmansa          #+#    #+#             */
/*   Updated: 2025/04/11 14:31:11 by lucmansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

int	wait(t_philo *philo, int i)
{
	struct timeval	start_time;

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
		return (pthread_mutex_unlock(&philo->context->fork[n]),
			pthread_mutex_unlock(&philo->context->fork[b]), 1);
	pthread_mutex_lock(&philo->eat_mutex);
	gettimeofday(&philo->last_eat, NULL);
	pthread_mutex_unlock(&philo->eat_mutex);
	if (print(philo, "is eating"))
		return (pthread_mutex_unlock(&philo->context->fork[n]),
			pthread_mutex_unlock(&philo->context->fork[b]), 1);
	wait(philo, philo->context->time_eat);
	check_eat(philo, 1);
	pthread_mutex_unlock(&philo->context->fork[b]);
	pthread_mutex_unlock(&philo->context->fork[n]);
	return (0);
}

void	create_philos(t_philo *philo, t_context *context)
{
	int			i;
	pthread_t	guard;

	i = -1;
	while (++i < context->philo_nb)
	{
		philo[i].context = context;
		philo[i].id = i;
		philo[i].eat_count = 0;
		philo[i].as_count = 0;
		if (philo[i].context->nb_eat > 0)
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
}

void	*main_philo(void *arg)
{
	t_philo	*philo;
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
			break ;
		if (philo->id % 2 && eat(philo, fork_l, fork_r))
			break ;
		else if (!(philo->id % 2) && eat(philo, fork_r, fork_l))
			break ;
		if (!print(philo, "is sleeping")
			&& wait(philo, philo->context->time_sleep))
			break ;
	}
	return (NULL);
}
