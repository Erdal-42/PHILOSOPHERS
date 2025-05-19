#include "philo.h"


/**
 * @brief Report the death of a philosopher and stop the simulation.
 *
 * Locks the sim_stop mutex, sets sim_stop flag if not already set,
 * and prints the death status of the philosopher.
 *
 * @param philo Pointer to the philosopher who died.
 */
static void    reportDeath(t_philo *philo)
{
    pthread_mutex_lock(&philo->table->sim_stop_lock);
    if (philo->table->sim_stop == false)
    {
        philo->table->sim_stop = true;
        writeStatus(philo, DIED);
    }
    pthread_mutex_unlock(&philo->table->sim_stop_lock);
}


/**
 * @brief Check if a philosopher has died due to starvation.
 *
 * Calculates elapsed time since the philosopher's last meal.
 * If it exceeds time_to_die, reports death and returns true.
 * Otherwise, returns false.
 *
 * @param philo Pointer to philosopher.
 * @return true if philosopher has died, false otherwise.
 */
bool hasPhiloDied(t_philo *philo)
{
    int elapsed_time;

    pthread_mutex_lock(&philo->meal_time_lock);
    elapsed_time = getTimeIn_ms() - philo->last_meal;
    pthread_mutex_unlock(&philo->meal_time_lock);

    if (elapsed_time >= philo->table->time_to_die)
    {
        reportDeath(philo);
        return true;
    }
    return false;
}


/**
 * @brief Check if any philosopher has died or if the simulation should stop.
 *
 * First checks the global stop flag. If set, returns true immediately.
 * Then iterates over all philosophers to check their individual status.
 * Finally, returns the updated stop status.
 *
 * @param table Pointer to simulation table.
 * @return true if simulation should stop due to death, false otherwise.
 */
bool    hasAnyoneDied(t_table *table)
{
    bool    status;
    int i;

    pthread_mutex_lock(&table->sim_stop_lock);
    status = table->sim_stop;
    pthread_mutex_unlock(&table->sim_stop_lock);
    if (status)
        return status; 
    i = -1;
    while (++ i < table->num_philos)
    {
        hasPhiloDied(table->philos[i]);
    }
    pthread_mutex_lock(&table->sim_stop_lock);
    status = table->sim_stop;
    pthread_mutex_unlock(&table->sim_stop_lock);
    return status;
}


/**
 * @brief Check if the simulation has been signaled to stop.
 *
 * Locks the sim_stop_lock mutex to safely read the sim_stop flag.
 * Returns the current stop status.
 *
 * @param table Pointer to simulation table.
 * @return true if simulation should stop, false otherwise.
 */
bool    hasSimStopped(t_table *table)
{
    bool  status;

    pthread_mutex_lock(&table->sim_stop_lock);
    status = table->sim_stop;
    pthread_mutex_unlock(&table->sim_stop_lock);

    return status;
}


/**
 * @brief Check if all philosophers have completed required meals.
 * 
 * Iterates through all philosophers and locks their meal_time_lock to
 * safely check if times_ate meets or exceeds min_dining.
 * Returns true only if all have eaten enough.
 * 
 * @param table Pointer to simulation table.
 * @return true if all philosophers completed meals, false otherwise.
 */
static bool areMealsCompleted(t_table *table)
{
    int i;
    bool completed;
    
    completed = true;
    //if there is a value for min_dining, check all philosophers to see if they all ate.
    i = -1;
    while (++ i < table->num_philos)
    {
        pthread_mutex_lock(&table->philos[i]->meal_time_lock);
        if (table->philos[i]->times_ate < table->min_dining)
        {
            pthread_mutex_unlock(&table->philos[i]->meal_time_lock);
            completed = false;
            break;
        }
        pthread_mutex_unlock(&table->philos[i]->meal_time_lock);
    }
    return completed;
}


/**
 * @brief Monitor thread to check philosophers' status.
 * 
 * Waits for simulation start time, then continuously checks:
 * - if any philosopher died,
 * - if simulation should stop,
 * - if minimum meals completed (then stops simulation).
 * Exits when simulation ends.
 * 
 * @param data Pointer to simulation table.
 * @return Always returns NULL.
 */
void *monitor(void *data)
{
    t_table *table;

    table = (t_table *)data;

    simStartDelay(table->start_time);

    if (table->time_to_die == 0)
        return NULL;
    while (true)
    {
        if (hasAnyoneDied(table))
            return NULL;
        if (hasSimStopped(table))
            return NULL;

        if (table->min_dining != -1 && areMealsCompleted(table))
        {
            pthread_mutex_lock(&table->sim_stop_lock);
            table->sim_stop = true;
            pthread_mutex_unlock(&table->sim_stop_lock);
            printf("ALL MEALS COMPLETE.\n");
            break;
        }
    }
    return NULL;
}