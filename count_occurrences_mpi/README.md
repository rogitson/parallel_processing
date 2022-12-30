To compile the code:

```sh
mpicc count_occurrences_mpi.c -o count_occurrences_mpi.out -fopenmp
```

To run the code:

```sh
mpiexec -n 4 ./count_occurrences_mpi.out
```

To run the code on a different number of nodes, manually modify the data array size to be divisible by (n-1) so that it can be divided evenly across the non-master nodes.