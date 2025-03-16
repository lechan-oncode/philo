/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 02:16:13 by lechan            #+#    #+#             */
/*   Updated: 2025/03/16 22:03:39 by bleow            ###   ########.fr       */
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
	// if (id == DIED)
	// 	fprintf(stderr, "DEBUG: Philo %d died at time %lu (next_meal: %lu, current: %lu)\n", 
	// 		   philo->n + 1, time, philo->next_meal, now());
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

/* Add this new function for centralized monitoring with starvation prevention */
void *ft_monitor(void *arg)
{
    t_data *data;
    int i, starving_philo;
    int done;
    long min_time_left;

    data = (t_data *)arg;
    while (1)
    {
        i = 0;
        done = 1;
        starving_philo = -1;
        min_time_left = 9999999;
        
        // Check if any philosopher has died or all have eaten enough
        while (i < data->n_philo)
        {
            pthread_mutex_lock(&data->death);
            
            // Track who is closest to starvation
            long time_left = data->philos[i]->next_meal - now();
            if (time_left < min_time_left) {
                min_time_left = time_left;
                starving_philo = i;
            }
            
            // Check if this philosopher has starved
            if (data->philos[i]->next_meal < now())
            {
                // fprintf(stderr, "DEBUG: Monitor detected Philo %d should die (next_meal: %lu, current: %lu)\n", 
                //        data->philos[i]->n + 1, data->philos[i]->next_meal, now());
                ft_print_msg(DIED, data->philos[i]);
                data->die = 1;
                pthread_mutex_unlock(&data->death);
                return (NULL);
            }
            pthread_mutex_unlock(&data->death);
            
            // Check if all philosophers have eaten enough
            if (data->max_meal != -1)
            {
                pthread_mutex_lock(&data->meal_check);
                if (data->philos[i]->eaten < data->max_meal)
                    done = 0;
                pthread_mutex_unlock(&data->meal_check);
            }
            i++;
        }
        
        // Exit if all philosophers have eaten enough
        if (data->max_meal != -1 && done)
        {
            pthread_mutex_lock(&data->death);
            data->die = 1;
            pthread_mutex_unlock(&data->death);
            fprintf(stderr, "DEBUG: All philosophers have completed their meals\n");
            return (NULL);
        }
        
        // Alert if a philosopher is close to starvation
        if (starving_philo != -1 && min_time_left < 200) {
            fprintf(stderr, "DEBUG: ALERT - Philo %d is close to starvation (%lu ms left)\n", 
                   data->philos[starving_philo]->n + 1, min_time_left);
        }
        
        // Sleep briefly to avoid excessive CPU usage
        usleep(1000);
    }
    return (NULL);
}

