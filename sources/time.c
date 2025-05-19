#include "philo.h"


/**
 * @brief Busy-wait until the specified start time.
 *
 * Loops until current time reaches or exceeds given time.
 *
 * @param time Target start time in milliseconds.
 */
void simStartDelay(time_t time)
{
    while (getTimeIn_ms() < time)
        continue;
}


/**
 * @brief Get current time in milliseconds.
 *
 * Uses gettimeofday to retrieve current time and converts
 * it to milliseconds.
 *
 * @return Current time in milliseconds.
 */
time_t getTimeIn_ms(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}


/**
 * @brief Pause philosopher activity for given time or until death occurs.
 *
 * Sleeps in small increments (1 ms) until the specified session
 * duration elapses or any philosopher dies.
 *
 * @param philo Pointer to the philosopher.
 * @param session Duration to sleep in milliseconds.
 */
void    lullPhilo(t_philo *philo, int session)
{
    time_t  beginning;

    beginning = getTimeIn_ms();
    while (hasAnyoneDied(philo->table) == false)
    {
        if (getTimeIn_ms() - beginning < session)
            usleep(1000);
        else
            break;
    }
}