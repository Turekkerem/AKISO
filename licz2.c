#include <stdio.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <stdlib.h>

int semid;

void wait_semaphore(int semnum) {
    struct sembuf sb = {semnum, -1, 0};
    semop(semid, &sb, 1);
}

void signal_semaphore(int semnum) {
    struct sembuf sb = {semnum, 1, 0};
    semop(semid, &sb, 1);
}

int main() {
    int i, j;
    int semkey = 1234; // Klucz semafora

    // Utwórz zbiór semaforów
    semid = semget(semkey, 2, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("semget failed");
        exit(1);
    }

    // Ustaw początkowe wartości semaforów (sem1 = 1, sem2 = 0)
    semctl(semid, 0, SETVAL, 1);  // semafor1 (do wypisywania liczb nieparzystych)
    semctl(semid, 1, SETVAL, 0);  // semafor2 (do wypisywania liczb parzystych)

    for (i = 2; ; i += 2) {  // Liczby parzyste
        wait_semaphore(1);  // Czekaj na semafor2

        printf("%d ", i);
        for (j = 0; j < i; j++) {
            printf("%d ", i);
        }
        printf("\n");

        signal_semaphore(0);  // Sygnalizuj semafor1, aby count1.c mogło wypisać liczbę

        sleep(1);
    }

    return 0;
}

