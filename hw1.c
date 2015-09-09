#include <stdio.h>
#include <time.h>

int main()
{
    int m, n, i, j;
    int* data;

    printf("print m, n\n");
    scanf("%d %d", &m, &n);
    
    data = (int*)malloc(sizeof(int) * m * n);
    for(i = 0; i < m; i++)
    {
        for(j = 0; j < n; j++)
        {
            *(data + i * n + j) = rand()%200 - 100;
            printf("%d\t", *(data + i * n + j));
        }
        printf("\n");
    }
    printf("\n\n\n");
    
    for(j = 0; j < n; j++)
    {
        for(i = 0; i < m; i++)
        {
            printf("%d\t", *(data + i * n + j));
        }
        printf("\n");
    }
    return 0;
}
