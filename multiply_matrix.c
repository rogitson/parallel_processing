#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <pthread.h>

double randdouble(double low, double high);
double **randMatrix(int size, int seed);
double **mulMatrix(double **m1, double **m2, int size);
double **mulMatrixPar(double **m1, double **m2, int size);
void *parMulMatrix(void *p);

struct mul
{
    // Matrix pointers.
    double **m1, **m2, **m;
    // Count of rows.
    int c;
    int size;
    int offset;
};

const int cores = 8,
          threads = cores * 2;
// Matrix sizes.
#define lcount 5
int l[lcount] = {10, 100, 200, 500, 1000};
// Range for random numbers.
int low = 0, high = RAND_MAX;

int main()
{
    // Variables for time calculation.
    clock_t start, end;
    long double time;

    // Matrices.
    double **m1;
    double **m2;
    double **m;

    for (int i = 0; i < lcount; ++i)
    {
        // Initialize input vectors.
        m1 = randMatrix(l[i], 1);
        m2 = randMatrix(l[i], 2);

        start = clock();
        m = mulMatrix(m1, m2, l[i]);
        end = clock();
        time = (long double)(end - start) / CLOCKS_PER_SEC;
        printf("Time taken to multiply matrices of size %dx%d sequentially is %Lf\n", l[i], l[i], time);

        // for (int i = 0; i < 10; ++i)
        //     for (int j = 0; j < 10; ++j)
        //         printf("%f ", m[i][j]);
        // printf("\n");
        // printf("%f\n", m[0][0]);
        free(m);

        start = clock();
        m = mulMatrixPar(m1, m2, l[i]);
        end = clock();
        time = (long double)(end - start) / CLOCKS_PER_SEC;
        printf("Time taken to multiply matrices of size %dx%d parallelly is %Lf\n", l[i], l[i], time / cores); // Divide time by number of physical cores

        // for (int i = 0; i < 10; ++i)
        //     for (int j = 0; j < 10; ++j)
        //         printf("%f ", m[i][j]);
        // printf("\n");
        // printf("%f\n", m[0][0]);
        free(m);

        // Cleanup.
        free(m1);
        free(m2);
    }

    return 0;
}

// Return a random double within range [low, high].
double randdouble(double low, double high)
{
    return ((double)rand() * (high - low)) / (double)RAND_MAX + low;
}

// Returns a matrix of length size filled with random numbers.
// Random numbers are generated using rand() with seed seed.
double **randMatrix(int size, int seed)
{
    // Initialize random seed.
    srand(seed);

    // Allocate the required memory locations.
    double **m = malloc(sizeof(double) * size);

    // Fill matrix with random numbers.
    for (int i = 0; i < size; ++i)
    {
        m[i] = malloc(sizeof(double) * size);
        for (int j = 0; j < size; ++j)
            m[i][j] = randdouble(low, high);
    }

    return m;
}

// Multiply two matrices sequentially.
double **mulMatrix(double **m1, double **m2, int size)
{
    double **m = malloc(sizeof(double) * size);

    for (int i = 0; i < size; ++i)
    {
        m[i] = malloc(sizeof(double) * size);
        for (int j = 0; j < size; ++j)
        {
            m[i][j] = 0;
            for (int k = 0; k < size; ++k)
                m[i][j] += m1[i][k] * m2[k][j];
        }
    }

    return m;
}

// Multiply two matrices parallelly.
double **mulMatrixPar(double **m1, double **m2, int size)
{
    double **m = malloc(sizeof(double) * size);
    pthread_t *tid = malloc(sizeof(pthread_t) * threads);

    for (int i = 0; i < size; ++i)
    {
        m[i] = malloc(sizeof(double) * size);
        for (int j = 0; j < size; ++j)
            m[i][j] = 0;
    }

    if (size <= threads)
    {
        for (int i = 0; i < size; ++i)
        {
            struct mul *s = malloc(sizeof(struct mul));
            (*s).m1 = m1;
            (*s).m2 = m2;
            (*s).m = m;
            (*s).size = size;
            (*s).c = 1;
            (*s).offset = i;
            pthread_create(tid + i, NULL, parMulMatrix, (void *)s);
        }
    }
    else
    {
        // Split elements evenly across threads
        // r number of threads will have 1 extra element
        int r = size % threads;
        int numElements = (size - r) / threads;

        for (int i = 0, t = r; i < threads; ++i, --t)
        {
            // Store both input element addresses and result address in a struct to pass it to the thread.
            struct mul *s = malloc(sizeof(struct mul));
            (*s).size = size;
            (*s).m1 = m1;
            (*s).m2 = m2;
            (*s).m = m;
            if (t > 0)
            {
                (*s).offset = (i * (numElements + 1));
                (*s).c = (numElements + 1);
            }
            else
            {
                (*s).offset = (i * numElements + r);
                (*s).c = numElements;
            }
            pthread_create(tid + i, NULL, parMulMatrix, (void *)s);
        }
    }

    for (int i = 0; i < threads; ++i)
        pthread_join(*(tid + i), NULL);

    free(tid);
    return m;
}

// Thread function, receives struct mul containing 3 addresses.
// Multiply elements in two addresses and store result in third address.
void *parMulMatrix(void *p)
{
    struct mul *s = (struct mul *)p;
    for (int i = (*s).offset; i < ((*s).offset + (*s).c); ++i)
        for (int j = 0; j < (*s).size; ++j)
            for (int k = 0; k < (*s).size; ++k)
                ((*s).m)[i][j] += ((*s).m1)[i][k] * ((*s).m2)[k][j];
    free(s);
}