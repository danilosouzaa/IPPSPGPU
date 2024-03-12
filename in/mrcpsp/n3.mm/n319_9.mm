************************************************************************
file with basedata            : cn319_.bas
initial value random generator: 524896965
************************************************************************
projects                      :  1
jobs (incl. supersource/sink ):  18
horizon                       :  125
RESOURCES
  - renewable                 :  2   R
  - nonrenewable              :  3   N
  - doubly constrained        :  0   D
************************************************************************
PROJECT INFORMATION:
pronr.  #jobs rel.date duedate tardcost  MPM-Time
    1     16      0       20        5       20
************************************************************************
PRECEDENCE RELATIONS:
jobnr.    #modes  #successors   successors
   1        1          3           2   3   4
   2        3          3           5   7  10
   3        3          3           9  11  12
   4        3          2          14  16
   5        3          3           6   9  15
   6        3          2          16  17
   7        3          3           8  12  13
   8        3          2           9  11
   9        3          1          14
  10        3          3          11  12  16
  11        3          1          14
  12        3          2          15  17
  13        3          1          15
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
  2      1     3       8    0    0    7    0
         2     6       5    0    9    0    5
         3     8       2    0    7    6    5
  3      1     2       4    0    8    0    9
         2     3       0    5    8    7    9
         3     9       0    4    7    7    8
  4      1     6       8    0    0    7    0
         2     8       0   10    9    0    0
         3     9       5    0    8    0    0
  5      1     4      10    0    6    8    0
         2     7       7    0    3    4    3
         3    10       0    7    0    0    1
  6      1     2       0    6    0    8    9
         2     3       0    4    7    0    9
         3     8       6    0    0    0    9
  7      1     7       0    9    0    0    4
         2     9       9    0    0    8    0
         3    10       0    1    5    0    0
  8      1     2       7    0    7    0    0
         2     8       0    4    0    4    7
         3     9       6    0    6    0    7
  9      1     2       3    0    8    0    2
         2     3       0    2    0    9    0
         3     4       0    2    7    0    0
 10      1     2       4    0    0    4    0
         2     3       0    3    9    3    0
         3     5       0    3    0    2    3
 11      1     4       6    0    2    0    0
         2     4       0    7    4    0    0
         3     6       6    0    0    7    0
 12      1     4       4    0    0    8    6
         2     8       0   10    3    7    0
         3     9       0    3    1    3    0
 13      1     2       0    2    0    7    9
         2     6      10    0    0    4    0
         3     6      10    0    1    0    3
 14      1     2       5    0    0    7    0
         2     5       1    0    0    2    0
         3     8       0    9    0    0    2
 15      1     3       0    5    0    6   10
         2     6       2    0    8    0    0
         3     8       0    4    5    5    0
 16      1     2       3    0    0    6    0
         2     6       2    0    0    4    0
         3     8       1    0   10    0    0
 17      1     2       5    0    0    0    4
         2     4       0    9    0    0    2
         3     8       4    0    0    0    1
 18      1     0       0    0    0    0    0
************************************************************************
RESOURCEAVAILABILITIES:
  R 1  R 2  N 1  N 2  N 3
   24   22   72   79   59
************************************************************************
