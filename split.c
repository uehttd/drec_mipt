#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define DEF_STRING_LENGTH 100
#define DEF_TOKENS_COUNT 10

int Split(char* string, char* delimiters, char*** tokens, int* tokensCount);

int main()
{
	char string[DEF_STRING_LENGTH] = "This is a string."; 
	printf("Enter the string:\n");
	fgets(&string[0], DEF_STRING_LENGTH, stdin); 
	
	char delimeters[] = " ,.-:?!\"\n\t;";
	
	char tokenData[DEF_STRING_LENGTH];
	memcpy(tokenData, string, sizeof(char) * DEF_STRING_LENGTH); //copying the string as it will be modified by the Split function
	
	char** tokens = (char**)calloc(DEF_TOKENS_COUNT, sizeof(char*));
	int tokensCount = 0;
	
	Split(tokenData, delimeters, &tokens, &tokensCount);
	
	int i = 0;
	for(i = 0; i < tokensCount; i++)
	{
		printf("%s\n", tokens[i]);
	}
  free(tokens);
	
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
