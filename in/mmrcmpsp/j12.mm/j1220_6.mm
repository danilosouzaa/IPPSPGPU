************************************************************************
file with basedata            : md84_.bas
initial value random generator: 1808020156
************************************************************************
projects                      :  1
jobs (incl. supersource/sink ):  14
horizon                       :  93
RESOURCES
  - renewable                 :  2   R
  - nonrenewable              :  2   N
  - doubly constrained        :  0   D
************************************************************************
PROJECT INFORMATION:
pronr.  #jobs rel.date duedate tardcost  MPM-Time
    1     12      0       17        6       17
************************************************************************
PRECEDENCE RELATIONS:
jobnr.    #modes  #successors   successors
   1        1          3           2   3   4
   2        3          2           5   7
   3        3          3           6   9  10
   4        3          3           5   6  10
   5        3          1           8
   6        3          1           7
   7        3          3           8  11  12
   8        3          1          13
   9        3          3          11  12  13
  10        3          3          11  12  13
  11        3          1          14
  12        3          1          14
  13        3          1          14
  14        1          0        
************************************************************************
REQUESTS/DURATIONS:
jobnr. mode duration  R 1  R 2  N 1  N 2
------------------------------------------------------------------------
  1      1     0       0    0    0    0
  2      1     3       8    0    7    0
         2     6       8    0    6    0
         3     7       7    0    0    8
  3      1     1       0    4    0    4
         2     7       0    4    0    3
         3     8       0    4    0    2
  4      1     1       6    0    0    3
         2     2       5    0    7    0
         3     9       3    0    6    0
  5      1     6       4    0    9    0
         2     7       0    6    7    0
         3     8       4    0    0    3
  6      1     4       6    0    9    0
         2     4       0    4    0    7
         3     7       6    0    7    0
  7      1     1       6    0    9    0
         2     3       0    5    7    0
         3    10       4    0    6    0
  8      1     5       5    0    6    0
         2     6       5    0    0    7
         3     7       0    2    0    2
  9      1     3       1    0    0    8
         2     8       0    7    0    8
         3     9       0    1    2    0
 10      1     5       0    6    0    2
         2     5       6    0    0    9
         3     8       6    0    8    0
 11      1     2       0    8    0    4
         2     3       5    0    0    4
         3     5       0    3    1    0
 12      1     7       0    6    8    0
         2     7       0    6    0    6
         3     8       0    6    0    5
 13      1     3       4    0    4    0
         2     3       0    4    4    0
         3     7       0    4    3    0
 14      1     0       0    0    0    0
************************************************************************
RESOURCEAVAILABILITIES:
  R 1  R 2  N 1  N 2
   21   24   55   45
************************************************************************
