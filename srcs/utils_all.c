/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_all.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucmansa <lucmansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 14:38:08 by lucmansa          #+#    #+#             */
/*   Updated: 2025/04/08 15:18:25 by lucmansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

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

int	get_time(struct	timeval time)
{
	struct timeval	actual_time;

	gettimeofday(&actual_time, NULL);
	actual_time.tv_sec -= time.tv_sec;
	actual_time.tv_usec -= time.tv_usec;
	return (actual_time.tv_sec * 1000 + actual_time.tv_usec / 1000);
}
