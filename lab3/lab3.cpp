#include <stdio.h> //printf
#include <errno.h> //errno
#include <signal.h> //signal
#include <unistd.h> //pause getpid

void signal_handler(int signum);


int main()
{
	struct sigaction act;
	sigset_t sig_set;
	pid_t pid = getpid();//для kill -- указывать id программы
	printf("Id текущего процесса: %i\n", pid);
	printf("Для завершения программы нажмите CTRL+C\n");

	signal(SIGUSR1, signal_handler);
	signal(SIGINT, signal_handler);
		
	sigemptyset(&sig_set);
	sigaddset(&sig_set, SIGUSR1);

	sigprocmask(SIG_BLOCK, &sig_set, NULL);

	printf("SIGUSR1 masked\n");

	raise(SIGUSR1);

	printf("SIGUSR1 sent\n");

	while (1) {
		pause();
	}
	return 0;
}

void signal_handler(int signum) 
{
	sigset_t sig_set;

	if (signum == SIGINT) 
	{//обработчик сигнала SIGINT
		printf("Это обработчик сигнала SIGINT\n");

		sigemptyset(&sig_set);
		sigaddset(&sig_set, SIGUSR1);

		sigprocmask(SIG_UNBLOCK, &sig_set, NULL);
	}
	if (signum == SIGUSR1)
	{
		printf("Это обработчик сигнала SIGUSR1\n");
	}
}

// void signal_handler(int signum);
// 
// 
// int main() 
// {
// 	pid_t pid = getpid();//для kill -- указывать id программы
// 	printf("Id текущего процесса: %i\n", pid);
// 	printf("Для завершения программы нажмите CTRL+C\n");
// 
// 	signal(SIGUSR1, SIG_IGN);
// 
// 	printf("SIGUSR1 masked\n");
// 
// 	raise(SIGUSR1);
// 
// 	printf("SIGUSR1 sent\n");
// 
// 	signal(SIGINT, signal_handler);
// 
// 	while (1) {
// 		pause();
// 	}
// 	return 0;
// }
// 
// void signal_handler(int signum) {
// 	if (signum == SIGINT) {//обработчик сигнала SIGINT
// 		printf("Это обработчик сигнала SIGINT\n");
// 
// 		signal(SIGUSR1, signal_handler);
// 	}
// 	if (signum == SIGUSR1)
// 	{
// 		printf("Это обработчик сигнала SIGUSR1\n");
// 	}
// }