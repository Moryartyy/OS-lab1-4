#include "threadpool.h"

#define QUEUE_SIZE 10
#define NUMBER_OF_THREADS 3

threadpool pool; // Глобальная переменная для хранения информации о пуле потоков

// Инициализация пула потоков
void pool_init(void) {
    // Выделение памяти под массив потоков
    pool.threads = (pthread_t*)malloc(sizeof(pthread_t) * NUMBER_OF_THREADS);

    if (pool.threads == NULL) {
        perror("Error allocating memory for threads");
        exit(EXIT_FAILURE);
    }

    // Инициализация очереди
    initialize_queue(&pool.queue);

    // Инициализация мьютекса
    if (pthread_mutex_init(&pool.mutex, NULL) != 0) {
        perror("Error initializing mutex");
        exit(EXIT_FAILURE);
    }

    // Инициализация семафора
    if (sem_init(&pool.sem, 0, 0) != 0) {
        perror("Error initializing semaphore");
        exit(EXIT_FAILURE);
    }

    // Создание и запуск рабочих потоков
    for (int i = 0; i < NUMBER_OF_THREADS; ++i) {
        if (pthread_create(&pool.threads[i], NULL, worker, NULL) != 0) {
            perror("Error creating thread");
            exit(EXIT_FAILURE);
        }
    }
}

// Функция, которую выполняют рабочие потоки
void* worker(void* param) {
    while (1) {
        sem_wait(&pool.sem); // Ожидание сигнала для начала работы

        pthread_mutex_lock(&pool.mutex); // Блокировка мьютекса для доступа к очереди
        task* current_task = (task*)dequeue(&pool.queue); // Получение задачи из очереди
        pthread_mutex_unlock(&pool.mutex); // Разблокировка мьютекса

        if (current_task == NULL) {
            pthread_exit(NULL); // Если задачи нет, завершаем поток
        }

        execute(current_task->function, current_task->data); // Выполнение задачи
    }
}

// Выполнение задачи
void execute(void (*somefunction)(void* p), void* p) {
    (*somefunction)(p); // Вызов функции задачи
}

// Подача работы в пул потоков
int pool_submit(void (*somefunction)(void* p), void* p) {
    task* t = (task*)malloc(sizeof(task)); // Выделение памяти под задачу
    t->function = somefunction;
    t->data = p;

    pthread_mutex_lock(&pool.mutex); // Блокировка мьютекса для доступа к очереди
    if (enqueue(&pool.queue, t) != 0) {
        return 1; // В случае ошибки возвращаем 1
    }
    pthread_mutex_unlock(&pool.mutex); // Разблокировка мьютекса

    sem_post(&pool.sem); // Отправка сигнала рабочему потоку о начале работы

    return 0; // В случае успеха возвращаем 0
}

// Завершение работы пула потоков
void pool_shutdown(void) {
    // Ожидание завершения всех задач в очереди
    while (pool.queue.size > 0) { }

    // Отправка сигналов рабочим потокам о завершении работы
    for (int i = 0; i < NUMBER_OF_THREADS; ++i) {
        sem_post(&pool.sem);
    }

    // Ожидание завершения всех рабочих потоков
    for (int i = 0; i < NUMBER_OF_THREADS; ++i) {
        pthread_join(pool.threads[i], NULL);
    }

    // Освобождение памяти, выделенной под массив потоков
    free(pool.threads);

    // Уничтожение мьютекса
    pthread_mutex_destroy(&pool.mutex);

    // Уничтожение семафора
    sem_destroy(&pool.sem);
}
