#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MSG_KEY 1234

struct msgbuf {
    long mtype;
    int number;
};

int main() {
    int msgid;
    struct msgbuf message;
    message.mtype = 1;

    msgid = msgget(MSG_KEY, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }

    for (int i = 1; i <= 100; i++) { // przykładowe liczby 1 do 10
        message.number = i;
        if (msgsnd(msgid, &message, sizeof(message.number), 0) == -1) {
            perror("msgsnd");
            exit(1);
        }
        sleep(1); // opóźnienie dla demonstracji
    }

    return 0;
}

