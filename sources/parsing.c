#include "philo.h"


/**
 * @brief Validate command-line arguments.
 *
 * Checks if the number of philosophers and timing values are positive integers.
 * Prints an error message and returns false if any validation fails.
 *
 * @param ac Argument count.
 * @param av Argument vector.
 * @return true if all arguments are valid, false otherwise.
 */
bool isValid(int ac, char **av)
{
    if (atoi(av[1]) < 1)
    {
        printf("Number of Philosophers must be a positive integer.\n");
        return false;
    }
    if (atoi(av[2]) < 0)
    {
        printf("Time to die (milli seconds) must be a positive integer.\n");
        return false;
    }
    if (atoi(av[3]) < 0)
    {
        printf("Time to eat (milli seconds) must be a positive integer.\n");
        return false;
    }
    if (atoi(av[4]) < 0)
    {
        printf("Time to sleep (milli seconds) must be a positive integer.\n");
        return false;
    }
    if (ac == 6 && atoi(av[5]) < 0)
    {
        printf("Minimum number of meals each philosopher must eat must be a positive integer.\n");
        return false;
    }
    return true;
}
