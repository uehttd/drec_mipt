#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#define MESSAGE_LEGNTH 1000

void Send(int sockfd, struct sockaddr_in servaddr);
void Receive(int sockfd);
void Connect(int sockfd, struct sockaddr_in servaddr);

int main(int argc, char** argv)
{
  int sockfd;
  int n, len;
  char sendline[MESSAGE_LEGNTH], recvline[MESSAGE_LEGNTH];
  struct sockaddr_in servaddr, cliaddr;
  
  if (argc != 2)
  {
    printf("Usage: ./a.out <IP address>\n");
    exit(1);
  }
  if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
  {
    perror(NULL);
    exit(1);
  }
  
  bzero(&cliaddr, sizeof(cliaddr));
  cliaddr.sin_family = AF_INET;
  cliaddr.sin_port = htons(0);
  cliaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  
  if (bind(sockfd, (struct sockaddr*)&cliaddr, sizeof(cliaddr)) < 0)
  {
    perror(NULL);
    close(sockfd);
    exit(1);
  }
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(51000);
  if (inet_aton(argv[1], &servaddr.sin_addr) == 0)
  {
    printf("Invalid IP address\n");
    close(sockfd);
    exit(1);
  }
  
  Connect(sockfd, servaddr);
  
  pid_t pid = fork();
  if(pid == 0)
  {
    while(1)
    {
        Send(sockfd, servaddr);
    } 
  }
  else
  {
    while(1)
    {
      Receive(sockfd);
   }
  }
  close(sockfd);
  return 0;
}

void Send(int sockfd, struct sockaddr_in servaddr)
{   
    char sendLine[MESSAGE_LEGNTH] = "";
    fgets(sendLine, MESSAGE_LEGNTH, stdin);
    if(sendto(sockfd, sendLine, strlen(sendLine) + 1, 0,
       (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
        perror(NULL);
        close(sockfd);
        exit(1);
    }
}

void Receive(int sockfd)
{
    char recvLine[MESSAGE_LEGNTH] = "";
    if ((recvfrom(sockfd, recvLine, MESSAGE_LEGNTH, 0, (struct sockaddr*) NULL, NULL)) < 0)
    {
        perror(NULL);
        close(sockfd);
        exit(1);
    }
    printf("%s", recvLine);
}

void Connect(int sockfd, struct sockaddr_in servaddr)
{
    if(sendto(sockfd, NULL, 0, 0,
        (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
        perror(NULL);
        close(sockfd);
        exit(1);
    }
}
