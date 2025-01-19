#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    extern char **environ;

    if (argc == 1) {
        // Brak argumentów: wyświetl wszystkie zmienne środowiskowe
        for (char **env = environ; *env != NULL; ++env) {
            printf("%s\n", *env);
        }
    } else {
        // Argumenty podane: wyświetl tylko podane zmienne
        for (int i = 1; i < argc; ++i) {
            char *value = getenv(argv[i]);
            if (value) {
                printf("%s=%s\n", argv[i], value);
            } else {
                printf("Zmiennej środowiskowej '%s' nie znaleziono.\n", argv[i]);
            }
        }
    }

    return 0;
}
