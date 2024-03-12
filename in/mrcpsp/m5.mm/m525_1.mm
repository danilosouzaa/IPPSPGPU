************************************************************************
file with basedata            : cm525_.bas
initial value random generator: 18028
************************************************************************
projects                      :  1
jobs (incl. supersource/sink ):  18
horizon                       :  134
RESOURCES
  - renewable                 :  2   R
  - nonrenewable              :  2   N
  - doubly constrained        :  0   D
************************************************************************
PROJECT INFORMATION:
pronr.  #jobs rel.date duedate tardcost  MPM-Time
    1     16      0       12        0       12
************************************************************************
PRECEDENCE RELATIONS:
jobnr.    #modes  #successors   successors
   1        1          3           2   3   4
   2        5          3           5   9  13
   3        5          3           6   7  11
   4        5          2          11  12
   5        5          3           6   7  14
   6        5          2           8  10
   7        5          3           8  16  17
   8        5          1          15
   9        5          2          12  17
  10        5          2          12  16
  11        5          2          14  16
  12        5          1          15
  13        5          2          14  15
  14        5          1          17
  15        5          1          18
  16        5          1          18
  17        5          1          18
  18        1          0        
************************************************************************
REQUESTS/DURATIONS:
jobnr. mode duration  R 1  R 2  N 1  N 2
------------------------------------------------------------------------
  1      1     0       0    0    0    0
  2      1     1      10    0    7    0
         2     4       0    2    0    8
         3     6       0    1    6    0
         4     7       8    0    0    6
         5     8       0    1    0    5
  3      1     3       0    7    0    9
         2     5       0    5    0    7
         3     5       8    0    0    8
         4     8       7    0    0    6
         5     9       3    0    6    0
  4      1     1       1    0    0    4
         2     1       1    0   10    0
         3     2       0    5    0    4
         4     5       0    4    6    0
         5    10       0    3    0    4
  5      1     1       8    0    2    0
         2     5       8    0    0    6
         3     7       0    5    1    0
         4     9       4    0    0    5
         5    10       0    3    0    4
  6      1     1      10    0   10    0
         2     5       0    8    0    1
         3     6       8    0    9    0
         4     7       8    0    0    1
         5    10       6    0    9    0
  7      1     4       7    0    0    7
         2     5       0    5    0    7
         3     5       5    0    6    0
         4     6       3    0    0    7
         5     7       3    0    5    0
  8      1     2       9    0    7    0
         2     4       8    0    7    0
         3     5       0    5    0    6
         4     6       0    4    0    5
         5     6       8    0    4    0
  9      1     1       7    0    0    9
         2     4       5    0    0    9
         3     6       0    4    3    0
         4     6       2    0    0    7
         5     7       0    4    0    5
 10      1     3       0    8    9    0
         2     8       9    0    0    9
         3     9       0    4    6    0
         4    10       8    0    2    0
         5    10       8    0    0    7
 11      1     2       0   10    7    0
         2     5       9    0    6    0
         3     6       0    7    5    0
         4     6       0    7    0    9
         5     8       5    0    6    0
 12      1     3       9    0    0    7
         2     4       7    0    0    5
         3     6       6    0    8    0
         4     8       6    0    7    0
         5     8       0    7    0    4
 13      1     1       9    0    0    2
         2     3       8    0    6    0
         3     4       0    6    0    1
         4    10       0    5    0    1
         5    10       7    0    0    1
 14      1     1       3    0    8    0
         2     1       2    0   10    0
         3     3       0    4    0    7
         4     5       0    3    0    5
         5     9       0    3    4    0
 15      1     2       4    0    7    0
         2     3       0    3    7    0
         3     5       3    0    6    0
         4     7       0    1    6    0
         5     8       3    0    5    0
 16      1     2       0    6    0    5
         2     3       9    0    7    0
         3     4       7    0    0    3
         4     4       0    4    0    4
         5     6       6    0    0    2
 17      1     3       6    0    0    7
         2     6       6    0    6    0
         3     7       0   10    0    6
         4     8       6    0    5    0
         5     8       0    6    3    0
 18      1     0       0    0    0    0
************************************************************************
RESOURCEAVAILABILITIES:
  R 1  R 2  N 1  N 2
    8    7  111   96
************************************************************************
