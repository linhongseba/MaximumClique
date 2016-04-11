

# MaximumClique
The implementation for maximum clique enumeration algorithm.

We provide a practical implementation of maximum clique enumeration algorithm for massive networks.

## Table of Content

- [ReadMe](#readMe)

- [Publication](#publication)

- [Sample Files](#sample-files)

- [Walk-through Examples](#examples)

## ReadMe
The detailed ReadMe File is located [here](https://github.com/linhongseba/MaximumClique/blob/master/MCE_TODS/Readme_TODS.pdf)

## Publication

James Cheng, Yiping Ke, Ada Fu, Jeffrey Xu Yu, Linhong Zhu: Finding Maximal Cliques in Massive Networks by H* Graph. ACM SIGMOD 2010: 447-458 [pdf](http://www.isi.edu/~linhong/Papers/sigmod10.pdf)


James Cheng, Yiping Ke, Ada Fu, Jeffrey Yu, and Linhong Zhu. Finding Maximal Cliques in Massive Networks. ACM Transactions on Database Systems, Volume 36, Number 4, Pages 21:1-21:34, 2011. (TODS 2011) [Special issue on the best papers of SIGMOD 2010] [pdf](http://www.isi.edu/~linhong/Papers/TODS11.pdf).

## Sample files

There is sample-input 
```
Input:

12
0,4:1:2:4:5
1,2:0:2
2,5:0:1:3:4:5
4,4:0:2:3:5
3,2:2:4
5,3:0:2:4
10,1:11
11,1:10
```
Output : Output has 2 files
```
1) Cliques
0 2 1 
0 2 5 4 
10 11 

This lists all the cliques found in the input graph.

2) Clique Statistics
1	0
2	1
3	1
4	1
```
In every line first number is clique size and second number is total cliques found. For example, there is one clique of size 2 and size 3 etc..


## Examples
For this algorithm, node ids should be integers. Most of use-cases might have string ids. So I want to describe an example how to convert your string ids to integers using a hashtable.

#### 1. Converting strings to integer ids
Say, your input file is in [this](https://github.com/linhongseba/MaximumClique/blob/master/sample-files/str-input.txt) format
```
s_id1 s_id2
s_id3 s_id4
```

You can use the [str_to_int.py](https://github.com/linhongseba/MaximumClique/blob/master/util/str_to_int.py) script here to convert your string ids to integers. It uses a hashtable for that purpose. As you can see it will output hashtable to a file

Now you have ids
```
MAX_NODES
int_id1 int_id2
int_id3 int_id4
```
Output will look like [int-input.txt](https://github.com/linhongseba/MaximumClique/blob/master/sample-files/int-input.txt)

####Usage for str_to_int.py
```
python str_to_int.py str-input int-input
```
This will create a file int-input with integer ids and 2 more files with hashtable and inverse hashtable mapping.


Now, you have the integer ids, you can run the [edge2standard.cpp](https://github.com/linhongseba/MaximumClique/blob/master/util/edge2standard.cpp) program to get the format required for the algorithm.

Yay, you are done. You problem is solved and you can use the [MCE_TODS.cpp](https://github.com/linhongseba/MaximumClique/blob/master/MCE_TODS/MCE_TODS.cpp) for the next step. 
















