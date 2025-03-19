/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lechan <lechan@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 21:51:03 by lechan            #+#    #+#             */
/*   Updated: 2025/03/18 03:41:14 by lechan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILOSOPHER_H
# define PHILOSOPHER_H

# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
# include <sys/time.h>

# define FORK 1
# define EAT 2
# define SLEEP 3
# define THINK 4
# define DIED 5
# define END 6

typedef struct s_philo	t_philo;

typedef struct s_data
{
	int				n_philo;
	int				tt_die;
	int				tt_eat;
	int				tt_sleep;
	int				max_meal;
	int				done;
	int				die;
	int				meals_complete;
	long			tt_start;
	pthread_mutex_t	*forks;
	pthread_mutex_t	msg;
	pthread_mutex_t	death;
	pthread_mutex_t	meal_check;
	pthread_t		*thread;
	pthread_t		monitor;
	t_philo			**philos;
}	t_data;

typedef struct s_philo
{
	int		n;
	int		l_hnd;
	int		r_hnd;
	int		death;
	int		eaten;
	long	next_meal;
	long	last_meal;
	t_data	*misc;
}	t_philo;

int		ft_ext_msg(char *msg);
int		ft_atoi(const char *str);
long	now(void);
void	ft_msg(int id, long time, int n);
void	ft_print_msg(int type, t_philo *philo);
int		ft_check_num(int ac, char **av);
int		ft_parse(int ac, char **av, t_data *data);
int		ft_check_done(t_data *data);
void	*ft_monitor(void *arg);
int		ft_init_mutex(t_data *data);
int		ft_init_philo(t_data *data, t_philo ***philo);
void	ft_fork(t_philo *philo);
void	ft_eat(t_philo *philo);
void	ft_sleep(t_philo *philo);
int		ft_mutex_death(t_philo *philo);
void	*ft_dining(void *arg);
int		ft_thread(t_data *data, t_philo ***philo);
int		ft_free(t_data *data, t_philo ***philo);

#endif
