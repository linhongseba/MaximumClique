

# MaximumClique
The implementation for maximum clique enumeration algorithm.

We provide a practical implementation of maximum clique enumeration algorithm for massive networks.

Related Publications:

[James Cheng, Yiping Ke, Ada Fu, Jeffrey Xu Yu, Linhong Zhu: Finding Maximal Cliques in Massive Networks by H* Graph. ACM SIGMOD 2010: 447-458 [pdf]](http://www.isi.edu/~linhong/Papers/TODS11.pdf)


James Cheng, Yiping Ke, Ada Fu, Jeffrey Yu, and Linhong Zhu. Finding Maximal Cliques in Massive Networks. ACM Transactions on Database Systems, Volume 36, Number 4, Pages 21:1-21:34, 2011. (TODS 2011) [Special issue on the best papers of SIGMOD 2010] [pdf].

For more details, please read the paper and the Readme_TODS.pdf within MCE_TODS

Requirements:

a. All your node ids should be integers ranging from 0 to MAX-1, MAX is total number of nodes.

b. Input should be in the format as shown below. 


###Sample-files: 

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











