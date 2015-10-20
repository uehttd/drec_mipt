#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define N 10000000

double a[3] = {0, 0, 0};
int semid;
void* ThreadFunc(void* arg)
{
    struct sembuf mybuf;
    double i = 0;
    for(i = 0; i < N; i++)
    {
        mybuf.sem_flg = 0;
        mybuf.sem_num = 0;
        mybuf.sem_op  = -1;
        if (semop(semid , &mybuf , 1) < 0) 
        {
            printf("Can`t wait for condition\n");
            semctl(semid, IPC_RMID, 0);
            exit(-1);
        }
        
        a[0]++;
        a[*((int*)arg)]++;
        
        mybuf.sem_flg = 0;
        mybuf.sem_num = 0;
        mybuf.sem_op = 1;
        if(semop(semid, &mybuf, 1) < 0)
        {
            printf("Can't wait for condition %d\n", *((int*)arg));
            semctl(semid, IPC_RMID, 0); 
            exit(-1);
        }
    }
    
    return NULL;
}

int main()
{
    pthread_t threadID1, threadID2;  
    const int arg1 = 1, arg2 = 2;
    
    key_t key;
    char* pathname = "threads.c";
    
    if(key = ftok(pathname, 0) < 0)
    {
        printf("Can't generate key\n");
        exit(-1);
    }
    if ((semid = semget(key , 1 , 0666 | IPC_CREAT)) < 0) 
    {
        printf("Can`t get semid\n");
        exit(-1);
    }
    
    struct sembuf mybuf;
    mybuf.sem_flg = 0;
    mybuf.sem_num = 0;
    mybuf.sem_op  = 1;
    if (semop(semid , &mybuf , 1) < 0) 
    {
        printf("!Can`t wait for condition\n");
        exit(-1);
    }
    
    int result1 = pthread_create(&threadID1, (pthread_attr_t*)NULL, ThreadFunc, (void*)&arg1);
    int result2 = pthread_create(&threadID2, (pthread_attr_t*)NULL, ThreadFunc, (void*)&arg2);
    
    if(result1 || result2)
    {
        fprintf(stderr, "Couldn't create thread, res1 = %u, res2 = %u\n", result1, result2);
        exit(-1);
    }
    
    pthread_join(threadID1, (void**)NULL);
    pthread_join(threadID2, (void**)NULL);
    
    printf("a[0] = %lg\na[1] = %lg\na[0] + a[1] = %lg\na[2] = %lg\n", a[0], a[1], a[0] + a[1], a[2]);
    
    semctl(semid, IPC_RMID, 0);
    
    return 0;
}
