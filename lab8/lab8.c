
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
#include <sys/wait.h>

FILE * logfile;
pid_t main_pid;
int sem_id;

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

void signal_handler(int num) {
     if (num == SIGCHLD)
     {
        time_t cur_time;
	struct tm * cur_datatime;
        pid_t pid;

        while ((pid = waitpid(-1, NULL, WNOHANG)) != 0)
        {
            if (pid == -1)
            {
               break;
            }

            time(&cur_time);
	    cur_datatime = localtime(&cur_time);

	    my_sem_wait(sem_id);

	    fprintf(logfile, "%02d.%02d.%4d %02d-%02d-%02d: Завершение процесса с PID=%d\n",
			cur_datatime->tm_mday,
			cur_datatime->tm_mon + 1,
			cur_datatime->tm_year + 1900,
			cur_datatime->tm_hour,
			cur_datatime->tm_min,
			cur_datatime->tm_sec,
			pid);

	    my_sem_post(sem_id);
        }

        fflush(logfile);
     }
}

void my_sleep(int sec)
{
     int rem = sec;

     do
     {
         rem = sleep(rem);

     } while (rem);
}

int main(int argc, char *argv[])
{
	int res;
	time_t cur_time;
	struct tm * cur_datatime;

	logfile = fopen("lab8.log", "w");
	if (!logfile)
	{
		printf("Возникла ошибка при создании лога (%d)\n", errno);
		return 1;
	}

	key_t sem_key = ftok(argv[0], 12345);
	if (sem_key == -1) {

		fprintf(logfile, "Возникла ошибка при получении ключа. Ошибка: %d\n", errno);
		return 1;
	}

	sem_id = semget(sem_key, 1, IPC_CREAT | 0666);
	if (sem_id == -1)
	{
		fprintf(logfile, "Не удалось создать семафор. Ошибка: %d\n", errno);
		return 2;
	}

	semctl(sem_id, 0, SETVAL, 1);

	if (daemon(0, 0) == -1)
	{
		fprintf(logfile, "Возникла ошибка при переходе в фоновый режим (%d)\n", errno);
		return 2;
	}

        fprintf(logfile, "Демон успешно запущен (PID=%d)\n", getpid());
        fflush(logfile);

        signal(SIGCHLD, signal_handler);//установка обработчика сигнала

	for (;;)
	{
		res = fork();
		if (res == -1)
		{
			fprintf(logfile, "Возникла ошибка при создании потомка\n");
			my_sleep(10);
		}
		else if (res == 0)
		{
			// Потомок
                        srand(time(NULL));
			// Сон на 5-25 с (для получения интервала берется старшие биты от rand)
			my_sleep(5 + (int)(25.0 * rand() / (RAND_MAX + 1.0)));

			exit(0);
		}
		else
		{
			// Родитель
			time(&cur_time);
			cur_datatime = localtime(&cur_time);

			my_sem_wait(sem_id);

			fprintf(logfile, "%02d.%02d.%4d %02d-%02d-%02d: Запуск процесса с PID=%d\n",
				cur_datatime->tm_mday,
				cur_datatime->tm_mon + 1,
				cur_datatime->tm_year + 1900,
				cur_datatime->tm_hour,
				cur_datatime->tm_min,
				cur_datatime->tm_sec,
				res);
                        fflush(logfile);

			my_sem_post(sem_id);

			my_sleep(10);
		}
	}

	return 0;
}