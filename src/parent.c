#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <locale.h>

extern char **environ;

#define MAX_ENV_VARS 100
#define CHILD_NAME_FORMAT "child_%02d"
#define ENV_FILE_PATH ".env"  // Жестко заданный путь к файлу .env

void print_sorted_env() {
    extern char **environ;
    char *env_vars[MAX_ENV_VARS];
    int count = 0;

    // Копируем переменные окружения в массив
    for (char **env = environ; *env != NULL && count < MAX_ENV_VARS; env++) {
        env_vars[count++] = *env;
    }

    if (count >= MAX_ENV_VARS) {
        fprintf(stderr, "Too many environment variables\n");
        return;
    }

    // Сортируем переменные окружения
    setlocale(LC_COLLATE, "C");
    qsort(env_vars, count, sizeof(char*), (int (*)(const void*, const void*))strcmp);

    // Выводим отсортированные переменные окружения
    for (int i = 0; i < count; i++) {
        printf("%s\n", env_vars[i]);
    }
}

void launch_child(int child_num, const char *env_file, char *const envp[]) {
    char child_name[10];
    snprintf(child_name, sizeof(child_name), CHILD_NAME_FORMAT, child_num);

    char child_path[256];
    const char *child_dir = getenv("CHILD_PATH");
    if (child_dir == NULL) {
        fprintf(stderr, "CHILD_PATH not set\n");
        return;
    }
    snprintf(child_path, sizeof(child_path), "%s/child", child_dir);

    pid_t pid = fork();
    if (pid == 0) {
        // Дочерний процесс
        const char *argv[] = {child_name, env_file, NULL};
        execve(child_path, (char *const *)argv, envp);
        perror("execve");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // Родительский процесс
        wait(NULL); // Ожидаем завершения дочернего процесса
    } else {
        perror("fork");
    }
}

int main(void) {
    // Выводим отсортированные переменные окружения
    print_sorted_env();

    int child_num = 0;
    char input;

    while (1) {
        printf("Enter '+', '*', or 'q': ");
        if (scanf(" %c", &input) != 1) {
            fprintf(stderr, "Invalid input\n");
            while (getchar() != '\n'); // Очистка буфера ввода
            continue;
        }

        if (input == '+') {
            launch_child(child_num++, ENV_FILE_PATH, environ);
        } else if (input == '*') {
            // Запуск дочернего процесса с передачей окружения через envp
            launch_child(child_num++, NULL, environ);
        } else if (input == 'q') {
            break;
        } else {
            printf("Invalid input\n");
        }
    }

    return 0;
}
