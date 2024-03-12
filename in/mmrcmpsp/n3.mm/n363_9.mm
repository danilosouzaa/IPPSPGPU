************************************************************************
file with basedata            : cn363_.bas
initial value random generator: 1882609184
************************************************************************
projects                      :  1
jobs (incl. supersource/sink ):  18
horizon                       :  128
RESOURCES
  - renewable                 :  2   R
  - nonrenewable              :  3   N
  - doubly constrained        :  0   D
************************************************************************
PROJECT INFORMATION:
pronr.  #jobs rel.date duedate tardcost  MPM-Time
    1     16      0       15        0       15
************************************************************************
PRECEDENCE RELATIONS:
jobnr.    #modes  #successors   successors
   1        1          3           2   3   4
   2        3          3           7   9  17
   3        3          3           7  13  15
   4        3          3           5   7  12
   5        3          3           6   8  17
   6        3          2          10  11
   7        3          1          10
   8        3          3          11  14  15
   9        3          3          10  11  12
  10        3          1          14
  11        3          1          13
  12        3          2          15  16
  13        3          1          16
  14        3          1          16
  15        3          1          18
  16        3          1          18
  17        3          1          18
  18        1          0        
************************************************************************
REQUESTS/DURATIONS:
jobnr. mode duration  R 1  R 2  N 1  N 2  N 3
------------------------------------------------------------------------
  1      1     0       0    0    0    0    0
  2      1     2       8   10    7    9    7
         2     3       7    9    6    6    7
         3     7       6    8    4    4    5
  3      1     2       3    2    6    5   10
         2     4       3    1    4    3    9
         3     8       2    1    3    1    9
  4      1     1       8    9    8   10    4
         2     3       7    8    7    8    4
         3     8       4    8    5    6    3
  5      1     3       8    5    8    9    3
         2     7       8    4    8    8    3
         3     8       8    4    7    7    2
  6      1     2       6    8    7    6    8
         2     3       5    6    4    6    7
         3    10       2    5    4    5    6
  7      1     1       8    6    6    7    6
         2     8       8    5    3    6    6
         3     9       5    2    2    4    5
  8      1     3       6    7    3    7    9
         2     5       5    7    3    5    6
         3     6       3    6    3    4    6
  9      1     1       6    4   10    5    3
         2     7       4    3    9    4    3
         3     8       2    2    9    4    2
 10      1     6       2    4    8    9    5
         2     8       2    3    6    6    3
         3     9       2    2    4    1    2
 11      1     4       7    7    4    7    3
         2     8       7    6    3    7    1
         3     8       7    6    4    6    1
 12      1     3       6    2    5    6    2
         2     5       4    1    5    4    2
         3    10       3    1    5    2    1
 13      1     3       7    4    9   10    6
         2     3       6    5    5   10    7
         3     6       5    2    1   10    3
 14      1     2       9    7    7   10    7
         2     5       5    7    7   10    6
         3     8       3    6    6   10    2
 15      1     2       9   10    2    2    8
         2     3       8    9    1    2    7
         3    10       6    8    1    2    6
 16      1     1       4    6    3    6    6
         2     5       3    6    2    6    6
         3     9       2    2    2    3    5
 17      1     2       7    4    9    8    5
         2     3       6    4    4    6    5
         3     4       4    3    2    4    5
 18      1     0       0    0    0    0    0
************************************************************************
RESOURCEAVAILABILITIES:
  R 1  R 2  N 1  N 2  N 3
   21   18  102  116   93
************************************************************************
