#include <stdio.h>
#include "immintrin.h"

#define size 1024

int main()
{
    // initialize arrays
    double *d = aligned_alloc(32, sizeof(double) * size);
    double *d1 = aligned_alloc(32, sizeof(double) * size);
    double *d2 = aligned_alloc(32, sizeof(double) * size);
    for(int i = 0; i < size; ++i)
    {
        *(d1+i) = i+1;
        *(d2+i) = i+1;
    }

    __m256d v, v1, v2;
    // loads arrays into avx vectors and add them
    for(int i = 0; i < size; i+=4)
    {
        v1 = _mm256_load_pd(&d1[i]);
        v2 = _mm256_load_pd(&d2[i]);
        v = _mm256_add_pd(v1, v2);
        _mm256_store_pd(&d[i], v);
    }

    // print results array
    for(int i = 0; i < size; ++i)
    {
        if(i % 8 == 0)
            printf("%f\n", d[i]);
        else
            printf("%f ", d[i]);
    }
    printf("\n");

    return 0;
}
