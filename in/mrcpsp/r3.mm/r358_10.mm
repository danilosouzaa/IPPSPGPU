************************************************************************
file with basedata            : cr358_.bas
initial value random generator: 696654044
************************************************************************
projects                      :  1
jobs (incl. supersource/sink ):  18
horizon                       :  123
RESOURCES
  - renewable                 :  3   R
  - nonrenewable              :  2   N
  - doubly constrained        :  0   D
************************************************************************
PROJECT INFORMATION:
pronr.  #jobs rel.date duedate tardcost  MPM-Time
    1     16      0       20        6       20
************************************************************************
PRECEDENCE RELATIONS:
jobnr.    #modes  #successors   successors
   1        1          3           2   3   4
   2        3          3           6   7   9
   3        3          3           6   8   9
   4        3          3           5   6  12
   5        3          1          11
   6        3          3          15  16  17
   7        3          3           8  11  17
   8        3          1          10
   9        3          1          17
  10        3          1          12
  11        3          3          13  14  16
  12        3          3          13  14  16
  13        3          1          15
  14        3          1          15
  15        3          1          18
  16        3          1          18
  17        3          1          18
  18        1          0        
************************************************************************
REQUESTS/DURATIONS:
jobnr. mode duration  R 1  R 2  R 3  N 1  N 2
------------------------------------------------------------------------
  1      1     0       0    0    0    0    0
  2      1     3       5    7    5    5    4
         2     4       3    5    0    4    4
         3     9       0    5    3    2    3
  3      1     2       2    0    9    9    4
         2     7       0    9    7    8    4
         3     8       0    6    0    8    3
  4      1     6       0    5    0    4    7
         2     6       4    0    7    4    8
         3     8       0    0    5    1    7
  5      1     1       2    8    0    6   10
         2     4       2    0    0    6    5
         3     5       2    0    0    5    2
  6      1     2       0    4    0    8    8
         2     8       4    0    9    6    4
         3     9       3    0    7    5    2
  7      1     3       7    6    6    4    8
         2     3       8    0    0    5    9
         3     6       0    0    5    4    3
  8      1     4       0    8    0    8    2
         2     5       0    5    0    5    2
         3     7       0    4    0    5    2
  9      1     1       0    0    4    4    8
         2     8       0    6    0    3    7
         3    10       2    0    0    3    4
 10      1     4       4    8    0    8    6
         2     4       0    1    4   10    6
         3     7       0    0    3    8    6
 11      1     1       0    5    0    7    4
         2     3       5    0    0    4    4
         3     9       0    3    1    2    3
 12      1     2       0    8    5    4    5
         2     6       0    0    5    3    5
         3     9       0    5    4    3    4
 13      1     1       0    7    0    7    3
         2     1       0    0    4    7    3
         3     8       2    0    4    4    2
 14      1     2       6    0    0    6    3
         2     4       4    0    0    6    3
         3     6       2    0    7    6    3
 15      1     2       3    0    0    5    9
         2     8       2    0   10    5    6
         3    10       2    6    0    4    3
 16      1     3       0    8    9    9    9
         2     4       0    8    0    8    6
         3     7       0    7    8    8    5
 17      1     2       6    4    8    7    6
         2     5       0    0    6    3    4
         3     5       5    0    0    4    4
 18      1     0       0    0    0    0    0
************************************************************************
RESOURCEAVAILABILITIES:
  R 1  R 2  R 3  N 1  N 2
    8   15   13  104   98
************************************************************************
