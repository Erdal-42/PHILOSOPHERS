#include "philo.h"

/**
 * @brief Print the current status of a philosopher safely.
 *
 * Maps the status enum to a message, locks necessary mutexes to avoid
 * race conditions, then prints the timestamp, philosopher ID, and status.
 *
 * @param philo Pointer to the philosopher.
 * @param state Current status of the philosopher.
 */
void writeStatus(t_philo *philo, STATUS state)
{
    char    *str;

    if (state == GOT_RIGHT_FORK)
        str = "has taken right fork";
    else if (state == GOT_LEFT_FORK)
        str = "has taken left fork";
    else if (state == EATING)
        str = "is eating";
    else if (state == SLEEPING)
        str = "is sleeping";
    else if (state == THINKING)
        str = "is thinking";
    else if (state == DIED)
        str = "died";
    pthread_mutex_lock(&philo->meal_time_lock);
    pthread_mutex_lock(&philo->table->write_lock);
    printf("%ld ms\t%d\t%s\n", 
        getTimeIn_ms() - philo->table->start_time, philo->id, str);

    pthread_mutex_unlock(&philo->meal_time_lock);
    pthread_mutex_unlock(&philo->table->write_lock);
}
