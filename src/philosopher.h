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

typedef struct s_data
{
    int             n_philo;
    int             tt_die;
    int             tt_eat;
    int             tt_sleep;
    int             max_meal;
    int             die;
    long            tt_start;
    pthread_mutex_t *forks;
    pthread_mutex_t msg;
    pthread_mutex_t death;
    // pthread_mutex_t stop;
    pthread_t       *thread;
}   t_data;

typedef struct s_philo
{
    int             n;
    int             l_hnd;
    int             r_hnd;
    int             death;
    int             eaten;
    long            next_meal;
    // pthread_mutex_t *eat;
    t_data          *misc;
}   t_philo;

int ft_ext_msg(char *s);
int ft_atoi(const char *str);
long now(void);
void ft_msg(int id, long time, int n);
void ft_print_msg(int id, t_philo *philo);
int ft_check_num(int ac, char **av);
int ft_parse(int ac, char **av, t_data *data);
int ft_init_mutex(t_data *data);
int ft_init_philo(t_data *data, t_philo ***philo);
void *ft_chk_death(void *arg);
void ft_fork(t_philo *philo);
void ft_eat(t_philo *philo);
void ft_sleep(t_philo *philo);
void *ft_dining(void *arg);
int ft_thread(t_data *data, t_philo ***philo);
int ft_free(t_data *data, t_philo ***philo);

#endif
