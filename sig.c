#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

/*
 * Если программа пишет предупреждения, то это не здорово. 
 * Иногда за предупреждениями кроется ошибка, например, работа с неинициализированной переменной.
 * В данном случае вы забыли подключить заголовочный файл string.h, но всё же всегда обращайте на это внимание.
 */

#define STRING_LENGTH 255

pid_t pid;
int bitsRead;
int bytesRead;
char message[STRING_LENGTH];

void my_handler(int nsig)
{
    if(nsig == SIGUSR1) //0
    {
    }
    if(nsig == SIGUSR2) //1
    {
        message[bytesRead] = message[bytesRead] ^ (1 << bitsRead);
    }
    bitsRead++;
    //printf("%d %d %d %u\n", getpid(), bytesRead, bitsRead, message[bytesRead]);
    if(bitsRead == 8)
    {
        bitsRead = 0;
        bytesRead++;
        if(message[bytesRead - 1] == 0)
        {
            printf("%s\n", message);
            message[0] = 0;
            bytesRead = 0;
        }
    }
}

void SendAByte(char byte, pid_t pid);

int main(void)
{
    pid_t pid = fork();
    bytesRead = bitsRead = 0;
    
    if(pid == 0)
    {
        signal(SIGUSR1, my_handler);
        signal(SIGUSR2, my_handler);
        while(1);
    }
    else
    {
        while(1)
        {
            fgets(message, STRING_LENGTH, stdin);
            int l = strlen(message);
            int i = 0;
            for(i = 0; i < l + 1; i++)
            {
                SendAByte(message[i], pid);
            }
        }
    }

    return 0;
}

void SendAByte(char byte, pid_t pid)
{
    char c = byte;
    int i = 0;
    for(i = 0; i < 8; i++)
    {
        //printf("%d %d %u\n", getpid(), i, c);
        /*
	 * Есть удобная штука, которая называется тернарный оператор:
	 * код ниже можно написать так
	 * kill(pid, (c & 1) ? SIGUSR2 : SIGUSR1);
	 * Коротко и понятно.
	 */
        if(c & 1)
        {
            kill(pid, SIGUSR2);
        }
        else
        {
            kill(pid, SIGUSR1);
        }
        c = c >> 1;
        sleep(1); //making a pause for receiver to process the signal
	/*
	 * Можно было бы задействовать ещё один сигнал (любой, например, SIGINT), который отправлялся бы от получателя битов к отправителю,
	 * сигнализирующий о готовности принимать следующую информацию. Вы же всегда надеетесь, что за секунду принимающая
	 * сторона управится. Как-то совсем не круто: сейчас скорость передачи 1 bit/sec...
	 * Поправьте это.
	 */
    }
}
