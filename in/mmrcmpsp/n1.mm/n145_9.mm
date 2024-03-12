************************************************************************
file with basedata            : cn145_.bas
initial value random generator: 129561897
************************************************************************
projects                      :  1
jobs (incl. supersource/sink ):  18
horizon                       :  138
RESOURCES
  - renewable                 :  2   R
  - nonrenewable              :  1   N
  - doubly constrained        :  0   D
************************************************************************
PROJECT INFORMATION:
pronr.  #jobs rel.date duedate tardcost  MPM-Time
    1     16      0       22       14       22
************************************************************************
PRECEDENCE RELATIONS:
jobnr.    #modes  #successors   successors
   1        1          3           2   3   4
   2        3          1           8
   3        3          3           6   7  12
   4        3          2           5   6
   5        3          2          11  13
   6        3          3          13  14  15
   7        3          2           8   9
   8        3          3          11  15  17
   9        3          3          10  11  14
  10        3          3          13  15  17
  11        3          1          16
  12        3          2          14  16
  13        3          1          16
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
  2      1     6       8    7    8
         2     8       6    6    7
         3    10       6    4    6
  3      1     3       7    7   10
         2     5       6    7    9
         3     6       4    6    9
  4      1     4       5    4    4
         2     7       3    4    3
         3     9       2    3    3
  5      1     1       6    7   10
         2     5       5    6    9
         3     9       5    5    8
  6      1     1       3    7    6
         2     3       2    6    5
         3     6       1    6    4
  7      1     3       7    6    5
         2     6       6    6    5
         3     9       5    6    5
  8      1     5       5    7   10
         2     8       4    6    9
         3    10       3    5    8
  9      1     5       7    7    9
         2    10       7    4    3
         3    10       6    5    7
 10      1     3      10    8    9
         2     7      10    7    9
         3    10      10    6    9
 11      1     1       5    8    4
         2     5       4    8    3
         3     9       4    8    2
 12      1     6       7    5    8
         2     7       6    4    6
         3     9       1    3    5
 13      1     7       7   10    6
         2     8       7    7    4
         3     8       7    5    6
 14      1     1       3    9    7
         2     4       3    9    6
         3     8       3    8    6
 15      1     2       7    4    9
         2     4       7    3    7
         3     8       4    3    6
 16      1     1       9    2    7
         2     4       8    2    7
         3     7       7    1    6
 17      1     1       2    8    7
         2     2       2    7    6
         3    10       2    5    4
 18      1     0       0    0    0
************************************************************************
RESOURCEAVAILABILITIES:
  R 1  R 2  N 1
   15   14  104
************************************************************************
