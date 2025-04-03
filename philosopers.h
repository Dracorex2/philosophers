/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopers.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucmansa <lucmansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/27 17:15:36 by lucmansa          #+#    #+#             */
/*   Updated: 2025/04/01 17:24:44 by lucmansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILOSOPHERS_H
# define PHILOSOPHERS_H

# include <pthread.h>
# include <string.h>
# include <stdio.h>
# include <unistd.h>
# include <stdlib.h>
# include <sys/time.h>

typedef struct s_fork
{
	pthread_mutex_t	fork;
}	t_fork;

typedef struct s_context
{
	int				philo_nb;
	t_fork			*fork;
	int				time_death;
	int				time_eat;
	int				time_sleep;
	struct timeval	time_start;
	pthread_mutex_t	print;
	pthread_mutex_t	init;
	int				die;
	pthread_mutex_t	mutex_die;
}	t_context;

typedef struct s_philo
{
	t_context		*context;
	int				id;
	pthread_t		thread;
	struct timeval	last_eat;
	pthread_mutex_t	last_eat_m;
	int				think;
}	t_philo;

#endif