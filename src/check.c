/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lechan <lechan@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 03:30:32 by lechan            #+#    #+#             */
/*   Updated: 2025/03/18 03:34:33 by lechan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosopher.h"

long	now(void)
{
	struct timeval	now;

	gettimeofday(&now, NULL);
	return ((now.tv_sec * 1000) + (now.tv_usec / 1000));
}

int	ft_check_num(int ac, char **av)
{
	int	i;
	int	j;

	i = 1;
	while (i < ac)
	{
		j = 0;
		while (av[i][j] != '\0')
		{
			if (av[i][j] < '0' || av[i][j] > '9')
				return (1);
			j++;
		}
		i++;
	}
	return (0);
}

int	ft_check_done(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->n_philo)
	{
		pthread_mutex_lock(&data->death);
		if (data->philos[i]->next_meal < now())
		{
			ft_print_msg(DIED, data->philos[i]);
			data->die = 1;
			pthread_mutex_unlock(&data->death);
			return (1);
		}
		pthread_mutex_unlock(&data->death);
		if (data->max_meal != -1)
		{
			pthread_mutex_lock(&data->meal_check);
			if (data->philos[i]->eaten < data->max_meal)
				data->done = 0;
			pthread_mutex_unlock(&data->meal_check);
		}
		i++;
	}
	return (0);
}

void	*ft_monitor(void *arg)
{
	t_data	*data;

	data = (t_data *)arg;
	while (1)
	{
		data->done = 1;
		if (ft_check_done(data))
			return (NULL);
		if (data->max_meal != -1 && data->done)
		{
			pthread_mutex_lock(&data->death);
			data->die = 1;
			pthread_mutex_unlock(&data->death);
			printf("All philosophers have completed their meals\n");
			return (NULL);
		}
		usleep(1000);
	}
	return (NULL);
}

int	ft_mutex_death(t_philo *philo)
{
	t_data	*data;

	data = philo->misc;
	pthread_mutex_lock(&data->death);
	if (data->die)
	{
		pthread_mutex_unlock(&data->death);
		return (1);
	}
	pthread_mutex_unlock(&data->death);
	return (0);
}
