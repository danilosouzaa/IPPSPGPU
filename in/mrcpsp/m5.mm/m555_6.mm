************************************************************************
file with basedata            : cm555_.bas
initial value random generator: 1739815562
************************************************************************
projects                      :  1
jobs (incl. supersource/sink ):  18
horizon                       :  135
RESOURCES
  - renewable                 :  2   R
  - nonrenewable              :  2   N
  - doubly constrained        :  0   D
************************************************************************
PROJECT INFORMATION:
pronr.  #jobs rel.date duedate tardcost  MPM-Time
    1     16      0       18        4       18
************************************************************************
PRECEDENCE RELATIONS:
jobnr.    #modes  #successors   successors
   1        1          3           2   3   4
   2        5          2           9  13
   3        5          3           5   6   7
   4        5          3           8  11  16
   5        5          3           9  11  12
   6        5          2           8  17
   7        5          3           9  10  14
   8        5          1          10
   9        5          3          15  16  17
  10        5          1          15
  11        5          1          13
  12        5          2          14  16
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
  2      1     1       8   10    5    8
         2     1       6    9    6    7
         3     3       6    9    5    6
         4     5       3    7    2    6
         5    10       1    5    1    3
  3      1     2       4    2   10    6
         2     6       4    2    9    6
         3     7       3    2    7    5
         4     9       3    1    6    4
         5    10       2    1    5    4
  4      1     1      10    5   10    3
         2     3       7    5    9    3
         3     6       7    4    7    2
         4     9       4    4    7    2
         5    10       4    4    6    1
  5      1     1       9    3    3    7
         2     2       7    2    3    5
         3     4       6    2    3    5
         4     9       4    1    1    2
         5     9       3    1    2    3
  6      1     6       7    7    8    4
         2     7       6    6    8    4
         3     7       7    7    8    3
         4     8       6    5    8    3
         5     8       6    6    8    2
  7      1     5       6    7    6    8
         2     6       6    6    6    7
         3     8       6    3    3    5
         4     8       6    5    3    4
         5     9       6    1    2    4
  8      1     1       8    6    7    7
         2     2       6    5    5    5
         3     3       6    4    5    3
         4     3       4    5    4    3
         5     5       2    3    4    2
  9      1     3       2    5    7    6
         2     8       2    5    5    6
         3     9       2    3    5    6
         4    10       2    1    2    5
         5    10       2    2    1    5
 10      1     2       8    4   10    3
         2     4       7    3    8    2
         3     5       7    3    6    2
         4     6       6    2    5    2
         5     7       6    2    5    1
 11      1     7       5    7    8    8
         2     7       5    6    7    9
         3     8       2    5    4    7
         4     9       1    4    4    5
         5     9       1    5    3    7
 12      1     2       9   10    6    6
         2     2       8    9    7    6
         3     5       8    9    6    6
         4     6       8    9    5    6
         5     8       6    8    4    5
 13      1     2       7    9    7    7
         2     3       7    8    7    6
         3     4       7    8    4    6
         4     5       6    8    3    5
         5    10       6    7    2    5
 14      1     4       5    8    9    6
         2     5       4    6    8    6
         3     5       5    5    9    6
         4     7       4    4    8    6
         5     8       4    1    8    5
 15      1     2       6    4   10    8
         2     3       6    4    8    8
         3     5       6    3    7    7
         4     5       6    4    6    7
         5     8       6    3    5    6
 16      1     1       8    6    9    4
         2     5       6    3    6    4
         3     8       5    3    3    3
         4     8       5    2    3    4
         5     8       6    1    3    3
 17      1     2       9    9    7    2
         2     3       7    8    7    2
         3     4       7    8    4    1
         4     5       6    7    3    1
         5     6       5    7    2    1
 18      1     0       0    0    0    0
************************************************************************
RESOURCEAVAILABILITIES:
  R 1  R 2  N 1  N 2
   22   25  108   84
************************************************************************
