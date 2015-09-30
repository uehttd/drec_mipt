#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define DEF_COMMAND_LENGTH 255
#define DEF_TOKENS_COUNT 5

typedef struct
{ 
    char* rawData;
    int startTime;
    char** argv;
    char* path;
    pid_t pid;
} CommandData;

int Split(char* string, char* delimeters, char*** tokens, int* tokensCount);
int CompareCommands(const void* c1, const void* c2);
void   ScanProcesses(int numberOfCommands, CommandData* commands);
void LaunchProcesses(int numberOfCommands, CommandData* commands);

int main(int argc, char *argv[], char *envp[])
{
    int numberOfCommands = 0;
    scanf("%d\n", &numberOfCommands);
    
    CommandData* commands = (CommandData*)calloc(numberOfCommands, sizeof(CommandData));
	
    ScanProcesses(numberOfCommands, commands);        
    qsort(commands, numberOfCommands, sizeof(CommandData), CompareCommands);
    LaunchProcesses(numberOfCommands, commands);        
    
    free(commands);
    
    return 0;
}

int Split(char* string, char* delimeters, char*** tokens, int* tokensCount)
{
	int maxTokensCount = DEF_TOKENS_COUNT;
	
	*tokensCount = 0;

	(*tokens)[*tokensCount] = strtok(string, delimeters);
	while((*tokens)[*tokensCount] != NULL)
	{
		(*tokensCount)++;
		if(*tokensCount >= maxTokensCount)
		{
			*tokens = (char**)realloc(*tokens, sizeof(char*) * maxTokensCount * 2);
			maxTokensCount *= 2;
		}
		(*tokens)[*tokensCount] = strtok(NULL, delimeters);
	}
	
	return *tokensCount;
}

int CompareCommands(const void* c1, const void* c2)
{
    return ((CommandData*)c1)->startTime - ((CommandData*)c2)->startTime;
}

void ScanProcesses(int numberOfCommands, CommandData* commands)
{
    int i = 0;
    for(i = 0; i < numberOfCommands; i++)
    {
        int tokensCount = 0;
        char** tokens = (char**)calloc(DEF_TOKENS_COUNT, sizeof(char*));
        commands[i].rawData = (char*)calloc(DEF_COMMAND_LENGTH, sizeof(char));
        fgets(commands[i].rawData, DEF_COMMAND_LENGTH, stdin);
        Split(commands[i].rawData, " \n", &tokens, &tokensCount);
        
        commands[i].startTime = atoi(tokens[tokensCount - 1]);
        tokens[tokensCount - 1] = (char*)NULL;
        commands[i].argv = tokens;
        commands[i].path = tokens[0];
    }
}

void LaunchProcesses(int numberOfCommands, CommandData* commands)
{
    int i = 0;
    int currTime = 0;
    for(i = 0; i < numberOfCommands; i++)
    {
        sleep(commands[i].startTime - currTime);
        currTime = commands[i].startTime;
        
        commands[i].pid = fork(); 
        if(commands[i].pid == -1)
        {
            fprintf(stderr, "ERROR CREATING A PROCESS!\n");
        }
        else if(commands[i].pid == 0)
        {
            execvp(commands[i].path, commands[i].argv);
            fprintf(stderr, "Couldn't execute the process %s!\n", commands[i].path);
            exit(1);
        }
        
        free(commands[i].rawData);
    }
    
    pid_t pid = 0;    
    int status = 0;
    while((pid = wait(&status)) != -1)
    {
        printf("Process %d exited with status %d and returned %d\n", pid, status & 255, (status & 65280) >> 8);
    }
}
