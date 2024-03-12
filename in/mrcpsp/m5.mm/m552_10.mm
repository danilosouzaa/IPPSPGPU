************************************************************************
file with basedata            : cm552_.bas
initial value random generator: 817110777
************************************************************************
projects                      :  1
jobs (incl. supersource/sink ):  18
horizon                       :  148
RESOURCES
  - renewable                 :  2   R
  - nonrenewable              :  2   N
  - doubly constrained        :  0   D
************************************************************************
PROJECT INFORMATION:
pronr.  #jobs rel.date duedate tardcost  MPM-Time
    1     16      0       19       14       19
************************************************************************
PRECEDENCE RELATIONS:
jobnr.    #modes  #successors   successors
   1        1          3           2   3   4
   2        5          3           5   7  16
   3        5          3          13  16  17
   4        5          3           5   6   8
   5        5          3           9  10  14
   6        5          3          10  12  16
   7        5          2           8  10
   8        5          3           9  14  17
   9        5          1          11
  10        5          2          15  17
  11        5          1          12
  12        5          1          13
  13        5          1          15
  14        5          1          15
  15        5          1          18
  16        5          1          18
  17        5          1          18
  18        1          0        
************************************************************************
REQUESTS/DURATIONS:
jobnr. mode duration  R 1  R 2  N 1  N 2
------------------------------------------------------------------------
  1      1     0       0    0    0    0
  2      1     3       0    3    3    9
         2     4       9    0    3    9
         3     5       8    0    3    8
         4     6       6    0    2    7
         5    10       0    3    2    7
  3      1     1       5    0    5    7
         2     2       0    3    3    6
         3     6       4    0    3    6
         4     8       0    3    3    5
         5    10       1    0    1    5
  4      1     2       0    7    2    3
         2     3       0    6    2    3
         3     7       7    0    2    2
         4     7       6    0    2    3
         5    10       0    6    1    2
  5      1     4       0    6    9    9
         2     4       4    0    8    9
         3     5       3    0    7    9
         4     8       3    0    6    9
         5     9       0    5    4    9
  6      1     5       0    7    5    4
         2     6       3    0    4    3
         3     6       0    4    4    3
         4     7       0    3    3    3
         5     7       3    0    3    3
  7      1     2       0    3    9    7
         2     2       0    3   10    6
         3     8       0    2    9    6
         4     9       0    2    9    5
         5    10       6    0    8    5
  8      1     3       0    9    6    3
         2     3       3    0    6    3
         3     3       0   10    4    3
         4     7       0    9    3    3
         5     9       0    8    2    2
  9      1     3       0    9    9    9
         2     6       0    9    6    8
         3     8       5    0    5    6
         4     8       0    6    6    5
         5     9       4    0    4    3
 10      1     4       8    0    5    7
         2     8       6    0    4    6
         3     8       7    0    4    5
         4     9       6    0    4    3
         5    10       0   10    4    3
 11      1     5       9    0    8    8
         2     6       0   10    7    7
         3     6       5    0    6    8
         4     8       0   10    6    7
         5     9       3    0    2    5
 12      1     1       7    0   10    6
         2     2       0    8    8    6
         3     3       4    0    6    6
         4     3       0    7    7    5
         5     7       0    6    4    5
 13      1     1       0    5    7    9
         2     3       0    5    6    7
         3     5      10    0    6    7
         4     9       9    0    5    5
         5    10       9    0    4    4
 14      1     1       0    4    6    6
         2     5       8    0    6    6
         3     5       0    4    5    6
         4     7       5    0    5    5
         5     8       0    2    4    5
 15      1     1       5    0   10   10
         2     4       5    0   10    9
         3     7       4    0    9    8
         4    10       4    0    8    8
         5    10       0    8    8    8
 16      1     1      10    0    6   10
         2     5      10    0    6    9
         3     7       0    8    6    9
         4     8       9    0    6    8
         5    10       9    0    6    7
 17      1     1       6    0    5    8
         2     5       0    3    5    7
         3     6       6    0    5    6
         4     6       0    2    5    7
         5    10       0    1    5    4
 18      1     0       0    0    0    0
************************************************************************
RESOURCEAVAILABILITIES:
  R 1  R 2  N 1  N 2
   21   31   95  106
************************************************************************
