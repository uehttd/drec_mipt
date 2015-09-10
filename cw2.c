#include <string.h>
#include <stdio.h>
#define $ //printf("%s\n", mystr);

void GenerateString(int n, char* mystr);

int main()
{
	int n; int i;
	
	printf("input n\n");
	scanf("%d", &n);
	
	char* data = (char*)malloc(sizeof(char) * (1 << n));
	//printf("%d\n", (1 << n)); 
	
	for(i = 1; i <= n; i++)
	{
		GenerateString(i, data);
		printf("%s\n", data);
	}
	
	return 0;
}

void GenerateString(int n, char* mystr)
{
	int i = 1;
	mystr[0] = 'a';
	mystr[1] = 0;
	int len = 1;
	
	for(i = 1; i < n; i++)
	{
		mystr[len] = 'a' + i; $
		mystr[len+1] = 0; //printf(" !%s\n", mystr);
		
		memcpy((mystr + len + 1), mystr, len); 
		len = len * 2 + 1; $
		mystr[len] = 0; $ //printf("!!%s\n", mystr);
	}
}
