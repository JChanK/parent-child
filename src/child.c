#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Функция для чтения и вывода переменных окружения из файла .env
void print_env_from_file(const char *env_file) {
    FILE *file = fopen(env_file, "r");
    if (!file) {
        perror("fopen");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0; // Убираем символ новой строки
        char *value = getenv(line);
        if (value) {
            printf("%s=%s\n", line, value);
        }
    }

    fclose(file);
}

// Функция для вывода переменных окружения из массива envp
void print_env_from_envp(char *const envp[]) {
    for (const char **env = (const char **)envp; *env != NULL; env++) {
        printf("%s\n", *env);
    }
}

int main(int argc, char *argv[], char *envp[]) {
    printf("Child Name: %s\n", argv[0]);
    printf("PID: %d\n", getpid());
    printf("PPID: %d\n", getppid());

    if (argc > 1 && argv[1] != NULL) {
        // Если передан файл .env, читаем из него
        print_env_from_file(argv[1]);
    } else {
        // Иначе читаем из envp
        print_env_from_envp(envp);
    }

    return 0;
}