/* Modified ft_init_mutex function to initialize the new mutex */
int ft_init_mutex(t_data *data)
{
    int i;

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
int ft_init_philo(t_data *data, t_philo ***philo)
{
    int i;

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
			fprintf(stderr, "DEBUG: Death check - Philo %d should die (next_meal: %lu, current: %lu)\n", 
				   philo->n + 1, philo->next_meal, now());
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

/* Improved fork acquisition with death check to handle starvation better */
void ft_fork(t_philo *philo)
{
    t_data *data;
    long wait_start;

    data = philo->misc;

    // Special case for one philosopher
    if (data->n_philo == 1)
    {
        pthread_mutex_lock(&data->forks[philo->l_hnd]);
        ft_print_msg(FORK, philo);
        usleep(data->tt_die * 1000);
        pthread_mutex_unlock(&data->forks[philo->l_hnd]);
        return;
    }

    // Check if already dead before acquiring forks
    pthread_mutex_lock(&data->death);
    if (data->die)
    {
        pthread_mutex_unlock(&data->death);
        return;
    }
    pthread_mutex_unlock(&data->death);

    // Record when we started waiting
    wait_start = now();
    
    // Acquire forks in order based on index to prevent deadlock
    if (philo->l_hnd < philo->r_hnd) {
        pthread_mutex_lock(&data->forks[philo->l_hnd]);
        ft_print_msg(FORK, philo);
        
        // Check if we're approaching starvation while waiting for second fork
        while (pthread_mutex_trylock(&data->forks[philo->r_hnd]) != 0) {
            // Check if we're going to starve soon
            if (now() > philo->next_meal - 100) { // Leave 100ms margin
                fprintf(stderr, "DEBUG: Philo %d at risk of starvation (next_meal: %lu, current: %lu)\n", 
                    philo->n + 1, philo->next_meal, now());
                
                // If we've been waiting too long, release first fork and try again later
                if (now() - wait_start > data->tt_eat / 2) {
                    fprintf(stderr, "DEBUG: Philo %d releasing fork to avoid deadlock\n", philo->n + 1);
                    pthread_mutex_unlock(&data->forks[philo->l_hnd]);
                    usleep(1000); // Brief pause before retrying
                    return;
                }
            }
            
            // Check for death condition while waiting
            pthread_mutex_lock(&data->death);
            if (data->die)
            {
                pthread_mutex_unlock(&data->death);
                pthread_mutex_unlock(&data->forks[philo->l_hnd]);
                return;
            }
            pthread_mutex_unlock(&data->death);
            
            usleep(500); // Small wait to avoid CPU spinning
        }
        
        ft_print_msg(FORK, philo);
    } else {
        pthread_mutex_lock(&data->forks[philo->r_hnd]);
        ft_print_msg(FORK, philo);
        
        // Check if we're approaching starvation while waiting for second fork
        while (pthread_mutex_trylock(&data->forks[philo->l_hnd]) != 0) {
            // Check if we're going to starve soon
            if (now() > philo->next_meal - 100) { // Leave 100ms margin
                fprintf(stderr, "DEBUG: Philo %d at risk of starvation (next_meal: %lu, current: %lu)\n", 
                    philo->n + 1, philo->next_meal, now());
                
                // If we've been waiting too long, release first fork and try again later
                if (now() - wait_start > data->tt_eat / 2) {
                    fprintf(stderr, "DEBUG: Philo %d releasing fork to avoid deadlock\n", philo->n + 1);
                    pthread_mutex_unlock(&data->forks[philo->r_hnd]);
                    usleep(1000); // Brief pause before retrying
                    return;
                }
            }
            
            // Check for death condition while waiting
            pthread_mutex_lock(&data->death);
            if (data->die)
            {
                pthread_mutex_unlock(&data->death);
                pthread_mutex_unlock(&data->forks[philo->r_hnd]);
                return;
            }
            pthread_mutex_unlock(&data->death);
            
            usleep(500); // Small wait to avoid CPU spinning
        }
        
        ft_print_msg(FORK, philo);
    }
}

/* Improved eating with proper death checking */
void ft_eat(t_philo *philo)
{
    t_data *data;

    data = philo->misc;
    
    // Check if dead before eating
    pthread_mutex_lock(&data->death);
    if (data->die)
    {
        pthread_mutex_unlock(&data->death);
        pthread_mutex_unlock(&data->forks[philo->l_hnd]);
        pthread_mutex_unlock(&data->forks[philo->r_hnd]);
        return;
    }
    pthread_mutex_unlock(&data->death);
    
    // Proceed with eating
    ft_print_msg(EAT, philo);
    
    // Update meal time atomically
    pthread_mutex_lock(&data->death);
    philo->last_meal = now();
    philo->next_meal = philo->last_meal + data->tt_die;
    fprintf(stderr, "DEBUG: Philo %d eating - last_meal: %lu, next_meal: %lu\n", 
           philo->n + 1, philo->last_meal, philo->next_meal);
    pthread_mutex_unlock(&data->death);
    
    // Increment meal count atomically
    pthread_mutex_lock(&data->meal_check);
    philo->eaten++;
    pthread_mutex_unlock(&data->meal_check);
    
    // Sleep for eating duration
    usleep(data->tt_eat * 1000);
    
    // Release forks
    pthread_mutex_unlock(&data->forks[philo->l_hnd]);
    pthread_mutex_unlock(&data->forks[philo->r_hnd]);
}

/* Improved sleeping with death check */
void ft_sleep(t_philo *philo)
{
    t_data *data;

    data = philo->misc;
    
    // Check if dead before sleeping
    pthread_mutex_lock(&data->death);
    if (data->die)
    {
        pthread_mutex_unlock(&data->death);
        return;
    }
    pthread_mutex_unlock(&data->death);
    
    ft_print_msg(SLEEP, philo);
    usleep(data->tt_sleep * 1000);
}

/* Completely revamped dining function with proper synchronization and starvation avoidance */
void *ft_dining(void *arg)
{
    t_philo *philo;
    t_data *data;

    philo = (t_philo *)arg;
    data = philo->misc;
    
    // Even philosophers start slightly later to avoid initial deadlocks
    if (philo->n % 2 == 0)
        usleep(data->tt_eat / 2 * 1000);
    
    fprintf(stderr, "DEBUG: Philo %d starting with tt_die: %d\n", philo->n + 1, philo->misc->tt_die);
    
    // Main dining loop
    while (1)
    {
        // Check time left until starvation
        long time_left = philo->next_meal - now();
        fprintf(stderr, "DEBUG: Philo %d has %lu ms until starvation\n", philo->n + 1, time_left);
        
        // Check if simulation should end
        pthread_mutex_lock(&data->death);
        if (data->die)
        {
            pthread_mutex_unlock(&data->death);
            fprintf(stderr, "DEBUG: Philo %d exiting - death detected\n", philo->n + 1);
            break;
        }
        pthread_mutex_unlock(&data->death);
        
        // Try to get forks and eat
        ft_fork(philo);
        
        // Check again if we should continue
        pthread_mutex_lock(&data->death);
        if (data->die)
        {
            pthread_mutex_unlock(&data->death);
            break;
        }
        pthread_mutex_unlock(&data->death);
        
        ft_eat(philo);
        
        // Check if we've eaten enough
        pthread_mutex_lock(&data->meal_check);
        if (data->max_meal != -1 && philo->eaten >= data->max_meal)
        {
            fprintf(stderr, "DEBUG: Philo %d finished all meals (%d)\n", philo->n + 1, philo->eaten);
            pthread_mutex_unlock(&data->meal_check);
            break;
        }
        pthread_mutex_unlock(&data->meal_check);
        
        // Sleep and think
        ft_sleep(philo);
        
        // Check if dead after sleeping
        pthread_mutex_lock(&data->death);
        if (data->die)
        {
            pthread_mutex_unlock(&data->death);
            break;
        }
        pthread_mutex_unlock(&data->death);
        
        ft_print_msg(THINK, philo);
        
        // Adaptive thinking time - if we're close to starvation, think less
        time_left = philo->next_meal - now();
        if (time_left < data->tt_die / 2)
        {
            // Reduce thinking time if we're getting close to starvation
            fprintf(stderr, "DEBUG: Philo %d reducing thinking time (only %lu ms until starvation)\n", 
                   philo->n + 1, time_left);
            usleep(5 * 1000); // Very short thinking time when hungry
        }
    }
    
    return (NULL);
}

/* Modified thread management function to use the monitor thread */
int ft_thread(t_data *data, t_philo ***philo)
{
    int i;

    i = 0;
    data->thread = malloc(sizeof(pthread_t) * data->n_philo);
    if (!data->thread)
        return (1);
    
    fprintf(stderr, "DEBUG: Starting %d philosopher threads\n", data->n_philo);
    
    // Create the monitor thread first
    if (pthread_create(&data->monitor, NULL, ft_monitor, data))
        return (1);
    
    // Create philosopher threads
    while (i < data->n_philo)
    {
        if (pthread_create(&data->thread[i], NULL, ft_dining, (*philo)[i]))
            return (1);
        i++;
    }
    
    // Join threads
    i = 0;
    while (i < data->n_philo)
    {
        pthread_join(data->thread[i], NULL);
        fprintf(stderr, "DEBUG: Thread for philo %d has terminated\n", i + 1);
        i++;
    }
    
    // Join monitor thread
    pthread_join(data->monitor, NULL);
    
    return (0);
}

/* Modified cleanup function to handle the new mutex */
int ft_free(t_data *data, t_philo ***philo)
{
    int i;

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

/* Main function remains mostly the same */
int main(int ac, char **av)
{
    t_data data;
    t_philo **philo;

    if (ac < 5 || ac > 6)
        return (ft_ext_msg("wrong number of arguments"));
        
    data = (t_data){0};
    if (ft_parse(ac, av, &data))
        return (0);
    
    // fprintf(stderr, "DEBUG: Parsed arguments - philosophers: %d, tt_die: %d, tt_eat: %d, tt_sleep: %d, max_meal: %d\n",
    //     data.n_philo, data.tt_die, data.tt_eat, data.tt_sleep, data.max_meal);
    
    philo = malloc(sizeof(t_philo *) * data.n_philo);
    if (!philo)
        return (1);
    
    if (ft_init_mutex(&data) || ft_init_philo(&data, &philo))
    {
        free(philo);
        return (0);
    }
    
    if (ft_thread(&data, &philo))
    {
        ft_free(&data, &philo);
        free(philo);
        return (0);
    }
    
    ft_free(&data, &philo);
    free(philo);
    return (0);
}
