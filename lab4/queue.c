#include "queue.h"

// Функция для инициализации очереди
void initialize_queue(queue_t* queue) {
    queue->size = 0;      // Устанавливаем начальный размер очереди как 0
    queue->head = NULL;   // Устанавливаем начало очереди как NULL
    queue->tail = NULL;   // Устанавливаем конец очереди как NULL
}

// Функция для добавления элемента в конец очереди
int enqueue(queue_t* queue, void* data) {
    node_t* new_node = (node_t*)malloc(sizeof(node_t)); // Выделяем память под новый узел
    if (!new_node) {
        return 1; // Возвращаем 1 в случае ошибки выделения памяти
    }
    new_node->data = data;  // Записываем данные в новый узел
    new_node->next = NULL;  // Устанавливаем указатель на следующий узел как NULL

    // Если очередь пуста, устанавливаем новый узел как начало и конец очереди
    if (queue->tail == NULL) {
        queue->head = new_node;
        queue->tail = new_node;
    }
    else {
        // Иначе добавляем новый узел в конец очереди и обновляем указатель tail
        queue->tail->next = new_node;
        queue->tail = new_node;
    }
    queue->size++;  // Увеличиваем размер очереди

    return 0;  // Возвращаем 0 в случае успешного добавления
}

// Функция для удаления элемента из начала очереди
void* dequeue(queue_t* queue) {
    if (queue->head == NULL) {
        return NULL;  // Если очередь пуста, возвращаем NULL
    }

    // Получаем значение из начала очереди и обновляем указатель head
    void* data = queue->head->data;
    struct Node* temp = queue->head;
    queue->head = queue->head->next;

    // Если после удаления очередь стала пустой, обновляем указатель tail
    if (queue->head == NULL) {
        queue->tail = NULL;
    }

    // Освобождаем память, выделенную для удаленного узла
    free(temp);

    queue->size--;  // Уменьшаем размер очереди

    return data;  // Возвращаем удаленное значение
}

// Функция для проверки, пуста ли очередь
int is_empty(queue_t* queue) {
    return queue->head == NULL;  // Возвращаем 1, если очередь пуста, иначе 0
}
