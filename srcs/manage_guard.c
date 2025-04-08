/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   manage_guard.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucmansa <lucmansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 14:37:11 by lucmansa          #+#    #+#             */
/*   Updated: 2025/04/08 15:33:18 by lucmansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

void	print_die(t_philo *philo)
{
	int	timestamp;

	pthread_mutex_lock(&philo->context->print);
	pthread_mutex_lock(&philo->context->mutex_die);
	philo->context->die = 1;
	timestamp = get_time(philo->context->time_start);
	printf("%i %i died\n", timestamp, philo->id);
	pthread_mutex_unlock(&philo->context->mutex_die);
	pthread_mutex_unlock(&philo->context->print);
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

int	is_dead(t_philo *philo)
{
	int	dead;

	dead = 0;
	pthread_mutex_lock(&philo->context->mutex_die);
	dead = philo->context->die;
	pthread_mutex_unlock(&philo->context->mutex_die);
	return (dead);
}

void	*main_guard(void *arg)
{
	int				i;
	struct timeval	actual_time;
	t_philo			*philos;

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
			print_die(&philos[i]);
		pthread_mutex_unlock(&philos[i].eat_mutex);
		usleep(1);
	}
	return (NULL);
}
