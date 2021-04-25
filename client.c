
/* Программа-клиент, отправляющая запросы к серверу в виде сообщений с типом 1 и информацией об идентификаторе своего процесса.
Получает ответ от сервера.
*/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    int msqid; /* IPC дескриптор для очереди сообщений */

    char pathname[] = "ipc.c"; /* Имя файла, использующееся для генерации ключа. */

    key_t key; /* IPC ключ */

    // Структура для хранения информации о сообщении-запросе
    struct mymsgbuf
    {
        long mtype;
        int pid;
    } requestbuf;

    // Структура для хранения информации о сообщении-ответе
    struct servermsgbuf
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

    // Заполнение структуры сообщения-запроса с полезной информацией, содержащей PID процесса
    requestbuf.mtype = 1;
    requestbuf.pid = getpid();

    // Отсылаем запрос
    if ((msgsnd(msqid, &requestbuf, sizeof(requestbuf.pid), 0)) < 0)
    {
        printf("Can\'t send message!\n");
        msgctl(msqid, IPC_RMID, NULL);
        exit(-1);
    }

    // Получаем ответ от сервера
    if ((msgrcv(msqid, &responsebuf, 81, requestbuf.pid, 0)) < 0)
        {
            printf("Can\'t receive message from the queue!\n");
            exit(-1);
        }

    printf("Response from server with type %ld and text: \'%s\'\n", responsebuf.mtype,  responsebuf.rtext);
    return 0; /* Исключительно для отсутствия warning'ов при компиляции. */
}