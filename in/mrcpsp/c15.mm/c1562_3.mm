************************************************************************
file with basedata            : c1562_.bas
initial value random generator: 1338355746
************************************************************************
projects                      :  1
jobs (incl. supersource/sink ):  18
horizon                       :  116
RESOURCES
  - renewable                 :  2   R
  - nonrenewable              :  2   N
  - doubly constrained        :  0   D
************************************************************************
PROJECT INFORMATION:
pronr.  #jobs rel.date duedate tardcost  MPM-Time
    1     16      0       18       10       18
************************************************************************
PRECEDENCE RELATIONS:
jobnr.    #modes  #successors   successors
   1        1          3           2   3   4
   2        3          1           9
   3        3          3           5   6   7
   4        3          1          14
   5        3          3           8  10  15
   6        3          2          11  16
   7        3          1          13
   8        3          2           9  16
   9        3          1          12
  10        3          3          12  13  16
  11        3          1          14
  12        3          1          17
  13        3          1          14
  14        3          1          17
  15        3          1          18
  16        3          1          18
  17        3          1          18
  18        1          0        
************************************************************************
REQUESTS/DURATIONS:
jobnr. mode duration  R 1  R 2  N 1  N 2
------------------------------------------------------------------------
  1      1     0       0    0    0    0
  2      1     1       9    6    7    9
         2     6       9    6    6    9
         3    10       9    3    2    8
  3      1     2       9    3    7    6
         2     7       8    3    7    5
         3     8       6    2    4    3
  4      1     5       7    6    9    7
         2     7       7    6    8    5
         3     8       7    5    7    3
  5      1     7       5    5    6    8
         2     8       4    3    3    7
         3     8       5    4    3    3
  6      1     1       9    5    9    6
         2     3       9    5    9    5
         3    10       7    3    9    3
  7      1     2       8    4   10    6
         2     6       8    4    9    5
         3     7       8    2    9    5
  8      1     1       7    5    7    9
         2     2       7    4    6    5
         3     7       6    4    5    4
  9      1     3       3    9    3    8
         2     5       2    8    3    7
         3     6       1    6    2    7
 10      1     1       9    7    9    8
         2     3       8    3    5    7
         3     3       5    3    7    7
 11      1     1       3    4    9    9
         2     2       2    4    5    6
         3     6       2    3    2    3
 12      1     1       8    8    8    7
         2     1       7    8    9    5
         3     4       7    8    8    2
 13      1     4       7    7    9    5
         2     6       6    7    8    4
         3     8       5    6    8    3
 14      1     1       9    6    8    1
         2     1       8    6    9    1
         3     5       6    6    7    1
 15      1     2       6    8   10    9
         2     3       6    5    8    8
         3     8       4    3    6    6
 16      1     8       6    8    8    4
         2     9       5    4    6    3
         3    10       5    3    3    3
 17      1     3       7    2    7    9
         2     6       7    2    5    5
         3     8       6    2    3    5
 18      1     0       0    0    0    0
************************************************************************
RESOURCEAVAILABILITIES:
  R 1  R 2  N 1  N 2
   19   20  128  111
************************************************************************
