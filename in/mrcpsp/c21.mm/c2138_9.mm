************************************************************************
file with basedata            : c2138_.bas
initial value random generator: 227382715
************************************************************************
projects                      :  1
jobs (incl. supersource/sink ):  18
horizon                       :  122
RESOURCES
  - renewable                 :  2   R
  - nonrenewable              :  2   N
  - doubly constrained        :  0   D
************************************************************************
PROJECT INFORMATION:
pronr.  #jobs rel.date duedate tardcost  MPM-Time
    1     16      0       19       10       19
************************************************************************
PRECEDENCE RELATIONS:
jobnr.    #modes  #successors   successors
   1        1          3           2   3   4
   2        3          3           6   8   9
   3        3          3           5   8   9
   4        3          3           7   9  12
   5        3          3           6  10  14
   6        3          2           7  11
   7        3          2          13  17
   8        3          3          10  11  14
   9        3          2          13  14
  10        3          3          12  15  17
  11        3          2          12  13
  12        3          1          16
  13        3          2          15  16
  14        3          3          15  16  17
  15        3          1          18
  16        3          1          18
  17        3          1          18
  18        1          0        
************************************************************************
REQUESTS/DURATIONS:
jobnr. mode duration  R 1  R 2  N 1  N 2
------------------------------------------------------------------------
  1      1     0       0    0    0    0
  2      1     1       8    5    3    4
         2     4       7    5    3    4
         3     5       6    4    2    3
  3      1     4       8    8   10    7
         2     5       6    6   10    7
         3     9       4    5    9    6
  4      1     1       6    6    9    8
         2     2       5    5    7    6
         3     5       3    5    5    4
  5      1     4       9    8    6    6
         2     5       8    7    5    5
         3    10       5    2    2    5
  6      1     1       8    7    9    2
         2     4       6    5    6    1
         3    10       1    4    2    1
  7      1     1       7   10    4    5
         2     1       7    9    4    7
         3     2       7    8    2    4
  8      1     3      10    3    8    5
         2     7      10    3    7    3
         3    10       9    3    6    3
  9      1     2       2    4    9   10
         2     5       2    3    9   10
         3    10       1    3    9    9
 10      1     6       5    4    8    6
         2     8       3    2    8    4
         3     9       2    2    7    4
 11      1     2       3    7    5    8
         2     5       2    6    3    8
         3     5       2    6    4    7
 12      1     3       6    8    8    9
         2     7       6    6    7    7
         3     9       4    3    6    7
 13      1     3      10    8    5    7
         2     8       9    4    5    4
         3     8       8    5    5    5
 14      1     3       9    7    6    4
         2     5       7    3    6    3
         3     6       3    3    5    2
 15      1     3       8    6    6   10
         2     3       7    6    8   10
         3     6       3    6    6    9
 16      1     2       8    3    4    2
         2     4       7    3    4    2
         3     8       7    3    4    1
 17      1     4       9    9    5    5
         2    10       5    3    4    5
         3    10       2    5    5    4
 18      1     0       0    0    0    0
************************************************************************
RESOURCEAVAILABILITIES:
  R 1  R 2  N 1  N 2
   17   18   85   80
************************************************************************
