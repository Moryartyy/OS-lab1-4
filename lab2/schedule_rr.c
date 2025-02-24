//Циклическое планирование (RR), при котором каждая задача выполняется в течение определенного кванта времени.
// (или оставшуюся часть времени использования ЦПУ).

#include "stdio.h"
#include "stdlib.h"
#include "task.h"
#include "list.h"
#include "cpu.h"

#define QUANT 10 // Квант времени для циклического планирования (Round Robin)

// Указатель на начало списка задач
struct node *head = NULL;
// Временный указатель для цикла обработки задач
struct node *temp = NULL;

// Функция для добавления новой задачи в список
void add(char *name, int priority, int burst) {
    Task *task = malloc(sizeof(Task));
    task->name = name;
    task->priority = priority;
    task->burst = burst;

    insert(&head, task); // Вставляем задачу в список
}

// Функция для выбора следующей задачи из списка
Task *pickNextTask() {
    if (temp != NULL) {
        Task *task = temp->task;
        temp = temp->next; // Переходим к следующей задаче в списке
        return task;
    } else {
        return NULL; // Если список пуст, возвращаем NULL
    }
}

// Основная функция планировщика
void schedule(Parameters *parameters) {
    temp = head; // Инициализируем временный указатель началом списка
    int fl_rt = 0; // Флаг для времени отклика
    int fl_tr = 0; // Флаг для времени оборота
    int temp_turnaround = 0; // Переменная для временного хранения времени оборота

    while (head != NULL) { // Пока есть задачи в списке
        Task *task = pickNextTask(); // Выбираем следующую задачу
        int currentTime = 0; // Инициализируем текущее время

        while (task != NULL) {
            if (fl_rt == 0)
                parameters->responseTime += currentTime; // Увеличиваем время отклика при первом запуске

            if (QUANT >= task->burst) { // Если оставшееся время выполнения задачи меньше или равно кванту
                run(task, task->burst); // Выполняем задачу
                delete(&head, task); // Удаляем задачу из списка

                currentTime += task->burst; // Увеличиваем текущее время на время выполнения задачи
                parameters->turnaroundTime += currentTime; // Увеличиваем время оборота
                parameters->waitingTime += currentTime - task->burst; // Увеличиваем время ожидания
            } else { // Если оставшееся время выполнения задачи больше кванта
                run(task, QUANT); // Выполняем задачу в течение кванта
                task->burst -= QUANT; // Уменьшаем оставшееся время выполнения задачи

                currentTime += QUANT; // Увеличиваем текущее время на квант
                parameters->turnaroundTime += currentTime; // Увеличиваем время оборота
                parameters->waitingTime += currentTime - QUANT; // Увеличиваем время ожидания
            }

            task = pickNextTask(); // Выбираем следующую задачу
        }

        temp = head; // Сбрасываем временный указатель на начало списка
        if (fl_tr == 0) { // Если время оборота еще не установлено
            temp_turnaround = parameters->turnaroundTime; // Устанавливаем временное время оборота
            fl_tr = 1; // Устанавливаем флаг времени оборота
        }
        fl_rt = 1; // Устанавливаем флаг времени отклика
    }
    parameters->turnaroundTime += temp_turnaround; // Добавляем временное время оборота к общему времени оборота
}
