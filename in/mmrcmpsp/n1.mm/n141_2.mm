************************************************************************
file with basedata            : cn141_.bas
initial value random generator: 577166081
************************************************************************
projects                      :  1
jobs (incl. supersource/sink ):  18
horizon                       :  134
RESOURCES
  - renewable                 :  2   R
  - nonrenewable              :  1   N
  - doubly constrained        :  0   D
************************************************************************
PROJECT INFORMATION:
pronr.  #jobs rel.date duedate tardcost  MPM-Time
    1     16      0       25        9       25
************************************************************************
PRECEDENCE RELATIONS:
jobnr.    #modes  #successors   successors
   1        1          3           2   3   4
   2        3          3           7   8  13
   3        3          3           6   9  16
   4        3          3           5   6  11
   5        3          3           8  10  16
   6        3          2           8  10
   7        3          3          15  16  17
   8        3          1          14
   9        3          2          11  12
  10        3          1          12
  11        3          2          15  17
  12        3          2          13  17
  13        3          1          14
  14        3          1          15
  15        3          1          18
  16        3          1          18
  17        3          1          18
  18        1          0        
************************************************************************
REQUESTS/DURATIONS:
jobnr. mode duration  R 1  R 2  N 1
------------------------------------------------------------------------
  1      1     0       0    0    0
  2      1     2       0    5    7
         2     4       4    0    4
         3     7       1    0    3
  3      1     3       7    0    2
         2     6       3    0    1
         3     9       0    1    1
  4      1     7       0    3    7
         2     8       5    0    5
         3    10       0    2    5
  5      1     3       0   10    2
         2     8       1    0    1
         3    10       0    2    1
  6      1     1       0    8    8
         2     3       0    7    7
         3     6       0    6    7
  7      1     5       6    0    5
         2     5       5    0    7
         3     8       0    4    3
  8      1     4       8    0    7
         2     6       0    4    3
         3     8       4    0    3
  9      1     9       0    9    5
         2    10       4    0    5
         3    10       0    7    5
 10      1     1       0    6    8
         2     5       0    3    8
         3     9       3    0    5
 11      1     3       0    6    8
         2     5       0    5    3
         3     9       7    0    1
 12      1     3       8    0    7
         2     8       0    4    6
         3     8       4    0    5
 13      1     1       0   10    4
         2     6       0    5    3
         3     7       0    4    3
 14      1     3       0    8    3
         2     4       0    8    2
         3    10       0    5    1
 15      1     6       0    3    6
         2    10       2    0    6
         3    10       5    0    5
 16      1     2       0    2    9
         2     4       0    2    8
         3     4       2    0    8
 17      1     4       0    9    2
         2     8       0    8    1
         3     9       6    0    1
 18      1     0       0    0    0
************************************************************************
RESOURCEAVAILABILITIES:
  R 1  R 2  N 1
    5   12   75
************************************************************************
