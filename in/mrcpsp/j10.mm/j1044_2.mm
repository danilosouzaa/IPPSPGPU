************************************************************************
file with basedata            : mm44_.bas
initial value random generator: 1774024113
************************************************************************
projects                      :  1
jobs (incl. supersource/sink ):  12
horizon                       :  82
RESOURCES
  - renewable                 :  2   R
  - nonrenewable              :  2   N
  - doubly constrained        :  0   D
************************************************************************
PROJECT INFORMATION:
pronr.  #jobs rel.date duedate tardcost  MPM-Time
    1     10      0       15        4       15
************************************************************************
PRECEDENCE RELATIONS:
jobnr.    #modes  #successors   successors
   1        1          3           2   3   4
   2        3          2           7   8
   3        3          1          11
   4        3          2           5   7
   5        3          3           6   8  10
   6        3          2           9  11
   7        3          1           9
   8        3          1          11
   9        3          1          12
  10        3          1          12
  11        3          1          12
  12        1          0        
************************************************************************
REQUESTS/DURATIONS:
jobnr. mode duration  R 1  R 2  N 1  N 2
------------------------------------------------------------------------
  1      1     0       0    0    0    0
  2      1     2      10    7    7    0
         2     7       9    7    0   10
         3     9       7    7    2    0
  3      1     1       7    5    0    7
         2     8       7    5    8    0
         3     8       5    3    0    5
  4      1     7       5    1    0    9
         2     8       5    1    0    6
         3    10       3    1    9    0
  5      1     2       4    5    0    7
         2     6       3    4    6    0
         3     9       2    3    0    3
  6      1     3      10    6    4    0
         2     3       9    6    0    8
         3     4       6    3    0    4
  7      1     1       6    9    2    0
         2     1       4    9    0    3
         3     4       3    9    0    3
  8      1     3       7   10    0    5
         2     3       6    8    0    6
         3     9       5    6    9    0
  9      1     3       7    3    0    4
         2     5       7    3    0    3
         3     9       5    3    0    1
 10      1     3      10    8    1    0
         2    10       9    7    1    0
         3    10       9    7    0    6
 11      1     1       6    9    0    8
         2     7       3    9    0    6
         3    10       2    8    0    2
 12      1     0       0    0    0    0
************************************************************************
RESOURCEAVAILABILITIES:
  R 1  R 2  N 1  N 2
   18   17   46   68
************************************************************************
