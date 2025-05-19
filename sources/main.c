/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekocak <ekocak@student.42.org.tr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/19 17:42:00 by ekocak            #+#    #+#             */
/*   Updated: 2025/05/19 17:42:00 by ekocak           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"


/**
 * @brief Destroys all previously initialized mutexes in the simulation.
 *
 * This function is responsible for properly releasing system resources 
 * allocated for mutexes during the simulation. It ensures that:
 * - The write lock (used for synchronized output) is destroyed.
 * - The simulation stop lock is destroyed.
 * - Each fork lock and each philosopher's meal time lock is destroyed.
 *
 * This function should be called after the simulation has ended to avoid
 * resource leaks and ensure a clean shutdown.
 *
 * @param table A pointer to the simulation table structure containing all mutexes.
 */
static void    destroyMutex(t_table *table)
{
    int i;

    pthread_mutex_destroy(&table->write_lock);
    pthread_mutex_destroy(&table->sim_stop_lock);
    i = -1;
    while (++ i < table->num_philos)
    {
        pthread_mutex_destroy(&table->fork_locks[i]);
        pthread_mutex_destroy(&table->philos[i]->meal_time_lock);
    }
}


/**
 * @brief Initializes all necessary mutexes for the simulation.
 *
 * This function sets up mutexes required for thread-safe operations:
 * - A write lock to synchronize console output among philosophers.
 * - A simulation stop lock to safely control and check simulation termination.
 * - A mutex for each fork to ensure mutual exclusion on fork access.
 * - A mutex for each philosopher's `last_meal` access to avoid race conditions
 *   when reading/writing the meal time across threads.
 *
 * If any mutex fails to initialize, the function returns `false` to indicate failure.
 * Proper cleanup should be handled by the caller if this function fails part-way.
 *
 * @param table A pointer to the simulation table structure containing shared state.
 * @return `true` if all mutexes were successfully initialized, `false` otherwise.
 */
static bool    initializeMutex(t_table *table)
{
    int i;

    if (pthread_mutex_init(&table->write_lock, NULL) != 0)
        return false;
    if (pthread_mutex_init(&table->sim_stop_lock, NULL) != 0)
        return false;
    i = -1;
    while (++ i < table->num_philos)
    {
        if (pthread_mutex_init(&table->fork_locks[i], NULL) != 0)
            return false;
        if (pthread_mutex_init(&table->philos[i]->meal_time_lock, NULL) != 0)
            return false;
    }
    return true;
}


/**
 * @brief Starts the philosopher simulation.
 *
 * This function initializes the simulation start time with a small offset 
 * based on the number of philosophers to reduce immediate thread contention.
 * It performs the following steps:
 * 
 * - Initializes all required mutexes.
 * - Sets the `last_meal` time for each philosopher.
 * - Creates a thread for each philosopher to execute their routine.
 * - If there is more than one philosopher, a monitor thread is also created
 *   to check for starvation or completion conditions.
 *
 * If any thread fails to be created or mutex initialization fails, 
 * the function returns `false` indicating the simulation could not be started.
 *
 * @param table A pointer to the main simulation structure containing configuration and state.
 * @return `true` if the simulation threads were successfully started, `false` otherwise.
 */
static bool    startSimulator(t_table *table)
{
    int i;

    table->start_time = getTimeIn_ms() + 20 * table->num_philos;

    if (!initializeMutex(table))
        return false;

    i = -1;
    while (++i < table->num_philos)
    {
        table->philos[i]->last_meal = table->start_time;
        if (pthread_create(&table->philos[i]->thread, NULL, &philosopherRoutine, (void *)table->philos[i]) != 0)
            return false;
    }
    if (table->num_philos > 1)
    {
        if (pthread_create(&table->monitor, NULL, &monitor, (void *)table) != 0)
            return false;
    }

    return true;
}

/**
 * @brief Stops the philosopher simulation by joining all threads and cleaning up.
 *
 * This function waits for all philosopher threads and the monitor thread
 * to finish execution using `pthread_join`. Once all threads are properly
 * joined, it destroys all mutexes used in the simulation to prevent memory leaks
 * and undefined behavior.
 *
 * It ensures a clean and synchronized shutdown of the simulation.
 *
 * @param table A pointer to the main simulation structure containing thread data.
 */
static void    stopSimulator(t_table *table)
{
    int i;
    
    i = -1;
    while (++ i < table->num_philos)
        pthread_join(table->philos[i]->thread, NULL);
    pthread_join(table->monitor, NULL);
    destroyMutex(table);
}


/**
 * @brief The main function for the dining philosophers simulation.
 *
 * This function serves as the entry point of the program. It:
 * - Validates the number and format of command-line arguments
 * - Initializes the simulation table structure (`t_table`)
 * - Starts the simulation, which spawns philosopher and monitor threads
 * - Properly stops and frees all allocated resources when simulation ends
 * 
 * The simulator expects 4 or 5 arguments (number of philosophers, time to die,
 * time to eat, time to sleep, and optional minimum number of meals).
 *
 * @param ac The argument count.
 * @param av The argument vector (program arguments).
 * 
 * @return Returns EXIT_SUCCESS (0) if the simulation completes successfully,
 * or EXIT_FAILURE (1) on any failure or invalid input.
 */
int main(int ac, char **av)
{
    t_table *table;

    if (ac < 5 || ac > 6)
        return msg(ERR_USAGE, EXIT_FAILURE);
    table = NULL;
    if (!isValid(ac, av))
        return EXIT_FAILURE;
    table = initTable(ac, av);
    if (table == NULL)
        return EXIT_FAILURE;
    if (!startSimulator(table))
    {
        freeTable(table);
        return EXIT_FAILURE;
    }
    stopSimulator(table);
    freeTable(table);

    return EXIT_SUCCESS;
}