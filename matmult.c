#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 3
#define M 3
#define ELEM(MATR, i, j) *(MATR + i * M + j)

void* multipl(void* arg);
void PrintMatrix(int* data, int size);

int* A;
int* B;
int* C;

int main()
{
    A = (int*)calloc(M * M, sizeof(int));
    B = (int*)calloc(M * M, sizeof(int));
    C = (int*)calloc(M * M, sizeof(int));
    
    
    srand(time(NULL));
    int i = 0, j = 0;
    for(i = 0; i < M; i++)
    {
        for(j = 0; j < M; j++)
        {
            ELEM(A, i, j) = rand() % 20 - 10;
            ELEM(B, i, j) = rand() % 20 - 10;
        }
    }
    
    PrintMatrix(A, M);
    printf("\n");
    PrintMatrix(B, M);
    
    
    pthread_t threadIDs[N];
    int args[N] = {};
    unsigned int startTime = clock();
    for(i = 0; i < N; i++)
    {
        args[i] = i;
        int result = pthread_create(threadIDs + i, (pthread_attr_t*)NULL, multipl, (void*)(args + i));
        if(result)
        {
            fprintf(stderr, "Couldn't create process no %d, err %d\n", i, result);
            exit(-1);
        }
    }
    
    for(i = 0; i < N; i++)
    {
        pthread_join(threadIDs[i], (void**)NULL);
    }
    
    printf("Done! Clocks elapsed: %u\n", clock() - startTime);
    
    PrintMatrix(C, M);
    
    free(A);
    free(B);
    free(C);
        
    return 0;
}

void PrintMatrix(int* data, int size)
{
    int i = 0, j = 0;
    for(i = 0; i < size; i++)
    {
        for(j = 0; j < size; j++)
        {
            printf("%d\t", ELEM(data, i, j));
        }
        printf("\n");
    }
}

void* multipl(void* arg)
{
    int blockNumber = *((int*)arg);
    int blockSize = M / N;
    
    int i = 0, j = 0, k = 0;
    //printf("no %d, size %d, from %d to %d\n", blockNumber, blockSize, blockSize * blockNumber,
    //        blockSize * (blockNumber + 1));
    for(i = blockSize * blockNumber; i < blockSize * (blockNumber + 1); i++)
    {
        for(k = 0; k < M; k++)
        {
            for(j = 0; j < M; j++) 
            {
                ELEM(C, i, j) += ELEM(A, i , k) * ELEM(B, k, j);
            }
        }
    }
    
    return NULL;
}
