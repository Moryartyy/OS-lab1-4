/**
 * Representation of a task in the system.
 */

#ifndef TASK_H
#define TASK_H

typedef struct {
    int turnaroundTime;
    int waitingTime;
    int responseTime;
} Parameters;

typedef struct task {
    char *name;
    int tid;
    int priority;
    int burst;
} Task;

#endif
