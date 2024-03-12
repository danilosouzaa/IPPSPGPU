************************************************************************
file with basedata            : c2128_.bas
initial value random generator: 862274162
************************************************************************
projects                      :  1
jobs (incl. supersource/sink ):  18
horizon                       :  139
RESOURCES
  - renewable                 :  2   R
  - nonrenewable              :  2   N
  - doubly constrained        :  0   D
************************************************************************
PROJECT INFORMATION:
pronr.  #jobs rel.date duedate tardcost  MPM-Time
    1     16      0       13       15       13
************************************************************************
PRECEDENCE RELATIONS:
jobnr.    #modes  #successors   successors
   1        1          3           2   3   4
   2        3          3           6   9  10
   3        3          3           9  12  13
   4        3          3           5   6   7
   5        3          3           8  10  13
   6        3          3          11  12  13
   7        3          3           8  10  12
   8        3          1           9
   9        3          3          11  14  15
  10        3          2          14  17
  11        3          2          16  17
  12        3          3          14  15  17
  13        3          2          15  16
  14        3          1          16
  15        3          1          18
  16        3          1          18
  17        3          1          18
  18        1          0        
************************************************************************
REQUESTS/DURATIONS:
jobnr. mode duration  R 1  R 2  N 1  N 2
------------------------------------------------------------------------
  1      1     0       0    0    0    0
  2      1     1       0    8    8    0
         2     6       0    7    0    6
         3     9       7    0    8    0
  3      1     5       7    0    6    0
         2     7       0    8    6    0
         3    10       0    5    0    1
  4      1     1       5    0    7    0
         2     4       0    3    0    3
         3     8       5    0    2    0
  5      1     1       0    9    9    0
         2     4       0    4    0    8
         3     8       8    0    6    0
  6      1     2       8    0    2    0
         2     5       0    8    0    7
         3     8       0    8    0    6
  7      1     3       0    7    7    0
         2     3       0    6    0    9
         3     9       0    6    0    4
  8      1     2       0    5    0    7
         2     8       0    4    6    0
         3    10       0    4    4    0
  9      1     3       9    0    6    0
         2    10       0    4    0    7
         3    10       0    5    0    5
 10      1     1       5    0    8    0
         2     2       0    6    8    0
         3     7       0    6    6    0
 11      1     1       0    5    9    0
         2     3       7    0    0    7
         3     9       0    3    4    0
 12      1     1       0    4    0    8
         2     1       4    0    5    0
         3    10       0    5    5    0
 13      1     4       0    1    5    0
         2     4       6    0    4    0
         3     6       0    1    2    0
 14      1     3       4    0    3    0
         2     8       0    5    3    0
         3     8       3    0    0    9
 15      1     1       8    0    7    0
         2     3       8    0    0    9
         3     8       8    0    0    4
 16      1     1       0    9    0    3
         2     4       0    3    0    2
         3    10       3    0    4    0
 17      1     1       0    4    0    9
         2     5       7    0    5    0
         3     9       0    4    0    7
 18      1     0       0    0    0    0
************************************************************************
RESOURCEAVAILABILITIES:
  R 1  R 2  N 1  N 2
   22   32   97   93
************************************************************************
