/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_all.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucmansa <lucmansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 14:38:08 by lucmansa          #+#    #+#             */
/*   Updated: 2025/04/11 14:48:32 by lucmansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

int	ft_isdigit(char *nb)
{
	int	i;

	i = -1;
	while (nb[++i])
	{
		if (!(nb[i] >= '0' && nb[i] <= '9'))
			return (1);
	}
	return (0);
}

int	ft_atoi(char *nptr)
{
	int	nbr;
	int	i;

	nbr = 0;
	i = 0;
	if (ft_isdigit(nptr))
		return (0);
	while (nptr[i] >= '0' && nptr[i] <= '9')
		nbr = nbr * 10 + (nptr[i++] - '0');
	return (nbr);
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
	actual_time.tv_sec *= 1000;
	actual_time.tv_usec /= 1000;
	time.tv_sec *= 1000;
	time.tv_usec /= 1000;
	return ((actual_time.tv_sec + actual_time.tv_usec)
		- (time.tv_sec + time.tv_usec));
}
