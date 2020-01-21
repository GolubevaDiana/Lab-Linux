#include <stdio.h>
#include <stdbool.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

double fact(double N)
{
       if(N < 0) // если пользователь ввел отрицательное число
            return 0; // возвращаем ноль
       if (N == 0) // если пользователь ввел ноль,
            return 1; // возвращаем факториал от нуля
       else // Во всех остальных случаях
            return N * fact(N - 1); // делаем рекурсию.
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("На вход программы не подано числовое значение для вычисления факториала\n");
		return 1;
	}

	// В аргументе находится факториал, который необходимо вычислить
	errno = 0;
	double fact_num = strtod(argv[1], NULL);
	if (errno)
	{
	  	printf("На вход программы подано некорректное числовое значение\n");
		return 2;
	}

	struct itimerval it1;
	it1.it_value.tv_sec = 365 * 24 * 60 * 60;// дни * часы * минуты * секунды
	it1.it_value.tv_usec = 0;
	it1.it_interval.tv_sec = 0;
	it1.it_interval.tv_usec = 0;
	if (setitimer(ITIMER_REAL, &it1, NULL) == -1 || setitimer(ITIMER_VIRTUAL, &it1, NULL) == -1) {//Установка таймеров
		printf("Возникла ошибка при установке таймера. Ошибка: %i\n", errno);
		return 3;
	}

        double fact_res;

        // Вычисляем факториал много раз, чтобы увидеть изменение интервала
        for (int i = 0; i < 2000000; i++)
        {
            fact_res = fact(fact_num);
        }

	struct itimerval it11 = {0}, it12 = {0};
	if (getitimer(ITIMER_VIRTUAL, &it11) == -1 || getitimer(ITIMER_REAL, &it12) == -1) {
		printf("Произошла ошибка при получении значения");
		return 4;
	}

	//рассчёт времени в режиме задачи и в режиме ядра и вывод значений
	double setTime = 365.0 * 24.0 * 60.0 * 60.0 * 1000000.0;
	double virtualTime = setTime - ((double)it11.it_value.tv_sec * 1000000.0 + (double)it11.it_value.tv_usec);
	double realTime = setTime - ((double)it12.it_value.tv_sec * 1000000.0 + (double)it12.it_value.tv_usec);

	printf("\nРезультат вычисления: %e\n", fact_res);
	printf("\nАбсолютное время выполнения: %f мс",realTime/1000);
	printf("\nОтносительное время выполнения: %f мc\n",virtualTime/1000);

	return 0;
}

