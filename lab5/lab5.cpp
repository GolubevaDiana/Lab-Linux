#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 

int main()
{
	int pipe1_tx[2];
	int pipe1_rx[2];
	int pipe2_tx[2];
	int pipe2_rx[2];
	pid_t p1 = 0;
	pid_t p2 = 0;
	unsigned int matrix1[100];
	unsigned int matrix2[100];
	unsigned int recvd_matrix1[100] = { 0 };
	unsigned int recvd_matrix2[100] = { 0 };
	unsigned int res_matrix1[100];
	int cur_pipe_rx[2];
	int cur_pipe_tx[2];
        bool child = false;

        for (int i = 0; i < 100; i++)
        {
            matrix1[i] = 2;
            matrix2[i] = 3;
        }

	if (pipe(pipe1_tx) == -1)
	{
		return 1;
	}
	if (pipe(pipe1_rx) == -1)
	{
		return 1;
	}
	if (pipe(pipe2_tx) == -1)
	{
		return 1;
	}
	if (pipe(pipe2_rx) == -1)
	{
		return 1;
	}

	p1 = fork();
	if (p1 < 0)
	{
		return 1;
	}
	// parent
	else if (p1 > 0)
	{
		p2 = fork();
		if (p2 < 0)
		{
			return 1;
		}
		// parent
		else if (p2 > 0)
		{
			close(pipe1_tx[0]);
			close(pipe2_tx[0]);

			close(pipe1_rx[1]);
			close(pipe2_rx[1]);

			write(pipe1_tx[1], matrix1, sizeof(matrix1));
			write(pipe2_tx[1], matrix1, sizeof(matrix1));
			write(pipe1_tx[1], matrix2, sizeof(matrix2));
			write(pipe2_tx[1], matrix2, sizeof(matrix2));

			close(pipe1_tx[1]);
			close(pipe2_tx[1]);

			waitpid(p1, NULL, 0);
			waitpid(p2, NULL, 0);

			read(pipe1_rx[0], recvd_matrix1, sizeof(recvd_matrix1));
			read(pipe2_rx[0], recvd_matrix2, sizeof(recvd_matrix2));

			close(pipe1_rx[0]);
			close(pipe2_rx[0]);

			if (!memcmp(recvd_matrix1, recvd_matrix2, sizeof(recvd_matrix1)))
			{
				for (int i = 0; i < 100; i++)
				{
					if (i % 10 == 0)
					{
						printf("\n");
					}

                                        printf("%d ", recvd_matrix1[i]);
				}
			}

                        printf("\n");
		}
		// child2
		else
		{
			cur_pipe_rx[0] = pipe2_tx[0];
			cur_pipe_rx[1] = pipe2_tx[1];
			cur_pipe_tx[0] = pipe2_rx[0];
			cur_pipe_tx[1] = pipe2_rx[1];
                        child = true;
		}
	}
	// child1
	else
	{
		cur_pipe_rx[0] = pipe1_tx[0];
		cur_pipe_rx[1] = pipe1_tx[1];
		cur_pipe_tx[0] = pipe1_rx[0];
		cur_pipe_tx[1] = pipe1_rx[1];
                child = true;
	}

	if (child)
	{
		close(cur_pipe_rx[1]);
		close(cur_pipe_tx[0]);

		read(cur_pipe_rx[0], recvd_matrix1, sizeof(recvd_matrix1));
		read(cur_pipe_rx[0], recvd_matrix2, sizeof(recvd_matrix2));

		close(cur_pipe_rx[0]);

		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				res_matrix1[i * 10 + j] = 0;
				for (int k = 0; k < 10; k++)
					res_matrix1[i * 10 + j] += recvd_matrix1[i * 10 + k] * recvd_matrix2[k * 10 + j];
			}
		}

		write(cur_pipe_tx[1], res_matrix1, sizeof(res_matrix1));
		close(cur_pipe_tx[1]);

		exit(0);
	}
}