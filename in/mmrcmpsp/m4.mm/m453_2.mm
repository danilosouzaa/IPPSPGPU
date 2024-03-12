************************************************************************
file with basedata            : cm453_.bas
initial value random generator: 1059434452
************************************************************************
projects                      :  1
jobs (incl. supersource/sink ):  18
horizon                       :  138
RESOURCES
  - renewable                 :  2   R
  - nonrenewable              :  2   N
  - doubly constrained        :  0   D
************************************************************************
PROJECT INFORMATION:
pronr.  #jobs rel.date duedate tardcost  MPM-Time
    1     16      0       20        7       20
************************************************************************
PRECEDENCE RELATIONS:
jobnr.    #modes  #successors   successors
   1        1          3           2   3   4
   2        4          2           5   6
   3        4          3           7   8  14
   4        4          2          13  16
   5        4          3          10  11  13
   6        4          2           8  11
   7        4          3          10  12  17
   8        4          1           9
   9        4          3          10  12  13
  10        4          1          16
  11        4          2          12  14
  12        4          1          15
  13        4          2          15  17
  14        4          2          16  17
  15        4          1          18
  16        4          1          18
  17        4          1          18
  18        1          0        
************************************************************************
REQUESTS/DURATIONS:
jobnr. mode duration  R 1  R 2  N 1  N 2
------------------------------------------------------------------------
  1      1     0       0    0    0    0
  2      1     3       9   10    9   10
         2     4       7    9    7    9
         3     5       4    8    7    8
         4    10       4    7    4    5
  3      1     1       8    8    6    8
         2     1       9    7    4    8
         3     1       8    9    6    7
         4    10       6    4    1    6
  4      1     3      10    8    8    9
         2     6       7    7    8    9
         3     7       7    7    6    9
         4     9       4    7    3    9
  5      1     8      10    7   10    7
         2     9       9    7    7    6
         3    10       9    6    6    5
         4    10       9    5    6    6
  6      1     2       6    8    8   10
         2     2       7    8    8    8
         3     5       6    7    8    4
         4     7       6    5    7    3
  7      1     1       6    6    6    9
         2     1       7    6    5    8
         3     5       5    6    4    6
         4     6       4    5    2    6
  8      1     4       7   10    6    6
         2     5       4   10    5    6
         3     5       5   10    4    6
         4     7       3   10    2    6
  9      1     2       5    4    8    8
         2     3       5    3    7    8
         3     6       3    3    5    8
         4     7       2    1    3    7
 10      1     2       9   10    7    6
         2     4       8    9    6    5
         3    10       5    9    6    1
         4    10       5    9    5    3
 11      1     5       7    3    7    9
         2     7       7    3    6    8
         3     8       4    3    6    8
         4     9       4    2    5    6
 12      1     1       9    7    3    5
         2     2       9    6    3    3
         3     2       8    5    3    4
         4    10       8    5    3    3
 13      1     2       8    8    7   10
         2     3       7    6    6    7
         3     6       7    5    3    4
         4     6       6    5    5    6
 14      1     1       3    5    6    4
         2     6       2    4    5    2
         3     8       2    4    3    1
         4     8       2    3    4    2
 15      1     1       3    9    8    7
         2     3       3    9    7    5
         3     8       2    6    6    4
         4     9       2    6    5    3
 16      1     1       7    7   10    7
         2     9       7    5    9    4
         3     9       7    4   10    4
         4    10       7    4    9    2
 17      1     3       8    9    9    2
         2     5       8    8    9    2
         3     8       8    6    9    2
         4    10       7    5    8    1
 18      1     0       0    0    0    0
************************************************************************
RESOURCEAVAILABILITIES:
  R 1  R 2  N 1  N 2
   14   14  106  105
************************************************************************
