#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#define START_CASH 1e5
#define STRING_LENGTH 1000
#define DEF_PORT 51002

struct OperationResult
{
    int resultCode; //0 - success, 1 - exceed limit, 2 - default
    double balance;
};

int main(int argc, char **argv)
{
    int sockfd;
    int n;
    int i;
    char sendline[STRING_LENGTH], recvline[STRING_LENGTH];
    struct sockaddr_in servaddr;
    
    if(argc != 2)
    {
        printf("usage: ./a.out <IP>\n");
        exit(1);
    }
    
    bzero(sendline, STRING_LENGTH);
    bzero(recvline, STRING_LENGTH);
    if((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror(NULL);
        exit(1);
    }
    
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(DEF_PORT);
    if(inet_aton(argv[1], &servaddr.sin_addr) == 0)
    {
        printf("Invalid IP address\n");
        close(sockfd);
        exit(1);
    }
    if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
        perror(NULL);
        close(sockfd);
        exit(1);
    }
    struct OperationResult or;
    double cash = START_CASH;
    while(1)
    {
        printf("operation -> ");
        fflush(stdin);
        fgets(sendline, STRING_LENGTH, stdin);
        double operation = atof(sendline);
        if(cash + operation < 0)
        {
            printf("Not enough cash\n");
            continue;
        }
        if((n = write(sockfd, sendline, strlen(sendline) + 1)) < 0)
        {
            perror("cant wrrite\n");
            close(sockfd);
            exit(1);
        }
        if((n = read(sockfd, (void*)&or, sizeof(or))) < 0)
        {
            perror("cant read\n");
            close(sockfd);
            exit(1);
        }
        
        switch(or.resultCode)
        {
        case 0:
            cash += operation;
            printf("Operation completed, balance: %lg, cash: %lg\n", or.balance, cash);
            break;
        case 1:
            printf("Completing the operation would exceed the credit limit, balance: %lg, cash: %lg\n", or.balance, cash);
            break;
        case 2:
            printf("Completing the operation would make the bank go bankrupt, balance: %lg, cash: %lg\n", or.balance, cash);
        }
    }
    close(sockfd);
    return 0;
}
