#include <stdio.h> // perror , fgets
#include <stdlib.h> // waitpid
#include <string.h> // strcspn, strtok, strcmp
#include <unistd.h> // системные вызовы: getcwd,execvp,chdir
#include <unistd.h> //файла заголовка, обеспечивающего доступ к API операционной системы POSIX
#include <string.h> //заголовочный файл стандартной библиотеки языка Си, содержащий функции для работы со строками
#include <dirent.h> //возвращает указатель на структуру, содержащую информацию относительно файла
#include <sys/wait.h>
#include <sys/types.h>
#include <pwd.h>
#include <errno.h> // заголовочный файл стандартной библиотеки языка программирования С
#include "myshell.h" //файл заголовков функций оболочки

// Объявление переменных среды
extern char **environ;

// Функция для вывода приглашения
void print_prompt() {
    char cwd[1024];
    // Получаем текущий рабочий каталог и выводим его как приглашение
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("\x1b[38;2;255;128;0m%s>-\x1b[38;2;0;255;0m|EASY SHELL|->\x1b[38;2;255;255;255m", cwd);
    } else {
        perror("getcwd() error"); // Обрабатываем ошибку, если не удалось получить текущий каталог
    }
}

// Функция для обработки команды cd
void execute_cd(char *args[]) {
    if (args[1] == NULL) {
        // Если не указан каталог, выводим текущий каталог
        printf("%s\n", getenv("PWD"));
    } else if (chdir(args[1]) != 0) {
        // Меняем текущий каталог и обрабатываем возможную ошибку
        perror("myshell");
    } else {
        // Устанавливаем переменную среды PWD на новый каталог
        setenv("PWD", args[1], 1);
    }
}
// Функция для обработки команды dir
void execute_dir(char *args[]) {
    DIR *dir;
    struct dirent *entry;
    if (args[1] == NULL) {
        // Если не указан каталог, используем текущий каталог
        args[1] = ".";
    }
    if ((dir = opendir(args[1])) != NULL) {
        // Считываем и выводим содержимое каталога
        while ((entry = readdir(dir)) != NULL) {
            printf("%s\n", entry->d_name);
        }
        closedir(dir);
    } else {
        // Обрабатываем ошибку открытия каталога
        perror("myshell");
    }
}

// Функция для обработки команды environ
void execute_environ() {
    // Выводим все переменные среды
    for (char **env = environ; *env != 0; env++) {
        char *thisEnv = *env;
        printf("%s\n", thisEnv);
    }
}
// Функция для обработки команды pause
void execute_pause() {
    // Приостанавливаем работу оболочки до нажатия Enter
    printf("Press Enter to continue...\n");
    while (getchar() != '\n');
}

// Функция для обработки команды quit
void execute_quit() {
    // Завершаем работу оболочки
    exit(0);
}
// Функция для обработки команды echo
void execute_echo(char *args[]) {
    // Выводим переданный комментарий
    for (int i = 1; args[i] != NULL; i++) {
        printf("%s ", args[i]);
    }
    printf("\n");
}

// Функция для обработки команды help
void execute_help() {
    printf("\n| \x1b[38;2;255;128;0mmyshell \x1b[38;2;255;255;255m- Простая реализация оболочки.\n\n");
    printf("| \x1b[38;2;255;255;255mПоддерживаемые команды:\n\n");
    printf("| \x1b[38;2;255;128;0mcd <directory> \x1b[38;2;255;255;255m- Сменить каталог на <directory>.\n\n");
    printf("| \x1b[38;2;255;128;0mclr \x1b[38;2;255;255;255m- Очистить экран.\n\n");
    printf("| \x1b[38;2;255;128;0mdir <directory> \x1b[38;2;255;255;255m- Вывести содержимое каталога <directory>.\n\n");
    printf("| \x1b[38;2;255;128;0menviron \x1b[38;2;255;255;255m- Вывести все переменные среды.\n\n");
    printf("| \x1b[38;2;255;128;0mecho <comment> \x1b[38;2;255;255;255m- Вывести <comment> на экран.\n\n");
    printf("| \x1b[38;2;255;128;0mhelp \x1b[38;2;255;255;255m- Показать это сообщение с подсказками.\n\n");
    printf("| \x1b[38;2;255;128;0mpause \x1b[38;2;255;255;255m- Приостановить работу оболочки до нажатия Enter.\n\n");
    printf("| \x1b[38;2;255;128;0mquit \x1b[38;2;255;255;255m- Выйти из оболочки.\n\n");
}

// Функция для обработки команды clr
void execute_clr() {
    // Очищаем экран
    printf("\033[H\033[J");
}

void execute_external_command(char *args[]) {
    pid_t pid = fork();
    if (pid == 0) {
        // В дочернем процессе выполняем команду
        execvp(args[0], args);
        perror("myshell"); // Обрабатываем ошибку execvp
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Обрабатываем ошибку fork
        perror("myshell");
    } else {
        // В родительском процессе ждем завершения дочернего процесса
        wait(NULL);
    }
}

int main(int argc, char *argv[]){
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char *args[100];
    // Установить переменную среды shell
    setenv("shell", argv[0], 1);

    while (1) {
        print_prompt(); // Выводим приглашение

        read = getline(&line, &len, stdin); // Считываем строку ввода
        if (read == -1) {
            break;
        }
        // Убираем символ новой строки
        line[strcspn(line, "\n")] = 0;

        // Разделяем строку на аргументы
        int i = 0;
        args[i] = strtok(line, " ");
        while (args[i] != NULL) {
            args[++i] = strtok(NULL, " ");
            if (args[0] == NULL) {
            continue; // Если команда пустая, продолжаем цикл
            } else if (strcmp(args[0], "cd") == 0) {
                execute_cd(args); // Выполняем команду cd
            } else if (strcmp(args[0], "clr") == 0) {
                execute_clr(); // Выполняем команду clr
            } else if (strcmp(args[0], "dir") == 0) {
                execute_dir(args); // Выполняем команду dir
            } else if (strcmp(args[0], "environ") == 0) {
                execute_environ(); // Выполняем команду environ
            } else if (strcmp(args[0], "echo") == 0) {
                execute_echo(args); // Выполняем команду echo
            } else if (strcmp(args[0], "help") == 0) {
                execute_help(); // Выполняем команду help
            } else if (strcmp(args[0], "pause") == 0) {
                execute_pause(); // Выполняем команду pause
            } else if (strcmp(args[0], "quit") == 0) {
                execute_quit(); // Выполняем команду quit
            } else {
                execute_external_command(args); // Выполняем внешнюю команду
            }
        }
    }
}
