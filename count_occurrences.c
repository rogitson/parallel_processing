#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

char *FileToString(const char *filename);
int *Count(char * buffer);
int *parCountTask(char *buffer);
void *countCharTask(void *p);
int *parCountData(char *buffer);
void *countCharData(void *p);

struct s
{
    int *arr;
    char *buf;
    int c;
};

// Maximum number of characters to count through.
int maxlen = 10000000;
int threads = 16;
long length;
pthread_mutex_t lock;

int main()
{
    // Variables for time calculation.
    clock_t start, end;
    long double time;

    int *ans;
    char *buffer;

    char* files[] = {"lorem.txt", "lorem_20k.txt", "lorem_100k.txt", "lorem_1m.txt"};

    for(int i = 0; i < 4; ++i)
    {
        buffer = FileToString(files[i]);

        printf("In %s:\n", files[i]);

        start = clock();
        ans = Count(buffer);
        end = clock();
        time = (long double)(end - start) / CLOCKS_PER_SEC;

        printf("Time taken to count occurrences sequentially %Lf\n", time);

        start = clock();
        ans = parCountTask(buffer);
        end = clock();
        time = (long double)(end - start) / CLOCKS_PER_SEC;

        printf("Time taken to count occurrences while parallelizing the task is %Lf\n", time);

        start = clock();
        ans = parCountData(buffer);
        end = clock();
        time = (long double)(end - start) / CLOCKS_PER_SEC;

        printf("Time taken to count occurrences while parallelizing the data is %Lf\n", time);
    }

    // for (int i = 0; i < 128; ++i)
    //     printf("ASCII #%d: %c -> %d\n", i, i, *(ans + i));

    // Cleanup.
    free(buffer);
    free(ans);
    return 0;
}

char *FileToString(const char *filename)
{
    char *buffer = 0;
    // long length;
    FILE *f = fopen(filename, "r");

    if (f)
    {
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);
        buffer = malloc(length);
        if (buffer)
        {
            fread(buffer, 1, length, f);
        }
        fclose(f);
    }
    return buffer;
}

int *Count(char * buffer)
{
    int *arr = malloc(sizeof(int) * 128);

    if (buffer)
    {
        for (int i = 0; i < maxlen; ++i)
        {
            *(arr + *(buffer + i)) += 1;
            if (*(buffer + i) == '\0')
                break;
        }
    }

    return arr;
}

int *parCountTask(char *buffer)
{
    int *ans = malloc(sizeof(int) * 128);
    pthread_t *tid = malloc(sizeof(pthread_t) * 128);

    // Set results array to 0.
    for (int i = 0; i < 128; ++i)
        *(ans + i) = 0;

    for (int i = 0; i < 128; ++i)
    {
        struct s *t = malloc(sizeof(struct s));
        (*t).arr = ans;
        (*t).buf = buffer;
        (*t).c = i;
        pthread_create(tid + i, NULL, countCharTask, (void *)t);
    }
    for (int i = 0; i < 128; ++i)
        pthread_join(*(tid + i), NULL);

    free(tid);
    return ans;
}

void *countCharTask(void *p)
{
    struct s *t = (struct s *)p;
    char *buffer = (*t).buf;
    char target = (char)(*t).c;
    if (buffer)
    {
        for (int i = 0; i < maxlen; ++i)
        {
            if (*(buffer + i) == target)
                *((*t).arr + (*t).c) += 1;
            if (*(buffer + i) == '\0')
                break;
        }
    }
}

int *parCountData(char *buffer)
{
    int *ans = malloc(sizeof(int) * 128);
    pthread_t *tid = malloc(sizeof(pthread_t) * threads);

    // Set results array to 0.
    for (int i = 0; i < 128; ++i)
        *(ans + i) = 0;

    if (length <= threads)
    {
        for (int i = 0; i < length; ++i)
        {
            struct s *t = malloc(sizeof(struct s));
            (*t).arr = ans;
            (*t).buf = buffer;
            (*t).c = 1;
            pthread_create(tid + i, NULL, countCharData, (void *)t);
        }
    }
    else
    {
        // Split elements evenly across threads
        // r number of threads will have 1 extra element
        int r = length % threads;
        int numElements = (length - r) / threads;

        for (int i = 0, t = r; i < threads; ++i, --t)
        {
            // Store both input element addresses and result address in a struct to pass it to the thread.
            struct s *t = malloc(sizeof(struct s));
            (*t).arr = ans;
            (*t).buf = (t > 0) ? buffer + (i * (numElements + 1)) : buffer + (i * numElements + r);
            (*t).c = (t > 0) ? (numElements + 1) : numElements;
            pthread_create(tid + i, NULL, countCharData, (void *)t);
        }
    }

    for (int i = 0; i < threads; ++i)
        pthread_join(*(tid + i), NULL);

    free(tid);
    return ans;
}

void *countCharData(void *p)
{
    struct s *t = (struct s *)p;
    char *buffer = (*t).buf;
    int *arr = malloc(sizeof(int) * 128);

    // Set results array to 0.
    for (int i = 0; i < 128; ++i)
        *(arr + i) = 0;

    if (buffer)
    {
        for (int i = 0; i < (*t).c; ++i)
        {
            *(arr + *(buffer + i)) += 1;
            if (*(buffer + i) == '\0')
                break;
        }
    }

    pthread_mutex_lock(&lock);
    for (int i = 0; i < 128; ++i)
    {
        (*t).arr[i] += arr[i];
    }
    pthread_mutex_unlock(&lock);

    free(arr);
}