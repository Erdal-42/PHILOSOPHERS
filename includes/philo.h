
#ifndef PHILO_H
#define PHILO_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>

#define ERR_USAGE "Usage: <number_of_philosophers(int)> <time_to_die(int)> <time_to_eat(int)> <time_to_sleep(int)> [(optional)number_of_times_each_philosopher_must_eat(int)]"

typedef struct s_philo t_philo;

typedef struct s_table
{
    int     num_philos;
    time_t  start_time;
    int     time_to_die;
    int     time_to_eat;
    int     time_to_sleep;
    pthread_t monitor;
    pthread_mutex_t *fork_locks;
    pthread_mutex_t write_lock;
    pthread_mutex_t sim_stop_lock;
    bool    sim_stop;
    int     min_dining;
    t_philo **philos;
} t_table;

typedef struct s_philo
{
    int         id;
    pthread_t   thread;
    int         fork[2];
    int         times_ate;
    time_t      last_meal;
    pthread_mutex_t meal_time_lock;
    t_table     *table;
} t_philo;

typedef enum e_status
{
    GOT_RIGHT_FORK,     //0
    GOT_LEFT_FORK,      //1
    EATING,             //2
    SLEEPING,           //3
    THINKING,           //4
    DIED                //5
} STATUS;

int     msg(char *, int);
bool    isValid(int, char **);
t_table *initTable(int, char **);
void    *freeTableExit(t_table *);
void    freeTable(t_table *);
time_t  getTimeIn_ms(void);
bool    hasAnyoneDied(t_table *);
void    lullPhilo(t_philo *, int);
void    simStartDelay(time_t);
void    *philosopherRoutine(void *);
void    writeStatus(t_philo *, STATUS);
void    *monitor(void *);
bool    hasPhiloDied(t_philo *);
bool    hasSimStopped(t_table *table);


#endif