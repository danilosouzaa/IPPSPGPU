************************************************************************
file with basedata            : cn150_.bas
initial value random generator: 95513669
************************************************************************
projects                      :  1
jobs (incl. supersource/sink ):  18
horizon                       :  134
RESOURCES
  - renewable                 :  2   R
  - nonrenewable              :  1   N
  - doubly constrained        :  0   D
************************************************************************
PROJECT INFORMATION:
pronr.  #jobs rel.date duedate tardcost  MPM-Time
    1     16      0       19       10       19
************************************************************************
PRECEDENCE RELATIONS:
jobnr.    #modes  #successors   successors
   1        1          3           2   3   4
   2        3          3           9  10  11
   3        3          3           5   7  11
   4        3          3           6  12  17
   5        3          2          14  17
   6        3          2           8   9
   7        3          3           9  15  17
   8        3          2          10  13
   9        3          1          13
  10        3          2          14  15
  11        3          2          12  13
  12        3          2          15  16
  13        3          1          14
  14        3          1          16
  15        3          1          18
  16        3          1          18
  17        3          1          18
  18        1          0        
************************************************************************
REQUESTS/DURATIONS:
jobnr. mode duration  R 1  R 2  N 1
------------------------------------------------------------------------
  1      1     0       0    0    0
  2      1     1       0    6    5
         2     9       7    0    5
         3     9       0    5    5
  3      1     1       0    5    7
         2     2       2    0    5
         3     5       0    4    5
  4      1     1       6    0    7
         2     2       5    0    5
         3    10       0    2    2
  5      1     2       5    0    8
         2     4       0    9    7
         3     6       0    9    5
  6      1     3       8    0    6
         2     4       0    7    4
         3     9       0    5    2
  7      1     4       3    0    8
         2     4       0    8    6
         3     9       4    0    6
  8      1     3       0    7    9
         2     4       0    5    7
         3     9       9    0    5
  9      1     3       8    0    5
         2     8       7    0    4
         3     8       5    0    5
 10      1     4      10    0    5
         2     9       0    8    3
         3     9       4    0    3
 11      1     4       0    8    6
         2     5       6    0    5
         3     9       0    7    4
 12      1     5       2    0    7
         2     6       2    0    4
         3     9       0    2    4
 13      1     6       5    0    8
         2     7       0   10    8
         3    10       0   10    7
 14      1     3       5    0    5
         2     5       5    0    4
         3     7       0    1    3
 15      1     1       6    0    4
         2     2       0    9    3
         3     8       0    6    3
 16      1     2       0    3   10
         2     6       1    0   10
         3     8       0    2    9
 17      1     7       8    0    5
         2     8       0    8    4
         3     9       0    8    1
 18      1     0       0    0    0
************************************************************************
RESOURCEAVAILABILITIES:
  R 1  R 2  N 1
   19   14   96
************************************************************************
