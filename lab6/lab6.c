#include <stdio.h>
#include <string.h>
#include <stdlib.h> //atoi
#include <sys/types.h>
#include <sys/ipc.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/msg.h>
#include <unistd.h>

#define MESLEN 150

void signal_handler(int);

bool finish = false;
int mesId;

struct msgbuf {
	long mtype;
	char mtext[1];
};

int main(int argc, char *argv[])
{
	bool master = false;
	struct msgbuf *mBuf;


	if (argc < 2)
	{
		master = true;	  
	}

	key_t queueKey = ftok(argv[0], 12345);
	if (queueKey == -1) {
		printf("Возникла ошибка при получении ключа. Ошибка: %d\n", errno);
		return 1;
	}

	mBuf = (struct msgbuf*)malloc(sizeof(long) + MESLEN);
	if (!mBuf)
	{
		return 2;  
	}

	if (master)
	{
  		mesId = msgget(queueKey,  IPC_CREAT | 0660);
		if (mesId == -1) {
			printf("Возникла ошибка при создании очереди. Ошибка: %i\n", errno);
			return 3;
		}
		else {
			printf("Очередь сообщений создана.\n");
		}

		signal(SIGINT, signal_handler);

		while (finish != true) 
		{
			//получаем информацию
			if (msgrcv(mesId, mBuf, MESLEN-1, 0, 0) == -1) 
			{
				if (errno != EIDRM && errno != EINTR) 
				{
					printf("Возникла ошибка при получении информации из очереди сообщений. Ошибка: %d\n", errno);
				}
				continue;
			}
			//выводим информацию
			printf("%ld: %s", mBuf->mtype, mBuf->mtext);
			
			
		}
	}
	else
	{
		FILE * fp;

		char * line = NULL;
		pid_t pid = getpid();

		fp = fopen(argv[1], "r");
		if (!fp)
		{
			printf("Возникла ошибка при открытии указанного файла. Ошибка: %d\n", errno);
			return 3;	
		}

		mesId = msgget(queueKey, 0);//Пытаемся подключиться к очереди сообщений
		if (mesId == -1) 
		{
			printf("Возникла ошибка при подключении к очереди. Ошибка:%i\n", errno);
			return 4;
		}

		mBuf->mtype = (long)pid;

		while (fgets(mBuf->mtext, MESLEN - sizeof(long), fp) != NULL)
		{
			printf("Пересылаю сообщение от процесса с порядковым номером: %d\n", pid);

			if (msgsnd(mesId, mBuf, strlen(mBuf->mtext) + sizeof(long), 0) == -1) 
			{
				if (errno != EIDRM && errno != EINTR) 
				{
					printf("Возникла ошибка при отправки сообщения. Ошибка: %d\n", errno);
				}	
				free(mBuf);
				return 8;
			}

                        //sleep(1);
		}

		if (line)
		{
			free(line);
		}

		printf("Файл успешно отправлен\n");

	}

	free(mBuf);
	return 0;
	
}

void signal_handler(int num) {
	if (num == SIGINT) {
		printf("\nЗавершение!\n");
		if (msgctl(mesId, IPC_RMID, NULL) == -1) {//Удаляем очередь сообщений
			printf("Возникла ошибка при удалении очереди. Ошибка:%i\n", errno);
		}
		finish = true;
	}
}
