/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucmansa <lucmansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 14:33:50 by lucmansa          #+#    #+#             */
/*   Updated: 2025/04/09 16:52:12 by lucmansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

int	main(int argc, char **argv)
{
	t_context	context;
	t_philo		*philo;

	if (argc < 5 || argc > 6)
		return (write(1, "Error with arguments\n", 21), 1);
	context.die = 0;
	context.philo_nb = ft_atoi(argv[1]);
	context.time_death = ft_atoi(argv[2]);
	context.time_eat = ft_atoi(argv[3]);
	context.time_sleep = ft_atoi(argv[4]);
	context.nb_eat = 0;
	if (argc > 5)
		context.nb_eat = ft_atoi(argv[5]);
	if (context.philo_nb < 1 || context.time_death < 1 || context.time_eat < 1
		|| context.time_sleep < 1 || context.nb_eat < 0)
		return (write(1, "Error with arguments\n", 21), 1);
	context.fork = malloc(context.philo_nb * sizeof(pthread_mutex_t));
	philo = malloc(context.philo_nb * sizeof(t_philo));
	pthread_mutex_init(&context.print, NULL);
	pthread_mutex_init(&context.mutex_die, NULL);
	gettimeofday(&context.time_start, NULL);
	create_philos(philo, &context);
	free(philo);
	free(context.fork);
	return (0);
}
