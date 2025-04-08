/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucmansa <lucmansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 15:13:09 by lucmansa          #+#    #+#             */
/*   Updated: 2025/04/08 15:31:43 by lucmansa         ###   ########.fr       */
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

typedef struct s_context
{
	int				philo_nb;
	pthread_mutex_t	*fork;
	int				time_death;
	int				time_eat;
	int				time_sleep;
	int				nb_eat;
	struct timeval	time_start;
	pthread_mutex_t	print;
	int				die;
	int				end;
	pthread_mutex_t	mutex_die;
}	t_context;

typedef struct s_philo
{
	t_context		*context;
	int				id;
	pthread_t		thread;
	pthread_mutex_t	eat_mutex;
	pthread_mutex_t	mutex_eat_count;
	struct timeval	last_eat;
	int				as_count;
	int				eat_count;
}	t_philo;

void	print_die(t_philo *philo);
int		check_eat(t_philo *philo, int i);
int		check_all_eat(t_philo *philo);
int		is_dead(t_philo *philo);
void	*main_guard(void *arg);

int		wait(t_philo *philo, int i);
int		eat(t_philo *philo, int n, int b);
void	create_philos(t_philo *philo, t_context *context);
void	*main_philo(void *arg);

int		ft_atoi(const char *nptr);
int		print(t_philo *philo, char *str);
int		get_time(struct	timeval time);
#endif