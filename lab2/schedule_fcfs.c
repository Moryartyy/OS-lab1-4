// Первым пришел — первым обслужен» (FCFS), при котором задачи планируются
// в том порядке, в котором они запрашивают ЦПУ.

#include <stdio.h>
#include <stdlib.h>
#include "task.h"
#include "list.h"
#include "cpu.h"

// Указатель на начало списка задач
struct node *head = NULL;

// Функция для добавления новой задачи в список
void add(char *name, int priority, int burst) {
    // Создание новой задачи и инициализация ее полей
    Task *task = malloc(sizeof(Task));
    task->name = name;
    task->priority = priority;
    task->burst = burst;
    
    // Вставка задачи в список
    insert(&head, task);
}

// Функция для выбора следующей задачи для выполнения (FCFS - по порядку добавления)
Task *pickNextTask() {
    // Проверка, что список задач не пуст
    if (head != NULL) {
        struct node *curr = head;
        struct node *prev = NULL;

        // Проход по списку к последнему элементу (так как задачи добавляются в начало списка)
        while (curr->next != NULL) {
            prev = curr;
            curr = curr->next;
        }
        Task *last = curr->task;

        // Удаление выбранной задачи из списка
        if (prev != NULL) {
            delete(&head, last);
        } else {
            delete(&head, head->task);
        }
        return last;
    } else {
        // Если список пуст, возвращаем NULL
        return NULL;
    }
}

// Функция планировщика задач
void schedule(Parameters *parameters) {
    Task *task = pickNextTask(); // Выбор первой задачи
    int currentTime = 0; // Текущее время выполнения

    // Цикл выполнения задач
    while (task != NULL) {
        parameters->responseTime += currentTime; // Обновление времени отклика

        run(task, task->burst); // Выполнение задачи

        currentTime += task->burst; // Обновление текущего времени
        parameters->turnaroundTime += currentTime; // Обновление оборотного времени

        task = pickNextTask(); // Выбор следующей задачи
    }
}
