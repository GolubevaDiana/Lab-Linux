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
#include <sys/sem.h>
#include <time.h>

void my_sem_wait(int sem_id)
{
	struct sembuf sem_oper = { 0 };

	sem_oper.sem_op = 1;
	semop(sem_id, &sem_oper, 1);
}

void my_sem_post(int sem_id)
{
	struct sembuf sem_oper = { 0 };

	sem_oper.sem_op = -1;
	semop(sem_id, &sem_oper, 1);
}

int main(int argc, char *argv[])
{
	bool use_default_file = false;

	if (argc < 2)
	{
		use_default_file = true;
	}

	key_t sem_key = ftok(argv[0], 12345);
	if (sem_key == -1) {
		printf("Возникла ошибка при получении ключа. Ошибка: %d\n", errno);
		return 1;
	}

	int sem_id = semget(sem_key, 1, IPC_CREAT | 0666);
	if (sem_id == -1)
	{
		printf("Не удалось создать семафор. Ошибка: %d\n", errno);
		return 2;
	}

	semctl(sem_id, 0, SETVAL, 1);

	FILE * fp = fopen(use_default_file ? "textfile" : argv[1], "r");
	if (!fp)
	{
		printf("Не удалось открыть файл. Ошибка: %d\n", errno);
		return 3;
	}

	char buf[100];

	srand(time(NULL));

	my_sem_wait(sem_id);

	while (fgets(buf, 100, fp) != NULL)
	{
		printf("%s", buf);

		// Сон на 1-3 с (для получения интервала берется старшие биты от rand)
		sleep(1 + (int)(3.0 * rand() / (RAND_MAX + 1.0)));

		my_sem_post(sem_id);

		my_sem_wait(sem_id);
	}

	my_sem_post(sem_id);

	return 0;
}