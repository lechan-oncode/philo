/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   message.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lechan <lechan@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 03:26:22 by lechan            #+#    #+#             */
/*   Updated: 2025/03/18 04:21:56 by lechan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosopher.h"

int	ft_ext_msg(char *s)
{
	int	i;

	i = 0;
	write(1, "Error \n", 7);
	while (s[i])
		i++;
	write(1, s, i);
	write(1, "\n", 1);
	return (1);
}

void	ft_msg(int id, long time, int n)
{
	if (id == FORK)
		printf("%lu\t%d has taken a fork\n", time, n + 1);
	else if (id == EAT)
		printf("%lu\t%d is eating\n", time, n + 1);
	else if (id == SLEEP)
		printf("%lu\t%d is sleeping\n", time, n + 1);
	else if (id == THINK)
		printf("%lu\t%d is thinking\n", time, n + 1);
	else if (id == DIED)
		printf("%lu\t%d died\n", time, n + 1);
	else if (id == END)
		printf("Simulation completed!\n");
}

void	ft_print_msg(int id, t_philo *philo)
{
	long	time;

	if (!philo->misc || philo->misc->die)
		return ;
		pthread_mutex_lock(&philo->misc->msg);
		time = now() - philo->misc->tt_start;
		ft_msg(id, time, philo->n);
		if (id != DIED)
			pthread_mutex_unlock(&philo->misc->msg);
}

int	ft_atoi(const char *str)
{
	int	i;
	int	neg;
	int	res;

	i = 0;
	neg = 1;
	res = 0;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
		{
			neg = -1;
		}
		i++;
	}
	while (str[i] >= '0' && str[i] <= '9' && str[i] != '\0')
	{
		res = (str[i] - '0') + (res * 10);
		i++;
	}
	return (res * neg);
}

/* Modified cleanup function to handle the new mutex */
int	ft_free(t_data *data, t_philo ***philo)
{
	int	i;

	i = 0;
	pthread_mutex_destroy(&data->msg);
	pthread_mutex_destroy(&data->death);
	pthread_mutex_destroy(&data->meal_check);
	while (i < data->n_philo)
	{
		pthread_mutex_destroy(&data->forks[i]);
		free((*philo)[i]);
		i++;
	}
	free(data->forks);
	free(data->thread);
	return (0);
}
