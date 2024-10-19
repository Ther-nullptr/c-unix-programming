# homework 1

## 1-b

### usage

```bash
$ g++ 1-b.cpp -o build/1-b
$ ./build/1-b
```

### output

```
Person P1 Recent Activities (last 14 days):
Building Unit: Unit 201, Timestamp: 2024-09-28 20:52:39
Building Unit: Unit 101, Timestamp: 2024-10-03 20:52:39
Building Unit: Unit 102, Timestamp: 2024-10-07 20:52:39
Building Unit: Unit 201, Timestamp: 2024-10-08 08:52:39
Building Unit: Unit 103, Timestamp: 2024-10-08 14:52:39
Building Unit: Unit 104, Timestamp: 2024-10-08 18:28:39
Infected count within 1km of Unit 101: 2
```

### note

The program tests 2 usages:
1. It can display the activity trajectory of a specified person over the past 14 days.
2. It can show the number of COVID-19 infections within a 1km radius of a specified location over the last 24 hours.

## 2-b

### usage

```bash
$ g++ 2-b.cpp -o build/2-b
$ ./build/2-b
```

### output

```
The chosen ball is Ball #0 and it is lighter.
[1st time] a vs b
[result] The right is heavier.
[2nd time] a[0] + c[0:3] vs b[0] + a[1:4]
[result] The right is heavier.
[3rd time] a[0] vs c[0]
[result] The right is heavier.
The ball with different weight is Ball #0 and it is lighter.
[Correct]
...
The chosen ball is Ball #11 and it is heavier.
[1st time] a vs b
[result] The two groups have the same weight.
[2nd time] a[:3] vs c[:3]
[result] The two groups have the same weight.
[3rd time] a[3] vs c[3]
[result] The right is heavier.
The ball with different weight is Ball #11 and it is heavier.
[Correct]
```

### note

This program tests all 24 cases and gets the correct determination.