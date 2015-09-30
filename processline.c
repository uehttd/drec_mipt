#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//----------------------------------------------------------------
#define N 5

int main()
{
    srand(time(NULL));

    int pid, i, status;
    for(i = 0; i < N; i++)
    {
        pid = fork();
        sleep(rand()%10);
        if(pid == -1)
        {
            printf("ERROR!\n");
        }
        else if(pid == 0)
        {
            continue;
        }
        break;
    }
    printf("pid = %d, ppid = %d\n" , getpid(), getppid());
    pid = wait(&status);
    printf("Process %d exited with status %d and returned %d\n", pid, status & 255, (status & 65280) >> 8);
    return getpid();
}
