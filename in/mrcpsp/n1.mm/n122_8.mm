************************************************************************
file with basedata            : cn122_.bas
initial value random generator: 1851391296
************************************************************************
projects                      :  1
jobs (incl. supersource/sink ):  18
horizon                       :  132
RESOURCES
  - renewable                 :  2   R
  - nonrenewable              :  1   N
  - doubly constrained        :  0   D
************************************************************************
PROJECT INFORMATION:
pronr.  #jobs rel.date duedate tardcost  MPM-Time
    1     16      0       14       11       14
************************************************************************
PRECEDENCE RELATIONS:
jobnr.    #modes  #successors   successors
   1        1          3           2   3   4
   2        3          3           6  12  15
   3        3          3           5   9  10
   4        3          3           5   8  14
   5        3          3           7  11  12
   6        3          3           8  10  13
   7        3          2          15  16
   8        3          1          11
   9        3          3          11  15  17
  10        3          2          14  16
  11        3          1          16
  12        3          1          17
  13        3          1          14
  14        3          1          17
  15        3          1          18
  16        3          1          18
  17        3          1          18
  18        1          0        
************************************************************************
REQUESTS/DURATIONS:
jobnr. mode duration  R 1  R 2  N 1
------------------------------------------------------------------------
  1      1     0       0    0    0
  2      1     2       6    8    9
         2    10       5    6    9
         3    10       6    4    9
  3      1     1       9    8    9
         2     3       8    7    0
         3     6       7    4    9
  4      1     5       4    6    9
         2    10       4    4    0
         3    10       3    1    9
  5      1     4       8    3    0
         2     8       5    2    0
         3    10       4    2    8
  6      1     1       4    8    1
         2     5       4    7    0
         3     9       4    3    0
  7      1     1       6    8    0
         2     4       4    6    0
         3     5       3    1    2
  8      1     1       7    4    4
         2     6       3    3    4
         3     6       6    2    0
  9      1     2      10    4    2
         2     4       5    3    2
         3     9       3    3    1
 10      1     1       7    6    3
         2     6       4    6    0
         3     8       2    5    0
 11      1     1       6    7    5
         2     5       6    7    0
         3     7       6    6    0
 12      1     4       7    7    2
         2     8       7    5    0
         3     9       7    3    0
 13      1     4       8    4    0
         2     6       7    4    0
         3    10       5    3    0
 14      1     3       7    6    0
         2     6       7    3    6
         3     8       7    3    3
 15      1     1      10    7    2
         2     9       9    6    0
         3    10       9    3    2
 16      1     1       8    8    0
         2     5       4    5    2
         3     5       4    6    0
 17      1     1       9    5    7
         2     9       9    4    0
         3    10       8    2    0
 18      1     0       0    0    0
************************************************************************
RESOURCEAVAILABILITIES:
  R 1  R 2  N 1
   18   17   56
************************************************************************
