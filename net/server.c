#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_CLIENTS_COUNT 50
#define MAX_MESSAGE_LENGTH 1000
#define DEF_PORT 51000

/*
 * Выглядит правдоподобно, но я не могу проверить в данный момент на двух компьютерах. А на одном в данный момент это сделать нельзя.
 * Просмотрите мои комментарии к коду обеих программ.
 */

/*
 * 1. В этом code style'е названия структур с заглавной буквы пишут.
 * 2. для 255 нужна константа
 * 3. Обычно переменные называют так в данном стиле: cliCount, cliAddr, т.е. первое слово со строчной, остальные с заглавной.
 */
struct client
{
    struct sockaddr_in addr;
    char nickname[255];
};

int IsAClient(struct sockaddr_in current, struct client* clients, int clicount);
void AddClient(struct sockaddr_in newClient, struct client* clients, int* clicount);
void SendToAllCientsButOne(int sockfd, char* message, struct client* clients, int sender, int clicount);
void ListClients(struct client* clients, int clicount);

int main()
{
  int sockfd;
  int clilen, n;
  char line[MAX_MESSAGE_LENGTH];
  struct sockaddr_in servaddr, cliaddr;
  struct client clients[MAX_CLIENTS_COUNT];
  int clicount = 0;
  
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(DEF_PORT);
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  
  if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
  {
    perror(NULL);
    exit(1);
  } else
  {
    printf("Sock fd:%d\n", sockfd);
  }
  
  if (bind(sockfd, (struct sockaddr*) &servaddr, 
    sizeof(servaddr)) < 0)
  {
    perror(NULL);
    close(sockfd);
    exit(1);
  }
  
  while (1)
  {
    clilen = sizeof(cliaddr);
    if ((n = recvfrom(sockfd, line, MAX_MESSAGE_LENGTH - 1, 0, 
      (struct sockaddr*)&cliaddr, &clilen)) < 0)
    {
      perror(NULL);
      close(sockfd);
      exit(1);
    }
    else
    {
        int senderNum = -1;
        if((senderNum = IsAClient(cliaddr, clients, clicount)) >= 0)
        {
            printf("client no %d sent a message %s\n", senderNum, line);
            SendToAllCientsButOne(sockfd, line, clients, senderNum, clicount);
        }
        else
        {
            AddClient(cliaddr, clients, &clicount);
            printf("someones connected, %ul\n", cliaddr.sin_addr.s_addr);
            ListClients(clients, clicount);
        }
    }
  }
  return 0;
}

int IsAClient(struct sockaddr_in current, struct client* clients, int clicount)
{
    int i = 0;
    for(i = 0; i < clicount; i++)
    {
	/*
	 * Проверяйте заодно совпадение порта: это даст возможность запускать несколько клиентов и сервер на одной машине, что облегчит процесс разработки и тестирования
	 */
        if(clients[i].addr.sin_addr.s_addr == current.sin_addr.s_addr)
        {
            return i;
        }
    }
    return -1;
}

void AddClient(struct sockaddr_in newClient, struct client* clients, int* clicount)
{
    bzero((void*)&clients[*clicount].addr, sizeof(clients[*clicount].addr)); 
    clients[*clicount].addr.sin_family = newClient.sin_family;
    clients[*clicount].addr.sin_port = newClient.sin_port;
    clients[*clicount].addr.sin_addr.s_addr = newClient.sin_addr.s_addr;
    
    (*clicount)++;
}

void SendToAllCientsButOne(int sockfd, char* message, struct client* clients, int sender, int clicount)
{
    int i = 0;
    size_t clilen = sizeof(clients[0].addr);
    for(i = 0; i < clicount; i++)
    {
        if(i == sender)
        {
            continue;
        }
        if (sendto(sockfd, message, strlen(message) + 1, 0,
          (struct sockaddr*)&clients[i].addr, clilen) < 0)
        {
          perror(NULL);
          close(sockfd);
          exit(1);
        }
    }
}

void ListClients(struct client* clients, int clicount)
{
    int i = 0;
    for(i = 0; i < clicount; i++)
    {
        printf("%d\t %ul\n", i, clients[i].addr.sin_addr.s_addr);
    }
}
