
// Программа-сервер, ожидающая запросы от клиентов. В ответ отсылает им сообщение с типом их процесса и некоторым сообщением.

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


int main()
{
int msqid; // IPC дескриптор для очереди сообщений

    char pathname[] = "ipc.c"; // Имя файла, использующееся для генерации ключа.

    key_t key; // IPC ключ

    int len; // Переменная для хранения длины отправляемого сообщения

    // Структура для хранения информации о сообщении-запросе
    struct reqsgbuf
    {
        long mtype;
        int pid;
    } requestbuf;

    // Структура для хранения информации о сообщении-ответе
    struct resmsgbuf
    {
        long mtype;
        char rtext[81];
    } responsebuf;

    // Генерация ключа очереди сообщений

    if ((key = ftok(pathname, 0)) < 0)
    {
        printf("Can\'t generate key\n");
        exit(-1);
    }

    // Определение ID для очереди сообщений
    if ((msqid = msgget(key, 0666 | IPC_CREAT)) < 0)
    {
        printf("Can\'t get msqid\n");
        exit(-1);
    }

    // Запускаем в бесконечном цикле прием сообщений
    while (1)
    {

        // Принимаем сообщения типа 1
        if ((msgrcv(msqid, &requestbuf, sizeof(int), 1, 0)) < 0)
        {
            printf("Can\'t receive message grom queue!\n");
            exit(-1);
        }
        
        // Заполнение структуры сообщения для ответа на запрос
        responsebuf.mtype = requestbuf.pid;
        strcpy(responsebuf.rtext, "This is a response from the server");
        len = strlen(responsebuf.rtext) + 1;

        // Отвечаем на запрос, посылая сообщение в ту же очередь сообщений, но с типом процесса клиента
        if (msgsnd(msqid, &responsebuf, len, 0) < 0)
        {
            printf("Can\'t send message to queue\n");
            msgctl(msqid, IPC_RMID, (struct msqid_ds *)NULL);
            exit(-1);
        }
    }

    return 0;
}