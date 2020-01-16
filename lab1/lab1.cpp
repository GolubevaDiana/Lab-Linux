#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>//getuid geteuid
#include <pwd.h>//getpwuid
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>//bool: true, false
#include <fcntl.h>//open
#include <errno.h>//errno

int main(int argc, char * argv[])
{
	if (argc != 2) 
	{//на входе название программы в argv[0] и название каталога в argv[1]
		printf("На вход программы должено поступать название каталога, в котором будет произведён поиск.\n");
		return 1;
	}

	uid_t eUserID = geteuid();//получаем id пользователя этого процесса

	struct passwd *userInfo = getpwuid(eUserID);
	if (userInfo == NULL)
	{
		printf("Возникла проблема при получении информации об пользователе");
		return 2;
	}

	DIR *directory = opendir(argv[1]);//открываем каталог
	if (directory == NULL) 
	{
		printf("Возникла проблема при открытии каталога\n");
		return 3;
	}

	struct dirent *dirFile;
	char *fileName;
	while (1) 
	{//Обработка файлов
		dirFile = readdir(directory);
		if (dirFile == NULL)
			break;
		else 
		{
			if (dirFile->d_type == DT_REG) 
			{//если обычный файл
		
				fileName = (char *)malloc(strlen(argv[1]) + strlen(dirFile->d_name) + 2);//начало формирования абсолютного имени для функции stat
				strcpy(fileName, argv[1]);

				if (*(argv[1] + strlen(argv[1]) - 1) != '/')
				{
					strcat(fileName, "/");
				}

				strcat(fileName, dirFile->d_name);//завершение формирования абсолютного имени

				if (chmod(fileName, S_IXGRP) == -1)
				{
					printf("Возникла проблема при записи в файл %i.\n", errno);
				}
				
				free(fileName);//освобождение выделенной под абсолютное имя памяти
			}
		}
	}

	closedir(directory);//закрываем каталог
	
	return 0;
}