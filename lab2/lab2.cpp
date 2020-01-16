#include <stdio.h>
#include <stdlib.h>//system
#include <unistd.h>//pause
#include <errno.h>//errno
#include <time.h>//clock_gettime
#include <string.h>//strcpy, strcat, strstr

int main(int argc, char * argv[])
{
	char *line = NULL;
	size_t size;
	int res;

	if (system(NULL) != 0) {
		printf("System shell available.\n");
	}
	else {
		printf("System shell unavailable.\n");
		return 1;
	}

	for (;;)
	{
		printf("> ");

		if (getline(&line, &size, stdin) == -1) 
		{
			printf("Error getline\n");
			break;
		}

		if (!strcmp(line, "exit\n"))
		{
			break;
		}

		res = system(line);
		// При ошибках возвращается значение - 1, иначе возвращается статус выполнения команды.
		if (res == -1)
		{
			printf("\nSystem error = %i\n", errno);
		}
		else
		{
			// Таким образом, код выхода из команды будет равен WEXITSTATUS(status).
			// Этот макрос можно использовать, только если WIFEXITED вернул ненулевое значение.
			if (WIFEXITED(res))
			{
				printf("\nReturn code = 0\n");
			}
			else
			{
				printf("\nReturn code = %c\n", WEXITSTATUS(res));
			}
		}

		printf("\n");
	}

	return 0;
}