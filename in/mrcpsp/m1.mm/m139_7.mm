************************************************************************
file with basedata            : cm139_.bas
initial value random generator: 1727101773
************************************************************************
projects                      :  1
jobs (incl. supersource/sink ):  18
horizon                       :  81
RESOURCES
  - renewable                 :  2   R
  - nonrenewable              :  2   N
  - doubly constrained        :  0   D
************************************************************************
PROJECT INFORMATION:
pronr.  #jobs rel.date duedate tardcost  MPM-Time
    1     16      0       31       10       31
************************************************************************
PRECEDENCE RELATIONS:
jobnr.    #modes  #successors   successors
   1        1          3           2   3   4
   2        1          3           5   9  15
   3        1          3           6   8  13
   4        1          2           7  10
   5        1          2          12  14
   6        1          3           9  11  16
   7        1          3           8   9  16
   8        1          3          11  14  15
   9        1          2          12  14
  10        1          2          13  16
  11        1          1          12
  12        1          1          17
  13        1          1          15
  14        1          1          17
  15        1          1          18
  16        1          1          18
  17        1          1          18
  18        1          0        
************************************************************************
REQUESTS/DURATIONS:
jobnr. mode duration  R 1  R 2  N 1  N 2
------------------------------------------------------------------------
  1      1     0       0    0    0    0
  2      1     2       5    8    2    5
  3      1     8       6    7    6    7
  4      1     2       5   10    1    9
  5      1     4       3    9    7    2
  6      1     1       2    6    1    4
  7      1     1       8    8    2    4
  8      1    10       8    4    7    4
  9      1     2       2    5    2    5
 10      1     8       5    4    7   10
 11      1     1       6    4    8    5
 12      1     2       5    2    3    9
 13      1     9       7    6    9    5
 14      1     8       2    5    2    9
 15      1     8       4    4    4    8
 16      1    10       6    6    4    7
 17      1     5       7    4    9    7
 18      1     0       0    0    0    0
************************************************************************
RESOURCEAVAILABILITIES:
  R 1  R 2  N 1  N 2
   19   24   74  100
************************************************************************
