#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>

#define MSG_KEY 1234

struct msgbuf {
    long mtype;
    int number;
};

int main() {
    int msgid;
    struct msgbuf message;

    msgid = msgget(MSG_KEY, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }

    while (1) {
        if (msgrcv(msgid, &message, sizeof(message.number), 1, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }
        for (int i = 0; i < message.number; i++) {
            printf("%d ", message.number);
        }
        printf("\n");
    }

    return 0;
}

