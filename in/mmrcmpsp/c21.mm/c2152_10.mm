************************************************************************
file with basedata            : c2152_.bas
initial value random generator: 1296899357
************************************************************************
projects                      :  1
jobs (incl. supersource/sink ):  18
horizon                       :  134
RESOURCES
  - renewable                 :  2   R
  - nonrenewable              :  2   N
  - doubly constrained        :  0   D
************************************************************************
PROJECT INFORMATION:
pronr.  #jobs rel.date duedate tardcost  MPM-Time
    1     16      0       25        8       25
************************************************************************
PRECEDENCE RELATIONS:
jobnr.    #modes  #successors   successors
   1        1          3           2   3   4
   2        3          3           5   6  11
   3        3          3           5   6   7
   4        3          2           7   9
   5        3          3           8   9  10
   6        3          3           8  10  13
   7        3          3           8  10  11
   8        3          2          12  14
   9        3          3          12  13  17
  10        3          2          12  14
  11        3          2          16  17
  12        3          1          15
  13        3          2          15  16
  14        3          3          15  16  17
  15        3          1          18
  16        3          1          18
  17        3          1          18
  18        1          0        
************************************************************************
REQUESTS/DURATIONS:
jobnr. mode duration  R 1  R 2  N 1  N 2
------------------------------------------------------------------------
  1      1     0       0    0    0    0
  2      1     8       8    0    5   10
         2     8       9    0    5    8
         3    10       0    6    4    7
  3      1     1       0    6    8    5
         2    10       0    4    8    4
         3    10       2    0    8    3
  4      1     2       7    0   10    2
         2     7       0    6    9    2
         3     8       0    5    9    2
  5      1     1       7    0   10    6
         2     9       7    0    6    6
         3     9       6    0    8    6
  6      1     3       1    0    7    9
         2     8       0    3    6    7
         3     9       1    0    3    6
  7      1     1       0    5   10    9
         2     5       0    5    8    9
         3     8       0    4    6    9
  8      1     4       9    0    8    6
         2     6       0    4    7    4
         3     8       0    4    7    2
  9      1     9       0    5    8    6
         2     9       6    0    8    6
         3    10       0    4    8    1
 10      1     2       0    8    6    8
         2     5       5    0    5    7
         3     9       0    4    5    3
 11      1     7       0    7    8    8
         2     9       0    7    6    5
         3    10       5    0    3    4
 12      1     2       3    0    5    5
         2     4       0    6    4    5
         3     6       0    3    2    5
 13      1     2       8    0    9    8
         2     5       0    9    9    8
         3     9       0    9    9    7
 14      1     2       0    3    3    5
         2     2       9    0    3    6
         3     9       0    3    1    3
 15      1     5       0    7    4    8
         2     8       3    0    2    4
         3     8       3    0    3    2
 16      1     2       0    5    7    7
         2     3       0    4    5    6
         3     7       5    0    1    4
 17      1     1       6    0    7    8
         2     3       0   10    6    5
         3     4       0    9    2    5
 18      1     0       0    0    0    0
************************************************************************
RESOURCEAVAILABILITIES:
  R 1  R 2  N 1  N 2
   25   21  105  101
************************************************************************
