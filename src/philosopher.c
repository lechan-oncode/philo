/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lechan <lechan@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 02:16:13 by lechan            #+#    #+#             */
/*   Updated: 2025/03/18 04:18:22 by lechan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosopher.h"

int	ft_parse(int ac, char **av, t_data *data)
{
	if (ac > 6)
		return (ft_ext_msg("too many arg"));
	if (ft_check_num(ac, av))
		return (ft_ext_msg("digit only"));
	if (!av[1] || ft_atoi(av[1]) < 1)
		return (ft_ext_msg("number of philosopher"));
	if (!av[2] || ft_atoi(av[2]) < 60)
		return (ft_ext_msg("time to die"));
	if (!av[3] || ft_atoi(av[3]) < 60)
		return (ft_ext_msg("time to eat"));
	if (!av[4] || ft_atoi(av[4]) < 60)
		return (ft_ext_msg("time to sleep"));
	if (av[5] && ft_atoi(av[5]) < 1)
		return (ft_ext_msg("number to eat"));
	data->n_philo = ft_atoi(av[1]);
	data->tt_die = ft_atoi(av[2]);
	data->tt_eat = ft_atoi(av[3]);
	data->tt_sleep = ft_atoi(av[4]);
	data->max_meal = -1;
	if (av[5] != NULL)
		data->max_meal = ft_atoi(av[5]);
	return (0);
}

/* Modified ft_init_mutex function to initialize the new mutex */
int	ft_init_mutex(t_data *data)
{
	int	i;

	i = 0;
	data->tt_start = now();
	pthread_mutex_init(&data->msg, NULL);
	pthread_mutex_init(&data->death, NULL);
	pthread_mutex_init(&data->meal_check, NULL);
	data->forks = malloc(sizeof(pthread_mutex_t) * data->n_philo);
	if (!data->forks)
		return (1);
	while (i < data->n_philo)
		pthread_mutex_init(&data->forks[i++], NULL);
	return (0);
}

/* Modified ft_init_philo function to track last meal time */
int	ft_init_philo(t_data *data, t_philo ***philo)
{
	int	i;

	i = -1;
	data->philos = *philo;
	while (++i < data->n_philo)
	{
		(*philo)[i] = malloc(sizeof(t_philo));
		if (!(*philo)[i])
			return (1);
		(*philo)[i]->n = i;
		(*philo)[i]->l_hnd = i;
		(*philo)[i]->r_hnd = (i + 1) % data->n_philo;
		(*philo)[i]->death = 0;
		(*philo)[i]->eaten = 0;
		(*philo)[i]->misc = data;
		(*philo)[i]->last_meal = now();
		(*philo)[i]->next_meal = (*philo)[i]->last_meal + data->tt_die;
	}
	return (0);
}

/* Modified thread management function to use the monitor thread */
int	ft_thread(t_data *data, t_philo ***philo)
{
	int	i;

	i = 0;
	data->thread = malloc(sizeof(pthread_t) * data->n_philo);
	if (!data->thread)
		return (1);
	if (pthread_create(&data->monitor, NULL, ft_monitor, data))
		return (1);
	while (i < data->n_philo)
	{
		if (pthread_create(&data->thread[i], NULL, ft_dining, (*philo)[i]))
			return (1);
		i++;
	}
	i = 0;
	while (i < data->n_philo)
		pthread_join(data->thread[i++], NULL);
	pthread_join(data->monitor, NULL);
	return (0);
}

/* Main function remains mostly the same */
int	main(int ac, char **av)
{
	t_data	data;
	t_philo	**philo;

	if (ac < 5 || ac > 6)
		return (ft_ext_msg("wrong number of arg"));
	data = (t_data){0};
	if (ft_parse(ac, av, &data))
		return (0);
	philo = malloc(sizeof(t_philo *) * data.n_philo);
	if (!philo)
		return (1);
	if (ft_init_mutex(&data) || ft_init_philo(&data, &philo))
	{
		free(philo);
		return (0);
	}
	ft_thread(&data, &philo);
	ft_free(&data, &philo);
	free(philo);
	return (0);
}
