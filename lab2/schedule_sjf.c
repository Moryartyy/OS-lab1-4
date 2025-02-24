// Shortest-job-first (SJF), при котором задачи планируются в порядке
// продолжительности использования ими ЦПУ.
#include <stdio.h>
#include <stdlib.h>
#include "task.h"   // Заголовочный файл для описания структуры задачи
#include "list.h"   // Заголовочный файл для работы со списком задач
#include "cpu.h"    // Заголовочный файл для выполнения задачи на ЦПУ

struct node *head = NULL;   // Глобальная переменная для головы списка задач

// Функция для добавления задачи в список
void add(char *name,int priority,int burst){
    Task *task = malloc(sizeof(Task)); // Выделение памяти для новой задачи
    task->name=name;                    // Задание имени задачи
    task->priority = priority;          // Задание приоритета задачи
    task->burst = burst;                // Задание времени выполнения задачи
    insert(&head,task);                 // Вставка задачи в список
}

// Функция для выбора следующей задачи для выполнения по алгоритму SJF
Task *pickNextTask() {
    if (head != NULL) {
        Task *min = head->task;         // Инициализация минимальной задачи
        struct node *curr = head->next; // Инициализация указателя на следующую ноду

        // Поиск задачи с минимальным временем выполнения
        while (curr != NULL) {
            if (curr->task->burst < min->burst)
                min = curr->task;

            curr = curr->next;
        }

        delete(&head, min);  // Удаление выбранной задачи из списка

        return min;          // Возвращение выбранной задачи
    } else {
        return NULL;         // Возвращение NULL, если список пуст
    }
}

// Функция для планирования выполнения задач с помощью алгоритма SJF
void schedule(Parameters *parameters) {
    Task *task = pickNextTask();    // Выбор первой задачи для выполнения
    int currentTime = 0;            // Инициализация текущего времени выполнения
    while (task != NULL) {          // Пока есть задачи для выполнения
        parameters->responseTime += currentTime; // Время отклика
        run(task, task->burst);                  // Выполнение задачи на ЦПУ
        currentTime += task->burst;               // Обновление текущего времени
        parameters->turnaroundTime += currentTime; // Оборотное время
        task = pickNextTask();                    // Выбор следующей задачи
    }
}
