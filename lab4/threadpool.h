// function prototypes
#pragma once

#include "queue.h"       // Включение заголовочного файла для работы с очередью
#include <pthread.h>     // Включение заголовочного файла для работы с потоками
#include <stdlib.h>      // Включение заголовочного файла для стандартных функций обработки памяти
#include <stdio.h>       // Включение заголовочного файла для стандартного ввода/вывода
#include <semaphore.h>   // Включение заголовочного файла для работы с семафорами

// function prototypes
void execute(void (*somefunction)(void* p), void* p); // Прототип функции для выполнения задачи
int pool_submit(void (*somefunction)(void* p), void* p); // Прототип функции для подачи задачи в пул потоков
void* worker(void* param); // Прототип функции-рабочего потока
void pool_init(void); // Прототип функции для инициализации пула потоков
void pool_shutdown(void); // Прототип функции для завершения работы пула потоков

// this represents work that has to be completed by a thread in the pool
typedef struct {
    void (*function)(void* p); // Указатель на функцию задачи
    void* data;                // Данные, передаваемые в функцию задачи
} task; // Структура задачи, которая должна быть выполнена потоком в пуле потоков

typedef struct {
    pthread_mutex_t mutex;   // Мьютекс для обеспечения доступа к ресурсам пула потоков
    sem_t sem;               // Семафор для управления потоками
    pthread_t* threads;      // Массив потоков
    queue_t queue;           // Очередь задач
} threadpool; // Структура пула потоков
