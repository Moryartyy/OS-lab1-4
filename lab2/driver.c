/**
 * Driver.c
 *
 * Schedule is in the format
 *
 *  [name] [priority] [CPU burst]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "list.h"
#include "schedulers.h"

#define SIZE    100

int main(int argc, char *argv[])
{
    FILE *in;
    char *temp;
    char task[SIZE];

    char *name;
    int priority;
    int burst;

    int countT = 0;

    in = fopen(argv[1],"r");
    
    while (fgets(task,SIZE,in) != NULL) {
        temp = strdup(task);
        name = strsep(&temp,",");
        priority = atoi(strsep(&temp,","));
        burst = atoi(strsep(&temp,","));

        // add the task to the scheduler's list of tasks
        add(name,priority,burst);

        free(temp);
        countT++;
    }

    fclose(in);

    Parameters parameters = {0,0,0};
    // invoke the scheduler
    schedule(&parameters);

    float turnaroundTime = (float)parameters.turnaroundTime / countT;
    float waitingTime = (float)parameters.waitingTime / countT;
    float responseTime = (float)parameters.responseTime / countT;

    printf("Оборотное время: %.3f\n", turnaroundTime);
    printf("Время ожидания: %.3f\n", waitingTime);
    printf("Время отклика: %.3f\n", responseTime);

    return 0;
}
