************************************************************************
file with basedata            : cr528_.bas
initial value random generator: 1792386119
************************************************************************
projects                      :  1
jobs (incl. supersource/sink ):  18
horizon                       :  116
RESOURCES
  - renewable                 :  5   R
  - nonrenewable              :  2   N
  - doubly constrained        :  0   D
************************************************************************
PROJECT INFORMATION:
pronr.  #jobs rel.date duedate tardcost  MPM-Time
    1     16      0       19       15       19
************************************************************************
PRECEDENCE RELATIONS:
jobnr.    #modes  #successors   successors
   1        1          3           2   3   4
   2        3          3           7  13  14
   3        3          3           5   6  10
   4        3          2          11  13
   5        3          3           7   9  12
   6        3          3           8  12  14
   7        3          1          17
   8        3          3           9  11  16
   9        3          1          15
  10        3          2          11  12
  11        3          1          15
  12        3          2          16  17
  13        3          2          16  17
  14        3          1          15
  15        3          1          18
  16        3          1          18
  17        3          1          18
  18        1          0        
************************************************************************
REQUESTS/DURATIONS:
jobnr. mode duration  R 1  R 2  R 3  R 4  R 5  N 1  N 2
------------------------------------------------------------------------
  1      1     0       0    0    0    0    0    0    0
  2      1     2       0   10    0    0    0    8    0
         2     3       0    0    7    0    0    0    1
         3     6       0    8    1    8    0    5    0
  3      1     1       0    0    0    8    4    0    5
         2     3       9    2    3    6    2    6    0
         3     8       0    0    3    0    0    3    0
  4      1     2       0    6    8    6    0    0    5
         2     8       9    0    0    0    1    6    0
         3     8       8    0    0    4    0    0    5
  5      1     5       0    5    7    0    0    0    6
         2     6       0    2    4    0    0    0    3
         3     7       3    0    0    6    3    3    0
  6      1     7       5    8    0    0    7    0    3
         2     8       0    0    0    7    0    7    0
         3    10       0    6    8    6    0    7    0
  7      1     3       0    5    0    0    0    0    6
         2     8       0    0    2    0    0    4    0
         3    10       0    5    0    0    7    0    4
  8      1     3       0    8    6    6    0    0    9
         2     6       0    5    6    5    8    5    0
         3     8       9    5    4    0    0    0    2
  9      1     3       0    0    9    4    0    4    0
         2     5       0    8    0    0    7    4    0
         3     7       0    5    6    0    2    3    0
 10      1     2       6    0    7    0    0    7    0
         2     3       3    4    0    4    0    0    6
         3     5       0    3    0    0    0    5    0
 11      1     3       5   10    0    8    5    0    6
         2     7       5    9    0    8    5    0    3
         3     7       0    0    8    7    4    7    0
 12      1     7      10    0    0    0    3   10    0
         2     7       0    9    0    6    0    0    6
         3    10       7    8    0    1    0    6    0
 13      1     2       7    0    6    0    0    0    2
         2     4       0    8    4    6   10    0    2
         3     6       5    7    0    0    0    0    1
 14      1     7       0    8    9    8    8    1    0
         2     8       7    0    0    7    0    1    0
         3    10       0    0    5    3    0    1    0
 15      1     1       7    6   10    4    4    0    6
         2     3       7    0    9    3    0    8    0
         3     3       0    0    0    1    4    7    0
 16      1     1       0    8    6    6    0    0    9
         2     3       0    0    5    0    3    6    0
         3     3       0    7    6    0   10    0    5
 17      1     4       0    0    4    4    0    6    0
         2     7       0    0    0    2    0    6    0
         3     8       2    5    3    2    2    0    7
 18      1     0       0    0    0    0    0    0    0
************************************************************************
RESOURCEAVAILABILITIES:
  R 1  R 2  R 3  R 4  R 5  N 1  N 2
   26   35   28   31   36   88   77
************************************************************************
