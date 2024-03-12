************************************************************************
file with basedata            : cr151_.bas
initial value random generator: 1214168101
************************************************************************
projects                      :  1
jobs (incl. supersource/sink ):  18
horizon                       :  123
RESOURCES
  - renewable                 :  1   R
  - nonrenewable              :  2   N
  - doubly constrained        :  0   D
************************************************************************
PROJECT INFORMATION:
pronr.  #jobs rel.date duedate tardcost  MPM-Time
    1     16      0       21        2       21
************************************************************************
PRECEDENCE RELATIONS:
jobnr.    #modes  #successors   successors
   1        1          3           2   3   4
   2        3          3           5   6   7
   3        3          2          12  14
   4        3          3           8   9  17
   5        3          2          10  11
   6        3          3           9  14  17
   7        3          2          11  13
   8        3          1          10
   9        3          2          13  16
  10        3          2          13  14
  11        3          3          12  15  16
  12        3          1          17
  13        3          1          15
  14        3          2          15  16
  15        3          1          18
  16        3          1          18
  17        3          1          18
  18        1          0        
************************************************************************
REQUESTS/DURATIONS:
jobnr. mode duration  R 1  N 1  N 2
------------------------------------------------------------------------
  1      1     0       0    0    0
  2      1     1       0    7    8
         2     2       0    4    6
         3     5       5    3    5
  3      1     2       7    3    7
         2     3       6    3    6
         3     8       0    2    4
  4      1     2       0    8    3
         2     2       4    7    3
         3     9       4    6    3
  5      1     6       0    8    6
         2     9       9    6    6
         3    10       6    3    6
  6      1     3       6    3    7
         2     6       0    3    5
         3    10       0    2    3
  7      1     2       4    4    5
         2     5       0    3    5
         3     7       0    3    4
  8      1     5       5    6    7
         2     5       0    6    8
         3    10       0    6    1
  9      1     1       7   10    9
         2     4       7    6    6
         3     6       3    5    4
 10      1     1       2    5    3
         2     2       0    4    2
         3     7       0    3    2
 11      1     5       0    7    7
         2     6       0    4    7
         3     7       0    3    6
 12      1     3       0    9   10
         2     5      10    8    8
         3     7       0    7    7
 13      1     3       7    7   10
         2     7       2    7    9
         3    10       0    7    7
 14      1     1       5    9    9
         2     4       2    6    6
         3     8       0    4    5
 15      1     1       6    2    9
         2     2       0    2    9
         3     6       0    2    8
 16      1     4       6    9    5
         2     6       3    8    4
         3     6       0    9    3
 17      1     6       3    9    6
         2     7       3    8    3
         3     7       0    8    4
 18      1     0       0    0    0
************************************************************************
RESOURCEAVAILABILITIES:
  R 1  N 1  N 2
   19   98  102
************************************************************************
