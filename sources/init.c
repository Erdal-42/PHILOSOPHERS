#include "philo.h"

/**
 * @brief Allocates and initializes philosopher structures for the simulation.
 * 
 * Allocates an array of philosopher pointers and then allocates each philosopher struct.
 * Sets philosopher ID, forks they use, times eaten, and a pointer to the shared table.
 * Returns NULL if any allocation fails.
 *
 * @param table Pointer to the simulation table containing configuration.
 * @return Pointer to array of initialized philosopher pointers, or NULL on failure.
 */
static t_philo **initPhilos(t_table *table)
{
    int i;
    t_philo    **philos;

    philos = malloc(sizeof(t_philo *) * table->num_philos);
    if (!philos)
    {
        return NULL;
    }
    i = -1;
    while (++i <  table->num_philos)
    {
        philos[i] = malloc(sizeof(t_philo));
        if (!philos[i])
            return NULL; 
        philos[i]->id = i + 1;
        philos[i]->fork[0] = i;
        philos[i]->fork[1] = (i + 1) % table->num_philos;
        philos[i]->times_ate = 0;
        philos[i]->table = table;
    }
    return philos;
}


/**
 * @brief Allocates and initializes the simulation table with parameters.
 * 
 * Parses command line arguments to set simulation settings,
 * allocates mutex array for forks, initializes philosophers,
 * and sets simulation stop flag to false.
 * Frees allocated memory and returns NULL on failure.
 * 
 * @param ac Argument count.
 * @param av Argument vector.
 * @return Pointer to initialized t_table struct, or NULL on failure.
 */
t_table *initTable(int ac, char **av)
{
    t_table *table;

    table = malloc(sizeof(t_table));
    if (!table)
        return NULL;
    table->num_philos = atoi(av[1]);
    table->time_to_die = atoi(av[2]);
    table->time_to_eat = atoi(av[3]);
    table->time_to_sleep = atoi(av[4]);
    if (ac == 6)
        table->min_dining = atoi(av[5]);
    else 
        table->min_dining = -1;
    table->fork_locks = malloc(sizeof(pthread_mutex_t) * table->num_philos);
    if (!table->fork_locks)
    {
        return freeTableExit(table);
    }
    table->philos = initPhilos(table);
    if (table->philos == NULL)
    {
        return freeTableExit(table);
    }
    table->sim_stop = false;
    
    return table;
}