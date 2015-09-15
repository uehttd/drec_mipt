#include <string.h>
#include <stdio.h>


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
  /*
   * Не совсем понятны ваши пляски с двумя строчками. Что именно этим хотили сказать?
   * Сохранили и исходную строку и модифицированную. Ок, пусть так.
   */
	memcpy(tokenData, string, sizeof(char) * DEF_STRING_LENGTH); //copying the string as it will be modified by the Split function
	
	char** tokens = NULL;
	int tokensCount = 0;
	
	Split(tokenData, delimeters, &tokens, &tokensCount);
	
	int i = 0;
	for(i = 0; i < tokensCount; i++)
	{
		printf("%s\n", tokens[i]);
	}
	
	/*
   * FIXIT:
   * 1. не забывайте сами освобождать память за собой
   * 2. компилятор выдаёт предупреждения, т.к. вы не подключили нужный заголовочный файл для calloc. 
   */
	
	return 0;
}

int Split(char* string, char* delimeters, char*** tokens, int* tokensCount)
{
	
  /*
   * FIXIT:
   * Не круто когда функция выделяет какой-то кусок памяти, но сама не чистит за собой. Представьте, что вы хотите оформить вашу ф-ю в виде библиотеки, так что
   * пользователь не увидит ваш код. Он видит только сигнатуру ф-и. Он каким-то образом должен догадаться, что память вы сами за него выделили, а освободить должен именно он.
   * Если вы сделаете динамическую библиотеку, то такое точно сделать не получится (в разных местах выделить и освободить память).
   * Надо вынести выделение памяти в main.
   */
	*tokens = (char**)calloc(DEF_TOKENS_COUNT, sizeof(char*));
	
	int maxTokensCount = DEF_TOKENS_COUNT;
	
	*tokensCount = 0;
	/*
   * мне кажется, что *(*tokens + *tokensCount) куда менее понятно, чем (*tokens)[*tokensCount]
   */
	*(*tokens + *tokensCount) = strtok(string, delimeters);
	while(*(*tokens + *tokensCount) != NULL)
	{
		(*tokensCount)++;
		if(*tokensCount >= maxTokensCount)
		{
			*tokens = (char**)realloc(*tokens, sizeof(char*) * maxTokensCount * 2);
			maxTokensCount *= 2;
		}
		
		
		*(*tokens + *tokensCount) = strtok(NULL, delimeters);
	}
	
	return *tokensCount;
}
