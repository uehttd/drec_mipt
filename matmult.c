#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1
#define M 1000

/*
 * Лучше старайтесь избегать define`ов, т.к. можно долго отлавливать ошибки.
 * http://stackoverflow.com/questions/5726562/define-macro-usage-what-was-the-problem
 * 
 * И таких примеров можно много нагенерить.
 * Вот напишете где-нибудь ELEM(A, i + 1, k) и начнёте баг искать...
 */
#define ELEM(MATR, i, j) *(MATR + i * M + j)

/*
 * Называйте уж все ф-и с заглавной.
 */
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
            ELEM(A, i, j) = rand() % 200 - 100;
            ELEM(B, i, j) = rand() % 200 - 100;
        }
    }
    
    //PrintMatrix(A, M);
    //printf("\n");
    //PrintMatrix(B, M);
    
    pthread_t threadIDs[N];
    int args[N] = {};
    unsigned int startTime = clock();
    for(i = 0; i < N; i++)
    {
        args[i] = i;
        /*
         * Мне кажется, что если написать &(threadIDs[i]), то будет понятнее, что вы хотели сказать.
         * А между понятностью и краткостью лучше выбрать понятность. 
         * Хотя, конечно, это субъективное мнение. Делайте так, как сами считаете верным.
         */
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
    
    printf("Done! Clocks elapsed: %f\n", (double)(clock() - startTime) / CLOCKS_PER_SEC);
    
    //PrintMatrix(C, M);
    
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

/*
 * У меня на виртуалке последовательный код самый быстрый, хотя на неё 3 ядра выделено.
 * Надеюсь, что у вас на основной ОС получилось увидеть хоть какой-то boost.
 * На прошлом семинаре у Никиты и Станислава получилось увидеть ускорение.
 * 
 * В целом, нормально, засчитано. Поправьте мелкие замечания и закоммитьте. На семинаре похвастаетесь полученным ускорением + отличием наивной реализации от реализации с использованием разных фишек.
 * 1. Можете попробовать транспонировать вторую матрицу до умножения. Тогда обращение к памяти будет более локальным и промахов кэша будет меньше.
 * 
 * 2. Посмотрите, насколько отличается ijk от ikj алгортма умножения.
 * http://stackoverflow.com/questions/20467117/for-matrix-operation-why-is-ikj-faster-than-ijk
 */
