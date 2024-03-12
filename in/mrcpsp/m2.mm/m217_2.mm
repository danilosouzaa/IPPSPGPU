************************************************************************
file with basedata            : cm217_.bas
initial value random generator: 1124126561
************************************************************************
projects                      :  1
jobs (incl. supersource/sink ):  18
horizon                       :  111
RESOURCES
  - renewable                 :  2   R
  - nonrenewable              :  2   N
  - doubly constrained        :  0   D
************************************************************************
PROJECT INFORMATION:
pronr.  #jobs rel.date duedate tardcost  MPM-Time
    1     16      0       23        4       23
************************************************************************
PRECEDENCE RELATIONS:
jobnr.    #modes  #successors   successors
   1        1          3           2   3   4
   2        2          2           8  11
   3        2          3           5   6   8
   4        2          3           7  14  16
   5        2          3           9  13  14
   6        2          3           9  13  14
   7        2          3          10  11  17
   8        2          1          16
   9        2          3          10  11  16
  10        2          1          15
  11        2          1          12
  12        2          1          15
  13        2          1          17
  14        2          2          15  17
  15        2          1          18
  16        2          1          18
  17        2          1          18
  18        1          0        
************************************************************************
REQUESTS/DURATIONS:
jobnr. mode duration  R 1  R 2  N 1  N 2
------------------------------------------------------------------------
  1      1     0       0    0    0    0
  2      1     6       0    7    0    4
         2     6       0    8    8    0
  3      1     1       0    6    3    0
         2     6       4    0    0    2
  4      1     8       0    9    6    0
         2    10       0    8    4    0
  5      1     1       0    2   10    0
         2     6       7    0    7    0
  6      1     6       7    0    0    8
         2     8       6    0    7    0
  7      1     1       2    0    3    0
         2     1       0    7    0    9
  8      1     4       8    0    2    0
         2     7       7    0    2    0
  9      1     4       0   10    8    0
         2     6       0    5    6    0
 10      1     2       8    0    6    0
         2     2       5    0    0    2
 11      1     6       6    0    1    0
         2     9       0    5    1    0
 12      1     1       6    0    4    0
         2     3       0    5    4    0
 13      1     3       6    0    9    0
         2    10       6    0    5    0
 14      1     9       0    4    5    0
         2    10       7    0    0    4
 15      1     5       0    6    5    0
         2    10       7    0    0    5
 16      1     7       0    6    5    0
         2     7      10    0    4    0
 17      1     5       8    0    0    9
         2    10       7    0    0    7
 18      1     0       0    0    0    0
************************************************************************
RESOURCEAVAILABILITIES:
  R 1  R 2  N 1  N 2
   13   12   70   34
************************************************************************
