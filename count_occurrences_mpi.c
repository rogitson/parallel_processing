#include <mpi.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

// Length of array
#define MAX 9999
// Original data
char data[MAX];

// Number of targets
#define nTargets 9
// Character array containing targeted characters
char targets[nTargets] = {'a', 'b', 'c','d', 'e', 'f', 'g', 'h', 'i'};

// Result array that will be filled with the occurrences of each character
int occurrences[nTargets] = {0};

// Array in which to store the results returned from each node
int results[nTargets];
// Array in which to store the subset that each node receives from master
char subset[MAX];
// Length of subset
int len;
// Array in which to store the targets that each node receives from master
char targetarr[nTargets];
// Length of targets
int targetlen;

char *FileToString(const char *filename);
void initData();

int main()
{
    int rank, size;
    initData();
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Master
    if (rank == 0)
    {
        int chunk = MAX / (size - 1);
        int chunkTargets = nTargets;

        for (int i = 1; i < size; i++)
        {
            // Send data
            MPI_Send(&chunk, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(data + (i - 1) * chunk, chunk, MPI_CHAR, i, 0, MPI_COMM_WORLD);
            MPI_Send(&chunkTargets, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(targets, nTargets, MPI_CHAR, i, 0, MPI_COMM_WORLD);
        }

        for (int i = 1; i < size; i++)
        {
            MPI_Recv(results, nTargets, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for (int j = 0; j < nTargets; ++j)
                occurrences[j] += results[j];
        }

        for (int i = 0; i < nTargets; ++i)
            printf("%c - > %d\n", targets[i], occurrences[i]);
    }
    // Node
    else
    {
        MPI_Recv(&len, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(subset, len, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&targetlen, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(targetarr, targetlen, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int results[targetlen];

        int j;
#pragma omp parallel for private(j)
        for (int i = 0; i < targetlen; ++i)
        {
            results[i] = 0;
            for (j = 0; j < len; ++j)
            {
                if (subset[j] == targetarr[i])
                {
                    results[i]++;
                }
            }
        }

        MPI_Send(results, targetlen, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}

void initData()
{
    for (int i = 0; i < MAX; ++i)
    {
        data[i] = 'a';
    }
}
