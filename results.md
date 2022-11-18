# Results

This file contains a sample of my results. These are of my codes performed on my laptop. 

My specs are a Ryzen 7 5700u, 8GB DDR4 3200MHz.

## Count Occurrences of ASCII Characters

```
In lorem.txt:
Time taken to count occurrences sequentially 0.000021
Time taken to count occurrences while parallelizing the task is 0.001379
Time taken to count occurrences while parallelizing the data is 0.000095
In lorem_20k.txt:
Time taken to count occurrences sequentially 0.000052
Time taken to count occurrences while parallelizing the task is 0.001748
Time taken to count occurrences while parallelizing the data is 0.000089
In lorem_100k.txt:
Time taken to count occurrences sequentially 0.000274
Time taken to count occurrences while parallelizing the task is 0.006359
Time taken to count occurrences while parallelizing the data is 0.000270
In lorem_1m.txt:
Time taken to count occurrences sequentially 0.003938
Time taken to count occurrences while parallelizing the task is 0.065147
Time taken to count occurrences while parallelizing the data is 0.000928
```

## Vector Multiplication

```
Time taken to multiply vectors of size 10 sequentially is 0.000001
Time taken to multiply vectors of size 10 parallelly is 0.000125
Time taken to multiply vectors of size 100 sequentially is 0.000001
Time taken to multiply vectors of size 100 parallelly is 0.000070
Time taken to multiply vectors of size 1000 sequentially is 0.000004
Time taken to multiply vectors of size 1000 parallelly is 0.000052
Time taken to multiply vectors of size 10000 sequentially is 0.000053
Time taken to multiply vectors of size 10000 parallelly is 0.000110
Time taken to multiply vectors of size 100000 sequentially is 0.000930
Time taken to multiply vectors of size 100000 parallelly is 0.000417
Time taken to multiply vectors of size 1000000 sequentially is 0.008082
Time taken to multiply vectors of size 1000000 parallelly is 0.005863
Time taken to multiply vectors of size 10000000 sequentially is 0.060784
Time taken to multiply vectors of size 10000000 parallelly is 0.061139
```

## Matrix Multiplication

```
Time taken to multiply matrices of size 10x10 sequentially is 0.000011
Time taken to multiply matrices of size 10x10 parallelly is 0.000059
Time taken to multiply matrices of size 100x100 sequentially is 0.003974
Time taken to multiply matrices of size 100x100 parallelly is 0.001997
Time taken to multiply matrices of size 200x200 sequentially is 0.031615
Time taken to multiply matrices of size 200x200 parallelly is 0.009742
Time taken to multiply matrices of size 500x500 sequentially is 0.538281
Time taken to multiply matrices of size 500x500 parallelly is 0.140733
Time taken to multiply matrices of size 1000x1000 sequentially is 7.776751
Time taken to multiply matrices of size 1000x1000 parallelly is 1.165385
```