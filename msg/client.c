#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main()
{
    int msqid = 0;
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
   
    
    size_t size = sizeof(struct mybuf) - sizeof(long);
    
    printf("print two numbers\n");
    scanf("%lg%lg", &(myMsgBuf.a), &(myMsgBuf.b));
    int pid = 0;
    myMsgBuf.pid = pid = getpid();
    
    if(msgsnd(msqid, (struct msgbuf*) &myMsgBuf, size, 0) < 0)
    {
        printf("Can't send a message, errno = %d\n", errno);
        msgctl(msqid, IPC_RMID, (struct msqid_ds*)NULL);
        exit(-1);
    }
   
    if(msgrcv(msqid, (struct msgbuf*) &myMsgBuf, size, pid, 0) < 0)
    {
        printf("Can't receive a message, errno = %d\n", errno);
        msgctl(msqid, IPC_RMID, (struct msqid_ds*)NULL);
        exit(-1);
    }        
        
    printf("a*b = %lg\n", myMsgBuf.a);
   
        
    return 0;
}
