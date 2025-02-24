//Циклическое планирование с приоритетом: задачи планируются в порядке приоритета и используется
// циклическое планирование для задач с одинаковым приоритетом.

#include "stdio.h"
#include "stdlib.h"
#include "task.h"
#include "list.h"
#include "cpu.h"

#define QUANT 10 // Определяем квант времени для циклического планирования

// Указатели на начало списка задач
struct node *head = NULL;
struct node *temp = NULL;

// Указатели на список задач для приоритетной сортировки
struct node *pr_unsort = NULL;
struct node *pr_sort = NULL;

// Массив для подсчета количества задач с определенным приоритетом
int priority_count[10] = {0};

// Функция для добавления новой задачи в список
void add(char *name, int priority, int burst) {
    Task *task = malloc(sizeof(Task));
    task->name = name;
    task->priority = priority;
    task->burst = burst;

    priority_count[priority]++; // Увеличиваем счетчик задач с данным приоритетом

    insert(&head, task); // Вставляем задачу в список
}

// Функция для выбора задачи с наивысшим приоритетом из списка pr_unsort
Task *pickPriorityTask() {
    if (pr_unsort != NULL) {
        Task *max = pr_unsort->task;
        struct node *curr = pr_unsort->next;

        while (curr != NULL) {
            if (curr->task->priority < max->priority)
                max = curr->task;

            curr = curr->next;
        }

        delete(&pr_unsort, max); // Удаляем выбранную задачу из списка

        return max;
    } else {
        return NULL; // Если список пуст, возвращаем NULL
    }
}

// Функция для выбора следующей задачи из временного списка
Task *pickNextTask() {
    if (temp != NULL) {
        Task *task = temp->task;
        temp = temp->next;
        return task;
    } else {
        return NULL; // Если временный список пуст, возвращаем NULL
    }
}

// Функция для циклического планирования задач (Round Robin)
void schedule_rr(Parameters *parameters, int *currTime) {
    temp = head;
    int fl_rt = 0; // Флаг для первого вычисления времени отклика
    int fl_tr = 0; // Флаг для первого вычисления времени оборота
    int temp_turnaround = 0;

    while (head != NULL) {
        Task *task = pickNextTask();
        int currentTime = 0;

        while (task != NULL) {
            if (fl_rt == 0)
                parameters->responseTime += currentTime; // Увеличиваем время отклика при первом выполнении задачи

            if (QUANT >= task->burst) {
                run(task, task->burst); // Выполняем задачу до конца
                delete(&head, task);

                currentTime += task->burst; 
                *currTime += task->burst;
                parameters->turnaroundTime += currentTime; // Увеличиваем время оборота
                parameters->waitingTime += currentTime - task->burst; // Увеличиваем время ожидания
            } else {
                run(task, QUANT); // Выполняем задачу на квант времени
                task->burst -= QUANT;

                currentTime += QUANT; 
                *currTime += QUANT;
                parameters->turnaroundTime += currentTime; // Увеличиваем время оборота
                parameters->waitingTime += currentTime - QUANT; // Увеличиваем время ожидания
            }

            task = pickNextTask();
        }

        temp = head;

        if (fl_tr == 0) {
            temp_turnaround = parameters->turnaroundTime;
            fl_tr = 1;
        }

        fl_rt = 1;
    }

    parameters->turnaroundTime += temp_turnaround;
}

// Основная функция планировщика
void schedule(Parameters *parameters) {
    pr_unsort = head;
    pr_sort = NULL;

    // Сортируем задачи по приоритету
    Task *task_pr = pickPriorityTask();
    while (task_pr != NULL) {
        insert(&pr_sort, task_pr);
        task_pr = pickPriorityTask();
    }

    struct node *curr = pr_sort;
    int currentTime = 0;

    // Планируем выполнение задач
    while (curr != NULL) {
        Task *task = curr->task;

        if (priority_count[task->priority] > 1) { // Если есть несколько задач с одним приоритетом
            struct node *pr_temp = NULL;
            while (priority_count[task->priority] != 0) {
                Task *task_pr = curr->task;
                insert(&pr_temp, task_pr);
                priority_count[task->priority]--;
                curr = curr->next;
            }
            head = pr_temp;
            schedule_rr(parameters, &currentTime); // Планируем задачи с одинаковым приоритетом по циклическому алгоритму
        } else {
            parameters->responseTime += currentTime; // Увеличиваем время отклика

            run(task, task->burst); // Выполняем задачу
            curr = curr->next;

            currentTime += task->burst;
            parameters->turnaroundTime += currentTime; // Увеличиваем время оборота
            parameters->waitingTime += currentTime - task->burst; // Увеличиваем время ожидания
        }
    }
}
