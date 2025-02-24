/**
 * Example client program that uses thread pool.
 */

#include <stdio.h>
#include <unistd.h>
#include "threadpool.h" // Подключение заголовочного файла для работы с пулом потоков

// Структура для передачи данных в функцию
struct data
{
    int a;
    int b;
};

// Функция, которая будет выполняться в потоках
void add(void *param)
{
    struct data *temp;
    temp = (struct data*)param;

    // Вывод результата сложения двух чисел
    printf("I add two values %d and %d result = %d\n",temp->a, temp->b, temp->a + temp->b);
}

int main(void)
{
    // Создание данных для работы
    struct data work1;
    work1.a = 2;
    work1.b = 2;

    struct data work2;
    work2.a = 3;
    work2.b = 3;

    struct data work3;
    work3.a = 5;
    work3.b = 5;

    // Инициализация пула потоков
    pool_init();

    // Подача работы в очередь пула потоков
    pool_submit(&add,&work1);
    pool_submit(&add,&work2);
    pool_submit(&add,&work3);

    pool_submit(&add,&work1);
    pool_submit(&add,&work2);
    pool_submit(&add,&work3);

    pool_submit(&add,&work1);
    pool_submit(&add,&work2);
    pool_submit(&add,&work3);

    // Может быть полезно для проверки работы
    sleep(3);

    // Завершение работы пула потоков
    pool_shutdown();

    return 0;
}
