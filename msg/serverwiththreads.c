#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

#define N 2

typedef struct
{    
    long type;
    double a;
    double b;
    long pid;
} mybuf;

int msqid, semid;
struct sembuf mysembuf;

void* ThreadFunc(void* arg)
{
    mysembuf.sem_flg = 0;
    mysembuf.sem_num = 0;
    mysembuf.sem_op = -1;
    if(semop(semid, &mysembuf, 1) < 0)
    {
        printf("Can`t wait for condition\n");
        semctl(semid, IPC_RMID, 0); 
        return NULL;
    }
    mybuf* myMsgBuf = (mybuf*)arg;
    
    printf("started computing for %ld\n", myMsgBuf->pid);
    sleep(10);
    myMsgBuf->a = myMsgBuf->a * myMsgBuf->b;
            
    myMsgBuf->type = myMsgBuf->pid;
    size_t size = sizeof(mybuf) - sizeof(long);
    if(msgsnd(msqid, (struct msgbuf*)myMsgBuf, size, 0) < 0)
    {
        printf("Can't send a message, errno = %d\n", errno);
        msgctl(msqid, IPC_RMID, (struct msqid_ds*)NULL);
        return NULL;
    }
            
    printf("finished computing for %ld\n", myMsgBuf->pid);
    free(myMsgBuf);
    mysembuf.sem_flg = 0;
    mysembuf.sem_num = 0;
    mysembuf.sem_op  = 1;
    if(semop(semid, &mysembuf, 1) < 0)
    {
        printf("Can`t wait for condition\n");
        semctl(semid, IPC_RMID, 0); 
        return NULL;
    }
    return NULL;    
}

int main()
{
    char pathname[] = "ex0.c";
    
    key_t key;
    
    struct mybuf
    {
        long type;
        double a;
        double b;
        long pid;
    } myMsgBuf;
        
    if((key = ftok(pathname, 0)) < 0)
    {
        printf("Couldn't generate key!\n");
        exit(-1);
    }
    
    if((msqid = msgget(key, 0666 | IPC_CREAT)) < 0)
    {
       printf("Can\'t get msqid\n");
       exit(-1);
    }
    
    if((semid = semget(key, 1, 0666 | IPC_CREAT)) < 0)
    {
       printf("Can\'t create semaphore\n");
       exit(-1);
    }
    
    mysembuf.sem_flg = 0;
    mysembuf.sem_num = 0;
    mysembuf.sem_op  = N;
    if(semop(semid, &mysembuf, 1) < 0) 
    {
        printf("Can`t wait for condition\n");
        semctl(semid, IPC_RMID, 0);         
        exit(-1);
    }
    
    size_t size = sizeof(mybuf) - sizeof(long);
    while(1)
    {
        mybuf* myMsgBuf = (mybuf*)calloc(1, sizeof(mybuf));
        if(msgrcv(msqid, (struct msgbuf*)myMsgBuf, size, 1, 0) < 0)
        {
            printf("Can't receive a message, errno = %d\n", errno);
            msgctl(msqid, IPC_RMID, (struct msqid_ds*)NULL);
            exit(-1);
        }        
        printf("received a message from %ld\n", myMsgBuf->pid);
        
        pthread_t thread_id;
        
        /*
         * Да, всё нормально. Вы выделяете под каждое сообщение свой кусок памяти в кучу, а в ф-ю pthread_create передаёте этот указатель по значению.
         */
        if(pthread_create(&thread_id, (pthread_attr_t*)NULL, ThreadFunc, (void*)myMsgBuf) < 0)
        {
            printf("Can't create a thread!\n");
            exit(-1);
        }
    }
    
    msgctl(msqid, IPC_RMID, (struct msqid_ds*)NULL);
    semctl(semid, IPC_RMID, 0); 
        
    return 0;
}      
        
