#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#define STRING_LENGTH 1000
#define START_SUM 100e9
#define CREDIT_LIMIT 100e6
#define DEF_PORT 51002

struct OperationResult
{
    int resultCode; //0 - success, 1 - exceed limit, 2 - default
    double balance;
};

double money;
int semid;

void* ThreadFunc(void* arg)
{
    int* newsockfd = (int*)arg;
    printf("new client, %d\n", *newsockfd);
    int n = 0;
    double balance = 0;
    char line[STRING_LENGTH] = "";
    struct OperationResult or;
    struct sembuf mysembuf;
    while((n = read(*newsockfd, (void*)line, STRING_LENGTH - 1)) > 0)
    {
        double operation = atof(line);        
        bzero(line, n);
        printf("client %d, line %s\n", *newsockfd, line); 
        mysembuf.sem_flg = 0;
        mysembuf.sem_num = 0;
        mysembuf.sem_op  = -1;
        printf("!\n");
        if(semop(semid, &mysembuf, 1) < 0)
        {
            printf("Can`t wait for condition\n");
            semctl(semid, IPC_RMID, 0); 
            return NULL;
        }
        
        
        if(balance - operation < -CREDIT_LIMIT)
        {
            or.resultCode = 1;
            or.balance = balance;
        }
        else if(money - operation < 0)
        {
            or.resultCode = 2;
            or.balance = balance;
        }
        else
        {
            or.resultCode = 0;
            balance -= operation;
            money -= operation;
        }
        or.balance = balance;
        printf("client %d, operation %lg, result %d, balance %lg, reserve %lg\n", *newsockfd, operation, or.resultCode, balance, money);
        
        mysembuf.sem_flg = 0;
        mysembuf.sem_num = 0;
        mysembuf.sem_op  = 1;
        if(semop(semid, &mysembuf, 1) < 0)
        {
            printf("Can`t wait for condition\n");
            semctl(semid, IPC_RMID, 0); 
            return NULL;
        }
        
        
        if((n = write(*newsockfd, &or, sizeof(or))) < 0)
        {
            perror(NULL);
            close(*newsockfd);
            exit(1);
        }
        
    }
        
    if(n < 0)
    {
        perror(NULL);
        close(*newsockfd);
        free(newsockfd);
        //close(sockfd);
        exit(1);
    }
    
    close(*newsockfd);
    free(newsockfd);
    return NULL;
}

int main(int argc, char **argv)
{
    int sockfd;
    int clilen;
    money = START_SUM;    
    key_t key;
    struct sockaddr_in servaddr, cliaddr;
    struct sembuf mysembuf;    
    if((key = ftok("server.c", 0)) < 0)
    {
        printf("Couldn't generate key!\n");
        exit(-1);
    }
    if((semid = semget(key, 1, 0666 | IPC_CREAT)) < 0)
    {
       printf("Can\'t create semaphore\n");
       exit(-1);
    }
    mysembuf.sem_flg = 0;
    mysembuf.sem_num = 0;
    mysembuf.sem_op  = 1;
    if(semop(semid, &mysembuf, 1) < 0)
    {
        printf("Can`t wait for condition\n");
        semctl(semid, IPC_RMID, 0); 
        return -1;
    }
    
    if((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror(NULL);
        exit(1);
    }
    
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(DEF_PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
        perror(NULL);
        close(sockfd);
        exit(-1);
    }
    
    if(listen(sockfd, 5) < 0)
    {
        perror(NULL);
        close(sockfd);
        exit(-1);
    }
    
    
    while(1)
    {
        clilen = sizeof(cliaddr);
        int* newsockfd = (int*)calloc(1, sizeof(int));
        if((*newsockfd = accept(sockfd, (struct sockaddr*)&cliaddr, &clilen)) < 0)
        {
            perror(NULL);
            close(sockfd);
            exit(-1);
        }
        
        pthread_t thread_id;
        if(pthread_create(&thread_id, (pthread_attr_t*)NULL, ThreadFunc, (void*)newsockfd) < 0)
        {
            printf("Can't create a thread!\n");
            exit(-1);
        }
    }  
                
    
    close(sockfd);
    return 0;
}
