/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lechan <lechan@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 02:16:13 by lechan            #+#    #+#             */
/*   Updated: 2025/03/16 04:17:23 by lechan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosopher.h"

int ft_ext_msg(char *s)
{
	int i;

	i = 0;
	write(1, "Error \n", 7);
	while (s[i])
		i++;
	write(1, s, i);
	write(1, "\n", 1);
	return (1);
}

int ft_atoi(const char *str)
{
	int i;
	int neg;
	int res;

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

long now(void)
{
	struct timeval now;

	gettimeofday(&now, NULL);
	return ((now.tv_sec * 1000) + (now.tv_usec / 1000));
}

void ft_msg(int id, long time, int n)
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

void ft_print_msg(int id, t_philo *philo)
{
	long time;

	if (!philo->misc || philo->misc->die)
		return;
	pthread_mutex_lock(&philo->misc->msg);
	time = now() - philo->misc->tt_start;
	//printf("time: %li\n", philo->misc->tt_start);
	ft_msg(id, time, philo->n);
	if (id != DIED)
		pthread_mutex_unlock(&philo->misc->msg);
}

int ft_check_num(int ac, char **av)
{
	int i;
	int j;

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

int ft_parse(int ac, char **av, t_data *data)
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
	data->max_meal = (av[5]) ? ft_atoi(av[5]) : -1;
	return (0);
}

int ft_init_mutex(t_data *data)
{
	int i;

	i = 0;
	data->tt_start = now();
	pthread_mutex_init(&data->msg, NULL);
	pthread_mutex_init(&data->death, NULL);
	data->forks = malloc(sizeof(pthread_mutex_t) * data->n_philo);
	if (!data->forks)
		return (1);
	while (i < data->n_philo)
		pthread_mutex_init(&data->forks[i++], NULL);
	return (0);
}

int ft_init_philo(t_data *data, t_philo ***philo)
{
	int i;

	i = -1;
	// *philo = malloc(sizeof(t_philo *) * data->n_philo);
	// if (!*philo)
	//     return (1);
	while (++i < data->n_philo)
	{
		(*philo)[i] = malloc(sizeof(t_philo));
		(*philo)[i]->n = i;
		(*philo)[i]->l_hnd = i;
		(*philo)[i]->r_hnd = (i + 1) % data->n_philo;
		(*philo)[i]->death = 0;
		(*philo)[i]->eaten = 0;
		(*philo)[i]->misc = data;
		(*philo)[i]->next_meal = 0;
	}
	return (0);
}

int ft_chk_eat(t_philo *philo)
{
	t_data *data;

	data = philo->misc;
	if ((data->max_meal != -1) && (philo->eaten >= data->max_meal))
		return (0);
	return (1);
}

void *ft_chk_death(void *arg)
{
	t_philo *philo;
	t_data  *data;
	// int n = 0;
	philo = (t_philo *)arg;
	data = philo->misc;

	// while (data->die == 0)
	while (1)
	{
		pthread_mutex_lock(&data->death);
		if (philo->next_meal < now())
		{
			ft_print_msg(DIED, philo);
			// pthread_mutex_lock(&data->death);
			data->die = 1;
			pthread_mutex_unlock(&data->death);
			// ft_print_msg(DIED, philo);
			break;
		}

		// printf("test %d\n", n++);
		pthread_mutex_unlock(&data->death);
		usleep(1000);
	}
	return (NULL);
}

void ft_fork(t_philo *philo)
{
	t_data *data;

	data = philo->misc;

	if (data->n_philo == 1)
	{
		pthread_mutex_lock(&data->forks[philo->l_hnd]);
		ft_print_msg(FORK, philo);
		philo->next_meal = now() + data->tt_die;
		ft_print_msg(DIED, philo);
		data->die = 1;
		return;
	}

	// pthread_mutex_lock(&data->forks[philo->l_hnd]);
	// ft_print_msg(FORK, philo);
	// pthread_mutex_lock(&data->forks[philo->r_hnd]);
	// ft_print_msg(FORK, philo);

	if (philo->l_hnd < philo->r_hnd) {
		pthread_mutex_lock(&data->forks[philo->l_hnd]);
		ft_print_msg(FORK, philo);
		pthread_mutex_lock(&data->forks[philo->r_hnd]);
		ft_print_msg(FORK, philo);
	} else {
		pthread_mutex_lock(&data->forks[philo->r_hnd]);
		ft_print_msg(FORK, philo);
		pthread_mutex_lock(&data->forks[philo->l_hnd]);
		ft_print_msg(FORK, philo);
	}
}

void ft_eat(t_philo *philo)
{
	t_data *data;

	data = philo->misc;
	ft_print_msg(EAT, philo);
	philo->next_meal = now() + data->tt_die;
	philo->eaten++;
	if (data->tt_eat > data->tt_die)
		usleep(data->tt_die * 1000);
	else
		usleep(data->tt_eat * 1000);
	pthread_mutex_unlock(&data->forks[philo->l_hnd]);
	pthread_mutex_unlock(&data->forks[philo->r_hnd]);
}

void ft_sleep(t_philo *philo)
{
	t_data *data;

	data = philo->misc;
	ft_print_msg(SLEEP, philo);
	if ((data->tt_eat + data->tt_sleep) > data->tt_die)
		usleep(data->tt_die * 1000);
	else
		usleep(data->tt_sleep * 1000);
}

void *ft_dining(void *arg)
{
	t_philo *philo;
	pthread_t death;

	philo = (t_philo *)arg;
	philo->next_meal = now() + (long)philo->misc->tt_die;
	pthread_create(&death, NULL, ft_chk_death, philo);
	pthread_mutex_lock(&philo->misc->msg);
	pthread_mutex_unlock(&philo->misc->msg);
	// while (!philo->misc->die)
	// {
	while (1)
	{
		pthread_mutex_lock(&philo->misc->death);
		// philo->misc->n_philo % 2 == 0 ? usleep(100) : usleep(200);
		if (philo->misc->die)
		{
			pthread_mutex_unlock(&philo->misc->death);
			break ;
		}
		pthread_mutex_unlock(&philo->misc->death);
		ft_fork(philo);
		ft_eat(philo);
		if (!ft_chk_eat(philo))
			break;
		ft_sleep(philo);
		ft_print_msg(THINK, philo);
	}

	// pthread_mutex_unlock(&philo->misc->msg);
	pthread_join(death, NULL);
	return (NULL);
}

int ft_thread(t_data *data, t_philo ***philo)
{
	int i;

	i = 0;
	data->thread = malloc(sizeof(pthread_t) * data->n_philo);
	if (!data->thread)
		return (1);
	while (i < data->n_philo)
	{
		pthread_create(&data->thread[i], NULL, ft_dining, (*philo)[i]);
		i++;
	}
	i = 0;
	while (i < data->n_philo)
	{
		// printf("trigger %d\n", (*philo)[i]->n);
		pthread_join(data->thread[i], NULL);
		i++;
	}
	return (0);
}

int ft_free(t_data *data, t_philo ***philo)
{
	int i;

	i = 0;
	pthread_mutex_destroy(&data->msg);
	pthread_mutex_destroy(&data->death);
	while (i < data->n_philo)
	{
		// pthread_mutex_destroy((*philo)[i]->eat);
		pthread_mutex_destroy(&data->forks[i]);
		// free((*philo)[i]->eat);
		free((*philo)[i]);
		i++;
	}
	(void)philo;
	free(data->forks);
	free(data->thread);
	return (0);
}

int main(int ac, char **av)
{
	t_data data;
	t_philo **philo;

	data = (t_data){0};
	if (ft_parse(ac, av, &data))
		return (0);
	philo = malloc(sizeof(t_philo *) * data.n_philo);
	if (!philo)
		return (1);
	if (ft_init_mutex(&data) || ft_init_philo(&data, &philo))
		return (0);
	if (ft_thread(&data, &philo))
		return (0);
	ft_free(&data, &philo);
	free(philo);
	return (0);
}
