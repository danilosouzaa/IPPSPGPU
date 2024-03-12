************************************************************************
file with basedata            : cn353_.bas
initial value random generator: 2127900102
************************************************************************
projects                      :  1
jobs (incl. supersource/sink ):  18
horizon                       :  124
RESOURCES
  - renewable                 :  2   R
  - nonrenewable              :  3   N
  - doubly constrained        :  0   D
************************************************************************
PROJECT INFORMATION:
pronr.  #jobs rel.date duedate tardcost  MPM-Time
    1     16      0       22        2       22
************************************************************************
PRECEDENCE RELATIONS:
jobnr.    #modes  #successors   successors
   1        1          3           2   3   4
   2        3          3           5   6   7
   3        3          3           8  10  15
   4        3          3           8  11  15
   5        3          2           9  11
   6        3          2          12  17
   7        3          3           8   9  15
   8        3          2          12  14
   9        3          3          10  13  14
  10        3          2          16  17
  11        3          1          12
  12        3          1          16
  13        3          1          16
  14        3          1          17
  15        3          1          18
  16        3          1          18
  17        3          1          18
  18        1          0        
************************************************************************
REQUESTS/DURATIONS:
jobnr. mode duration  R 1  R 2  N 1  N 2  N 3
------------------------------------------------------------------------
  1      1     0       0    0    0    0    0
  2      1     3       8    7    7    7    7
         2     3       9    6    7    7    7
         3     7       8    4    7    7    7
  3      1     2       7    9    2    6    5
         2     6       7    7    2    6    4
         3     8       5    6    1    2    3
  4      1     3       7    9    7    9    2
         2     5       6    8    7    9    1
         3     9       4    6    6    7    1
  5      1     1       2    4    3    1    7
         2     7       1    3    2    1    3
         3     7       2    1    2    1    6
  6      1     1       8    9    8    4    7
         2     6       7    8    5    4    6
         3     7       6    6    3    1    6
  7      1     5       8    8    7    5    6
         2     6       8    6    7    4    4
         3     9       8    3    4    4    3
  8      1     4       7   10    6    2   10
         2     6       4    9    4    1    8
         3    10       4    8    4    1    7
  9      1     2       5    6    2    6    3
         2     3       2    3    1    5    2
         3     3       2    4    2    4    3
 10      1     3       5    3    5    8    9
         2     8       3    2    5    5    7
         3     9       2    2    5    1    4
 11      1     2       7   10    8    5   10
         2     7       6    7    7    5    9
         3     8       6    4    7    4    9
 12      1     1       7    6    4    9    6
         2     2       6    4    2    6    3
         3     6       1    4    1    3    2
 13      1     5       6    6    7    4    6
         2     6       5    3    7    3    5
         3     7       5    2    4    2    2
 14      1     8       9    5   10    5    5
         2     8       6    7   10    5    4
         3    10       5    1    7    3    2
 15      1     2      10   10    9    5    8
         2     5       9    7    7    4    4
         3     8       9    6    7    3    3
 16      1     1       6    7    4    8    6
         2     4       4    5    3    6    6
         3     9       4    4    2    3    6
 17      1     2       6    3    5   10    7
         2     3       5    3    5   10    5
         3     7       5    1    5    9    5
 18      1     0       0    0    0    0    0
************************************************************************
RESOURCEAVAILABILITIES:
  R 1  R 2  N 1  N 2  N 3
   14   13   87   84   94
************************************************************************
