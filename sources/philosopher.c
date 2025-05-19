#include "philo.h"


/**
 * @brief Safely increment philosopher's meal count.
 *
 * Locks meal_time mutex, increments times_ate, then unlocks.
 *
 * @param philo Pointer to the philosopher.
 */
static void updateTimesAte(t_philo *philo)
{
    pthread_mutex_lock(&philo->meal_time_lock);
    philo->times_ate ++;
    pthread_mutex_unlock(&philo->meal_time_lock);
} 


/**
 * @brief Update philosopher's last meal timestamp safely.
 * 
 * Locks meal_time mutex, sets last_meal to current time in ms, then unlocks.
 *
 * @param philo Pointer to the philosopher.
 */
static void stampLastMeal(t_philo *philo)
{
    pthread_mutex_lock(&philo->meal_time_lock);
    philo->last_meal = getTimeIn_ms();
    pthread_mutex_unlock(&philo->meal_time_lock);
}


/**
 * @brief Philosopher's eating routine.
 * 
 * Locks forks in order, checks simulation status between steps.
 * Updates last meal time, prints statuses, and simulates eating.
 * Unlocks forks after eating and updates times eaten.
 *
 * @param philo Pointer to the philosopher.
 */
static void eatRoutine(t_philo *philo)
{   
    if (hasAnyoneDied(philo->table))
        return;
    pthread_mutex_lock(&philo->table->fork_locks[philo->fork[0]]);
    if (hasAnyoneDied(philo->table))
        return;
    writeStatus(philo, GOT_RIGHT_FORK);

    if (hasAnyoneDied(philo->table))
        return;
    pthread_mutex_lock(&philo->table->fork_locks[philo->fork[1]]);
    if (hasAnyoneDied(philo->table))
        return;
    writeStatus(philo, GOT_LEFT_FORK);

    stampLastMeal(philo);
    
    if (hasAnyoneDied(philo->table))
        return;
    if (philo->table->time_to_eat != 0)
    {
        writeStatus(philo, EATING);
        lullPhilo(philo, philo->table->time_to_eat);
    }
    pthread_mutex_unlock(&philo->table->fork_locks[philo->fork[0]]);
    pthread_mutex_unlock(&philo->table->fork_locks[philo->fork[1]]);
    if (hasAnyoneDied(philo->table))
        return;

    updateTimesAte(philo);
}


/**
 * @brief Philosopher's sleeping routine.
 *
 * Checks if simulation is still running and time_to_sleep is non-zero.
 * Writes sleeping status and pauses the philosopher for time_to_sleep duration.
 *
 * @param philo Pointer to the philosopher.
 */
static void sleepRoutine(t_philo *philo)
{
    if (!hasAnyoneDied(philo->table) && philo->table->time_to_sleep != 0)
    {
        writeStatus(philo, SLEEPING);
        lullPhilo(philo, philo->table->time_to_sleep);
    }
}


/**
 * @brief Philosopher's thinking routine.
 *
 * Calculates thinking time based on time_to_die, time_to_eat, and time_to_sleep.
 * Adjusts thinking time if not the first thinking cycle and avoids negative or too long times.
 * Writes status and simulates thinking delay if no philosopher has died.
 *
 * @param philo Pointer to the philosopher.
 * @param first Boolean indicating if this is the first thinking cycle.
 */
static void thinkingRoutine(t_philo *philo, bool first)
{
    int thinking_time;

    if (hasAnyoneDied(philo->table))
        return;

    thinking_time = 
            (philo->table->time_to_die - philo->table->time_to_eat - philo->table->time_to_sleep) / 2;
    if (!first)
    {
        pthread_mutex_lock(&philo->meal_time_lock);
        if (thinking_time > (philo->table->time_to_die - (getTimeIn_ms() - philo->last_meal))) 
            thinking_time /= 2;
        pthread_mutex_unlock(&philo->meal_time_lock);
    }
    if (thinking_time <= 0)
        thinking_time = 1;
    else if (thinking_time > 600)
        thinking_time = 200;

    if (!hasAnyoneDied(philo->table))
    {
        writeStatus(philo, THINKING);
        lullPhilo(philo, thinking_time);
    }
}


/**
 * @brief Routine for the single philosopher scenario.
 *
 * Locks the only fork, reports it, waits until philosopher dies and
 * then unlocks the fork. Consequently the monitor thread reports death.
 *
 * @param philo Pointer to the philosopher structure.
 * @return Always returns NULL.
 */
static void *lonePhiloRoutine(t_philo *philo)
{
    pthread_mutex_lock(&philo->table->fork_locks[philo->fork[0]]);
    writeStatus(philo, GOT_RIGHT_FORK);
    lullPhilo(philo, philo->table->time_to_die);
    pthread_mutex_unlock(&philo->table->fork_locks[philo->fork[0]]);

    return NULL;
}


/**
 * @brief Main routine for each philosopher thread.
 *
 * Handles philosopher's lifecycle: waiting for simulation start,
 * special case for single philosopher, alternating actions of
 * thinking, eating, and sleeping until simulation stops or death.
 *
 * @param data Pointer to philosopher structure.
 * @return NULL when routine ends.
 */
void *philosopherRoutine(void *data)
{
    t_philo *philo;

    philo = (t_philo *)data;

    simStartDelay(philo->table->start_time);
    if (hasPhiloDied(philo))
        return NULL;
    if (philo->table->num_philos == 1)
        return lonePhiloRoutine(philo);
    if (philo->id % 2)
    {
        thinkingRoutine(philo, true);
    }
    while (!hasSimStopped(philo->table))
    {
        eatRoutine(philo);
        sleepRoutine(philo);
        thinkingRoutine(philo, false);
    }
    return NULL; 
}