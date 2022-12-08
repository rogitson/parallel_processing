#include <stdio.h>

#define MAX 1000
double a[MAX][MAX], b[MAX][MAX], c[MAX][MAX];

void initMatrix(double m[MAX][MAX])
{
    int i, j;
    #pragma omp parallel for private(j)
    for(i = 0; i < MAX; ++i)
        for(j = 0; j < MAX; ++j)
            m[i][j] = i + 2*j;
}

void initMatrixZeros(double m[MAX][MAX])
{
    int i, j;
    #pragma omp parallel for private(j)
    for(i = 0; i < MAX; ++i)
        for(j = 0; j < MAX; ++j)
            m[i][j] = 0;
}

void mm(double a[MAX][MAX], double b[MAX][MAX], double c[MAX][MAX])
{
    int i, j, k;
    for(j = 0; j < MAX; ++j)
        for(i = 0; i < MAX; ++i)
            for(k = 0; k < MAX; ++k)
                c[i][j] += a[i][k]*b[k][j];
}

void mmPar(double a[MAX][MAX], double b[MAX][MAX], double c[MAX][MAX])
{
    int i, j, k;
    #pragma omp parallel for private(i,k)
    for(j = 0; j < MAX; ++j)
        for(i = 0; i < MAX; ++i)
            for(k = 0; k < MAX; ++k)
                c[i][j] += a[i][k]*b[k][j];
}

int main()
{
    initMatrix(a);
    initMatrix(b);
    initMatrixZeros(c);

    mm(a, b, c);
    mmPar(a, b, c);
    return 0;
}