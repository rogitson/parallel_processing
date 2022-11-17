#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

char *FileToString(const char *filename);
int *parCountTask(char *buffer);
void *countChar(void *p);

struct s
{
    int *arr;
    char *buf;
    int c;
};

// Maximum number of characters to count through.
int maxlen = 10000;

int main()
{
    // Variables for time calculation.
    clock_t start, end;
    long double time;

    int *ans;
    char *buffer = FileToString("lorem.txt");

    start = clock();
    ans = parCountTask(buffer);
    end = clock();
    time = (long double)(end - start) / CLOCKS_PER_SEC;

    for (int i = 0; i < 128; ++i)
        printf("ASCII #%d: %c -> %d\n", i, i, *(ans + i));

    printf("Time taken to count occurrences parallelly is %Lf\n", time);

    // Cleanup.
    free(buffer);
    free(ans);
    return 0;
}

char *FileToString(const char *filename)
{
    char *buffer = 0;
    long length;
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
        pthread_create(tid + i, NULL, countChar, (void *)t);
    }
    for (int i = 0; i < 128; ++i)
        pthread_join(*(tid + i), NULL);

    free(tid);
    return ans;
}

void *countChar(void *p)
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