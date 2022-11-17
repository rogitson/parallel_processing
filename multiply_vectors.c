#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <pthread.h>

// int randint(int n);
double randdouble(double low, double high);
double *randVector(int size, int seed);
double *mulVector(double *v1, double *v2, int size);
double *mulVectorPar(double *v1, double *v2, int size);
void *parMulElement(void *p);

struct mul
{
    // Element pointers.
    double *e1, *e2, *e;
    // Count of elements.
    int c;
};

int threads = 16;
// Vector lengths.
#define lcount 7
int l[lcount] = {10, 100, 1000, 10000, 100000, 1000000, 10000000};
// Range for random numbers.
int low = 0, high = RAND_MAX;

int main()
{
    // Variables for time calculation.
    clock_t start, end;
    long double time;

    // Vectors.
    double *v1 = randVector(l[0], 1);
    double *v2 = randVector(l[0], 2);
    double *v;

    // start = clock();
    // v = mulVector(v1, v2, l[0]);
    // end = clock();
    // time = (long double)(end - start) / CLOCKS_PER_SEC;

    // printf("Time taken to multiply sequentially is %Lf\n", time);

    // for (int i = 0; i < l[0]; ++i)
    // {
    //     printf("%f * %f = %f\n", *(v1 + i), *(v2 + i), *(v + i));
    // }

    // start = clock();
    // v = mulVectorPar(v1, v2, l[0]);
    // end = clock();
    // time = (long double)(end - start) / CLOCKS_PER_SEC;

    // printf("Time taken to multiply parallelly is %Lf\n", time);

    // for (int i = 0; i < l[0]; ++i)
    // {
    //     printf("%f * %f = %f\n", *(v1 + i), *(v2 + i), *(v + i));
    // }

    for (int i = 0; i < lcount; ++i)
    {
        // Initialize input vectors.
        v1 = randVector(l[i], 1);
        v2 = randVector(l[i], 2);

        start = clock();
        v = mulVector(v1, v2, l[i]);
        end = clock();
        time = (long double)(end - start) / CLOCKS_PER_SEC;
        printf("Time taken to multiply vectors of size %d sequentially is %Lf\n", l[i], time);

        start = clock();
        v = mulVectorPar(v1, v2, l[i]);
        end = clock();
        time = (long double)(end - start) / CLOCKS_PER_SEC;
        printf("Time taken to multiply vectors of size %d parallelly is %Lf\n", l[i], time);
    }

    // Cleanup.
    free(v1);
    free(v2);
    free(v);
    return 0;
}

// /* Returns an integer in the range [0, n).
//  *
//  * Uses rand(), and so is affected-by/affects the same seed.
//  */
// int randint(int n)
// {
//     if ((n - 1) == RAND_MAX)
//     {
//         return rand();
//     }
//     else
//     {
//         // Supporting larger values for n would requires an even more
//         // elaborate implementation that combines multiple calls to rand()
//         assert(n <= RAND_MAX);

//         // Chop off all of the values that would cause skew...
//         int end = RAND_MAX / n; // truncate skew
//         assert(end > 0);
//         end *= n;

//         // ... and ignore results from rand() that fall above that limit.
//         // (Worst case the loop condition should succeed 50% of the time,
//         // so we can expect to bail out of this loop pretty quickly.)
//         int r;
//         while ((r = rand()) >= end)
//             ;

//         return r % n;
//     }
// }

// Return a random double within range [low, high].
double randdouble(double low, double high)
{
    return ((double)rand() * (high - low)) / (double)RAND_MAX + low;
}

// Returns a vector of length size filled with random numbers.
// Random numbers are generated using rand() with seed seed.
double *randVector(int size, int seed)
{
    // Initialize random seed.
    srand(seed);

    // Allocate the required memory locations.
    double *v = malloc(sizeof(double) * size);

    // Fill vector with random numbers.
    for (int i = 0; i < size; ++i)
    {
        *(v + i) = randdouble(low, high);
    }

    return v;
}

// Multiply two vectors sequentially.
double *mulVector(double *v1, double *v2, int size)
{
    double *v = malloc(sizeof(double) * size);

    for (int i = 0; i < size; ++i)
    {
        *(v + i) = *(v1 + i) * *(v2 + i);
    }

    return v;
}

// Multiply two vectors parallelly.
double *mulVectorPar(double *v1, double *v2, int size)
{
    double *v = malloc(sizeof(double) * size);
    pthread_t *tid = malloc(sizeof(pthread_t) * threads);

    if (size <= threads)
    {
        for (int i = 0; i < size; ++i)
        {
            struct mul *m = malloc(sizeof(struct mul));
            (*m).e1 = v1 + (i);
            (*m).e2 = v2 + (i);
            (*m).e = v + (i);
            (*m).c = 1;
            pthread_create(tid + i, NULL, parMulElement, (void *)m);
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
            struct mul *m = malloc(sizeof(struct mul));
            if (t > 0)
            {
                (*m).e1 = v1 + (i * (numElements + 1));
                (*m).e2 = v2 + (i * (numElements + 1));
                (*m).e = v + (i * (numElements + 1));
                (*m).c = (numElements + 1);
            }
            else
            {
                (*m).e1 = v1 + (i * numElements + r);
                (*m).e2 = v2 + (i * numElements + r);
                (*m).e = v + (i * numElements + r);
                (*m).c = numElements;
            }
            pthread_create(tid + i, NULL, parMulElement, (void *)m);
        }
    }

    for (int i = 0; i < threads; ++i)
        pthread_join(*(tid + i), NULL);

    free(tid);
    return v;
}

// Thread function, receives struct mul containing 3 addresses.
// Multiply elements in two addresses and store result in third address.
void *parMulElement(void *p)
{
    struct mul *m = (struct mul *)p;
    for (int i = 0; i < (*m).c; ++i)
    {
        *((*m).e + i) = *((*m).e1 + i) * *((*m).e2 + i);
    }
    free(m);
}