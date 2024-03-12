************************************************************************
file with basedata            : c1562_.bas
initial value random generator: 30556
************************************************************************
projects                      :  1
jobs (incl. supersource/sink ):  18
horizon                       :  133
RESOURCES
  - renewable                 :  2   R
  - nonrenewable              :  2   N
  - doubly constrained        :  0   D
************************************************************************
PROJECT INFORMATION:
pronr.  #jobs rel.date duedate tardcost  MPM-Time
    1     16      0       24        4       24
************************************************************************
PRECEDENCE RELATIONS:
jobnr.    #modes  #successors   successors
   1        1          3           2   3   4
   2        3          2           9  17
   3        3          1           5
   4        3          3           6  10  17
   5        3          2           8  12
   6        3          2           7   9
   7        3          1          16
   8        3          2           9  17
   9        3          2          11  14
  10        3          2          13  14
  11        3          1          16
  12        3          1          15
  13        3          1          15
  14        3          1          15
  15        3          1          18
  16        3          1          18
  17        3          1          18
  18        1          0        
************************************************************************
REQUESTS/DURATIONS:
jobnr. mode duration  R 1  R 2  N 1  N 2
------------------------------------------------------------------------
  1      1     0       0    0    0    0
  2      1     4       6    6    3    6
         2     5       5    5    3    6
         3     6       4    5    3    6
  3      1     3       4    2    6    8
         2     4       4    2    6    6
         3    10       4    1    5    3
  4      1     3       3    6    6    9
         2     6       3    5    4    9
         3     7       2    4    1    8
  5      1     1       4    9    5    8
         2     5       3    9    5    8
         3     8       3    7    3    8
  6      1     5      10    8   10    8
         2     5      10   10    9    6
         3     9      10    7    8    5
  7      1     2       6    3    6    7
         2     3       5    3    3    7
         3     6       2    2    3    6
  8      1     4       9    4    9    6
         2     6       7    4    8    5
         3     8       6    4    7    4
  9      1     6       8    7    6    8
         2     9       5    5    4    8
         3    10       5    2    1    8
 10      1     3      10    9    8    8
         2     6      10    7    5    5
         3     9      10    7    3    4
 11      1     6       6    7    7   10
         2     9       5    5    6    5
         3    10       4    3    2    5
 12      1     2       7    6   10    7
         2    10       2    4    9    7
         3    10       5    5    8    6
 13      1     1       6    4    2    5
         2     5       5    3    2    5
         3     6       2    3    1    5
 14      1     1       7   10    8    5
         2     8       3   10    6    5
         3     8       4   10    5    5
 15      1     3       4    9    7    9
         2     6       4    5    5    7
         3     7       3    3    2    6
 16      1     4       6    6    5    4
         2     9       5    6    4    3
         3    10       5    2    2    3
 17      1     5       4    5    6    7
         2     5       4    6    5    7
         3     9       2    4    4    7
 18      1     0       0    0    0    0
************************************************************************
RESOURCEAVAILABILITIES:
  R 1  R 2  N 1  N 2
   23   22  104  115
************************************************************************
