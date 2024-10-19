# homework 2

## 3-a

### usage

```bash
$ g++ 3-a.cpp -o build/3-a
$ ./build/3-a
```

### output

To simplify the output, the program will only print the result of the last query, and set the maximum number of words to 5 instead of 25.

```
input: 
hello word i love word word word hello
word word repeat in the recent 5 words
word word repeat in the recent 5 words
word word repeat in the recent 5 words
```

### note

1. The words are output streamingly in the practice, so the alarm appears repeatedly.
2. The program can be used for data encode -- like run-length encoding. The program can also used for gene sequence analysis.

## 4-a

### usage
  
```bash
$ g++ 4-c.c -o build/4-c
$ ./build/4-c
```

### output

```
# input
input: (-1 for break)
2
7
9
7
5
34
67
78
3
678
-1
# output
inorder: 
2 3 5 7 7 9 34 67 78 678 
```

