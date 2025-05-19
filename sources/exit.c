#include "philo.h"


/**
 * @brief Prints a message and returns the given value.
 *
 * @param str Message to print.
 * @param i Value to return.
 * @return The value of i.
 */
int msg(char *str, int i)
{
    printf("%s\n", str);
    return i;
}


/**
 * @brief Frees the table and returns NULL.
 *
 * Useful for cleanup in error cases.
 *
 * @param table Pointer to the simulation table.
 * @return Always returns NULL.
 */
void *freeTableExit(t_table *table)
{
    freeTable(table);
    return NULL;
}


/**
 * @brief Frees all dynamically allocated memory used in the simulation table.
 *
 * This function is responsible for cleaning up memory associated with:
 * - The array of fork mutexes.
 * - Each philosopher's structure.
 * - The array of philosopher pointers.
 * - The table structure itself.
 *
 * It should be called at the end of the program or upon failure to prevent
 * memory leaks and ensure proper resource deallocation.
 *
 * @param table A pointer to the simulation table structure to be freed.
 */
void freeTable(t_table *table)
{
    int i;

    free(table->fork_locks);
    i = -1;
    while (++i < table->num_philos)
    {
        free(table->philos[i]);
    }
    free(table->philos);
    free(table);
}