//Приоритетное планирование, при котором
// задачи планируются на основе приоритета.

#include <stdio.h>
#include <stdlib.h>
#include "task.h"
#include "list.h"
#include "cpu.h"

// Указатель на начало списка задач
struct node *head = NULL;

// Функция для добавления новой задачи в список
void add(char *name, int priority, int burst) {
    Task *task = malloc(sizeof(Task));
    task->name = name;
    task->priority = priority;
    task->burst = burst;
    insert(&head, task); // Вставляем задачу в список
}

// Функция для выбора следующей задачи с наивысшим приоритетом
Task *pickNextTask() {
    if (head != NULL) {
        Task *max = head->task;
        struct node *curr = head->next;

        // Проходим по списку задач и находим задачу с наивысшим приоритетом
        while (curr != NULL) {
            if (curr->task->priority > max->priority)
                max = curr->task;

            curr = curr->next;
        }

        delete(&head, max); // Удаляем выбранную задачу из списка

        return max;
    } else {
        return NULL; // Если список пуст, возвращаем NULL
    }
}

// Основная функция планировщика
void schedule(Parameters *parameters) {
    Task *task = pickNextTask(); // Выбираем следующую задачу с наивысшим приоритетом
    int currentTime = 0;

    while (task != NULL) {
        parameters->responseTime += currentTime; // Увеличиваем время отклика

        run(task, task->burst); // Выполняем задачу

        currentTime += task->burst; // Увеличиваем текущее время на время выполнения задачи
        parameters->turnaroundTime += currentTime; // Увеличиваем время оборота

        task = pickNextTask(); // Выбираем следующую задачу
    }
}
