#include <stdio.h>


#define FRAME_SIZE 256 // размер фрейма 2^8 байт
#define COUNT_FRAMES 256 // всего 256 фреймов
#define TLB_SIZE 16 // 16 записей в TLB
#define PAGE_SIZE 256 // размер страницы 2^8 байт

// Объявление файловых указателей
FILE *address_txt;
FILE *backing_store_bin;
FILE *out_txt;

// Структура для представления страницы
struct page {
    int offset; // смещение в пределах страницы
    int page_number; // номер страницы
    int logical_address; // логический адрес
};

// Структура для хранения соответствий страница-фрейм
struct page_frame_number {
    int page_number; // номер страницы
    int frame_number; // номер фрейма
};

// Объявление памяти и таблиц
int physical_memory[COUNT_FRAMES][FRAME_SIZE];
struct page_frame_number TLB[TLB_SIZE];
struct page_frame_number PAGE_TABLE[COUNT_FRAMES];

// Маска для извлечения смещения из адреса
const int offset_mask = (1 << 8) - 1; // 255

// Буфер для чтения из backing store и переменная для значения
signed char buf[256];
signed char value;

int next_available_index = 0; // индекс следующего доступного фрейма
int cached = 0; // количество закэшированных записей в TLB
int hit = 0; // количество попаданий в TLB
int page_miss = 0; // количество промахов по странице

// Функция для чтения страницы из backing store
int read_store(int page_number) {
    // Проверка доступности места в таблице страниц
    if (next_available_index >= COUNT_FRAMES) {
        return -1;
    }

    // Считывание данных из backing store в буфер
    fseek(backing_store_bin, page_number * PAGE_SIZE, SEEK_SET);
    fread(buf, sizeof(signed char), PAGE_SIZE, backing_store_bin);

    // Копирование данных из буфера в физическую память
    for (int i = 0; i < PAGE_SIZE; i++) {
        physical_memory[next_available_index][i] = buf[i];
    }

    // Обновление таблицы страниц
    PAGE_TABLE[next_available_index].page_number = page_number;
    PAGE_TABLE[next_available_index].frame_number = next_available_index;

    int new_frame = next_available_index;
    next_available_index++;

    return new_frame;
}

// Функция для вставки записи в TLB по алгоритму FIFO
void insert_TLB_FIFO(int page_number, int frame_number) {
    int i;
    // Сдвиг записей для освобождения места для новой записи, если находим в TLB
    for (i = 0; i < cached; i++) {
        if (TLB[i].page_number == page_number) {
            while (i < cached - 1) {
                TLB[i] = TLB[i + 1];
                i++;
            }
            break;
        }
    }
    // Если не находим совпадение по номеру страницы
    if (i == cached)
        for (int j = 0; j < i; j++)
            TLB[j] = TLB[j + 1];

    // Вставка новой записи
    TLB[i].page_number = page_number;
    TLB[i].frame_number = frame_number;

    // Обновление количества кэшированных записей
    if (cached < TLB_SIZE - 1)
        cached++;
}

// Функция для получения страницы из логического адреса
struct page get_page(int logical_address) {
    struct page current_page;
    current_page.offset = logical_address & offset_mask; // извлечение смещения
    current_page.page_number = (logical_address >> 8) & offset_mask; // извлечение номера страницы
    current_page.logical_address = logical_address;
    return current_page;
}

// Функция для обработки виртуальной страницы
void process_virtual_page(struct page current_page) {
    int page_number = current_page.page_number;
    int offset = current_page.offset;
    int logical_address = current_page.logical_address;

    int frame_number = -1;

    // Поиск в TLB
    for (int i = 0; i < cached + 1; i++) {
        if (TLB[i].page_number == page_number) {
            frame_number = TLB[i].frame_number;
            hit++;
            break;
        }
    }

    // Поиск в таблице страниц
    if (frame_number == -1) {
        for (int i = 0; i < next_available_index; i++) {
            if (PAGE_TABLE[i].page_number == page_number) {
                frame_number = PAGE_TABLE[i].frame_number;
                break;
            }
        }
    }

    // Чтение из backing store, если не нашли в TLB и таблице страниц
    if (frame_number == -1) {
        frame_number = read_store(page_number);
        page_miss++;
    }

    // Вставка записи в TLB
    insert_TLB_FIFO(page_number, frame_number);
    value = physical_memory[frame_number][offset];

    // Запись результата в выходной файл
    fprintf(out_txt, "Virtual address: %d Physical address: %d Value: %d\n", logical_address, (frame_number << 8) | offset, value);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        return -1;
    }

    // Открытие файлов
    address_txt = fopen(argv[1], "r");
    backing_store_bin = fopen("BACKING_STORE.bin", "rb");
    out_txt = fopen(argv[2], "w");

    if (address_txt == NULL || backing_store_bin == NULL || out_txt == NULL) {
        return -1;
    }

    int processed_address = 0; // количество обработанных адресов
    int logical_address; // логический адрес (виртуальный адрес)

    // Чтение логических адресов из файла и их обработка
    while (fscanf(address_txt, "%d", &logical_address) == 1) {
        process_virtual_page(get_page(logical_address));
        processed_address++;
    }

    // Вычисление и запись частоты ошибок страниц и TLB
    double Page_error_rate = page_miss / (double)processed_address;
    double TLB_error_rate = hit / (double)processed_address;

    fprintf(out_txt, "Частота ошибок страниц = %f\n", Page_error_rate);
    fprintf(out_txt, "Частота попаданий в TLB = %f\n", TLB_error_rate);

    // Закрытие файлов
    fclose(address_txt);
    fclose(backing_store_bin);
    fclose(out_txt);

    return 0;
}
