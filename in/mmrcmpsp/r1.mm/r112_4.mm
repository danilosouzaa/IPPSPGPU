************************************************************************
file with basedata            : cr112_.bas
initial value random generator: 905661541
************************************************************************
projects                      :  1
jobs (incl. supersource/sink ):  18
horizon                       :  118
RESOURCES
  - renewable                 :  1   R
  - nonrenewable              :  2   N
  - doubly constrained        :  0   D
************************************************************************
PROJECT INFORMATION:
pronr.  #jobs rel.date duedate tardcost  MPM-Time
    1     16      0       28        7       28
************************************************************************
PRECEDENCE RELATIONS:
jobnr.    #modes  #successors   successors
   1        1          3           2   3   4
   2        3          3           5   6   9
   3        3          3           5   6  11
   4        3          2           6   8
   5        3          3           7   8  14
   6        3          2           7  17
   7        3          2          15  16
   8        3          3          10  12  16
   9        3          2          11  13
  10        3          2          13  15
  11        3          2          14  17
  12        3          1          15
  13        3          1          17
  14        3          1          16
  15        3          1          18
  16        3          1          18
  17        3          1          18
  18        1          0        
************************************************************************
REQUESTS/DURATIONS:
jobnr. mode duration  R 1  N 1  N 2
------------------------------------------------------------------------
  1      1     0       0    0    0
  2      1     5       4    7    0
         2     7       2    5    0
         3     8       0    2    0
  3      1     9       9    2    0
         2     9       7    0    4
         3    10       0    0    3
  4      1     2       4    8    0
         2     4       0    0    9
         3     5       0    0    7
  5      1     9      10    0   10
         2    10       0    0   10
         3    10       0    9    0
  6      1     1       0    9    0
         2     7       6    6    0
         3     8       0    5    0
  7      1     1       9    0    3
         2     3       7    3    0
         3     6       1    0    1
  8      1     2       0    2    0
         2     7       0    0   10
         3     7       0    1    0
  9      1     1       0    6    0
         2     3      10    0    2
         3     4       9    3    0
 10      1     4       0    3    0
         2     6       0    0    9
         3     9       5    0    8
 11      1     5       2    8    0
         2     5       0    0    9
         3     8       0    8    0
 12      1     2       0    5    0
         2     4       0    3    0
         3     8       4    1    0
 13      1     1       8    3    0
         2     5       7    0    9
         3     7       0    0    8
 14      1     8       7    0    3
         2     8       0    4    0
         3     9       0    2    0
 15      1     3       0    0    7
         2     4       6    5    0
         3     7       0    2    0
 16      1     2       2    5    0
         2     6       0    5    0
         3     8       1    0    6
 17      1     2       5    6    0
         2     2       4    8    0
         3     4       2    1    0
 18      1     0       0    0    0
************************************************************************
RESOURCEAVAILABILITIES:
  R 1  N 1  N 2
   19   48   41
************************************************************************
