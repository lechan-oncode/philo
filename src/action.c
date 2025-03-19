/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   action.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lechan <lechan@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 03:28:06 by lechan            #+#    #+#             */
/*   Updated: 2025/03/18 03:45:45 by lechan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosopher.h"

void	ft_fork(t_philo *philo)
{
	t_data	*data;

	data = philo->misc;
	if (data->n_philo == 1)
	{
		pthread_mutex_lock(&data->forks[philo->l_hnd]);
		ft_msg(1, 0, 0);
		usleep(data->tt_die * 1000);
		pthread_mutex_unlock(&data->forks[philo->l_hnd]);
		return ;
	}
	pthread_mutex_lock(&data->forks[philo->l_hnd]);
	ft_print_msg(FORK, philo);
	pthread_mutex_lock(&data->forks[philo->r_hnd]);
	ft_print_msg(FORK, philo);
}

/* Improved eating with proper death checking */
void	ft_eat(t_philo *philo)
{
	t_data	*data;

	data = philo->misc;
	pthread_mutex_lock(&data->death);
	if (data->die)
	{
		pthread_mutex_unlock(&data->death);
		pthread_mutex_unlock(&data->forks[philo->l_hnd]);
		pthread_mutex_unlock(&data->forks[philo->r_hnd]);
		return ;
	}
	pthread_mutex_unlock(&data->death);
	ft_print_msg(EAT, philo);
	pthread_mutex_lock(&data->death);
	philo->last_meal = now();
	philo->next_meal = philo->last_meal + data->tt_die;
	pthread_mutex_unlock(&data->death);
	pthread_mutex_lock(&data->meal_check);
	philo->eaten++;
	pthread_mutex_unlock(&data->meal_check);
	usleep(data->tt_eat * 1000);
	pthread_mutex_unlock(&data->forks[philo->l_hnd]);
	pthread_mutex_unlock(&data->forks[philo->r_hnd]);
}

/* Improved sleeping with death check */
void	ft_sleep(t_philo *philo)
{
	t_data	*data;

	data = philo->misc;
	pthread_mutex_lock(&data->death);
	if (data->die)
	{
		pthread_mutex_unlock(&data->death);
		return ;
	}
	pthread_mutex_unlock(&data->death);
	ft_print_msg(SLEEP, philo);
	if (data->tt_die < data->tt_sleep)
		usleep(data->tt_die * 1000);
	else
		usleep(data->tt_sleep * 1000);
}

void	*ft_dining(void *arg)
{
	t_philo	*philo;
	t_data	*data;

	philo = (t_philo *)arg;
	data = philo->misc;
	if (philo->n % 2 == 0)
		usleep(data->tt_eat / 2 * 1000);
	while (!ft_mutex_death(philo))
	{
		ft_fork(philo);
		ft_eat(philo);
		pthread_mutex_lock(&data->meal_check);
		if (data->max_meal != -1 && philo->eaten >= data->max_meal)
		{
			pthread_mutex_unlock(&data->meal_check);
			break ;
		}
		pthread_mutex_unlock(&data->meal_check);
		ft_sleep(philo);
		ft_print_msg(THINK, philo);
	}
	return (NULL);
}
