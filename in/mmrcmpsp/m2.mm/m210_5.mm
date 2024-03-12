************************************************************************
file with basedata            : cm210_.bas
initial value random generator: 169445965
************************************************************************
projects                      :  1
jobs (incl. supersource/sink ):  18
horizon                       :  106
RESOURCES
  - renewable                 :  2   R
  - nonrenewable              :  2   N
  - doubly constrained        :  0   D
************************************************************************
PROJECT INFORMATION:
pronr.  #jobs rel.date duedate tardcost  MPM-Time
    1     16      0       21       15       21
************************************************************************
PRECEDENCE RELATIONS:
jobnr.    #modes  #successors   successors
   1        1          3           2   3   4
   2        2          3           7  11  14
   3        2          3           5   9  14
   4        2          2           6   8
   5        2          3           6   8  11
   6        2          3          15  16  17
   7        2          3           8   9  12
   8        2          1          10
   9        2          1          17
  10        2          2          13  16
  11        2          2          12  13
  12        2          1          15
  13        2          1          15
  14        2          2          16  17
  15        2          1          18
  16        2          1          18
  17        2          1          18
  18        1          0        
************************************************************************
REQUESTS/DURATIONS:
jobnr. mode duration  R 1  R 2  N 1  N 2
------------------------------------------------------------------------
  1      1     0       0    0    0    0
  2      1     1       0    1    6    0
         2     1       2    0    8    0
  3      1     5       3    0    0    9
         2     6       3    0    0    8
  4      1     4       1    0    0    9
         2     7       0    7    0    8
  5      1     1       7    0    0    5
         2     6       0    2    7    0
  6      1     5       9    0    5    0
         2     9       8    0    4    0
  7      1     7       4    0    0    5
         2     8       0    2    0    4
  8      1     3       5    0    0    6
         2     7       0    6    8    0
  9      1     5       5    0    4    0
         2    10       0    9    4    0
 10      1     4       0    5    0    5
         2     8       0    5    6    0
 11      1     2       0    6    5    0
         2     3       7    0    0   10
 12      1     2       0    3    8    0
         2     8       5    0    0    2
 13      1     4       9    0    3    0
         2     5       0    3    2    0
 14      1     5       0    4    0    7
         2     7       0    4    8    0
 15      1     2       0    8    0    2
         2     4       0    8    2    0
 16      1     4       0    9    1    0
         2     8       0    7    0    8
 17      1     4       0   10    0    5
         2     9       0    8    8    0
 18      1     0       0    0    0    0
************************************************************************
RESOURCEAVAILABILITIES:
  R 1  R 2  N 1  N 2
   17   15   45   47
************************************************************************
