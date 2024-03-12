************************************************************************
file with basedata            : md255_.bas
initial value random generator: 2060758410
************************************************************************
projects                      :  1
jobs (incl. supersource/sink ):  18
horizon                       :  128
RESOURCES
  - renewable                 :  2   R
  - nonrenewable              :  2   N
  - doubly constrained        :  0   D
************************************************************************
PROJECT INFORMATION:
pronr.  #jobs rel.date duedate tardcost  MPM-Time
    1     16      0       16        5       16
************************************************************************
PRECEDENCE RELATIONS:
jobnr.    #modes  #successors   successors
   1        1          3           2   3   4
   2        3          2           5  14
   3        3          2          11  12
   4        3          2          14  16
   5        3          3           6   7   9
   6        3          2           8  10
   7        3          2          10  13
   8        3          2          11  13
   9        3          3          10  11  13
  10        3          1          15
  11        3          2          16  17
  12        3          2          14  15
  13        3          3          15  16  17
  14        3          1          17
  15        3          1          18
  16        3          1          18
  17        3          1          18
  18        1          0        
************************************************************************
REQUESTS/DURATIONS:
jobnr. mode duration  R 1  R 2  N 1  N 2
------------------------------------------------------------------------
  1      1     0       0    0    0    0
  2      1     1       5    9    7    8
         2     3       5    9    5    7
         3     6       4    8    4    6
  3      1     1       9    3    9    9
         2     5       7    2    6    8
         3     6       3    2    5    8
  4      1     2       9    7    1    6
         2     7       9    6    1    5
         3    10       8    6    1    4
  5      1     2       8    5    1    8
         2     8       7    5    1    7
         3    10       7    4    1    6
  6      1     5       6    8    6    3
         2     9       4    7    5    2
         3     9       3    8    5    3
  7      1     1       4    5    8    7
         2     2       2    5    7    6
         3     6       1    2    7    5
  8      1     4       6   10   10    7
         2     8       5    6    8    7
         3    10       4    2    4    5
  9      1     3       6   10    7    9
         2     4       6    5    5    9
         3    10       5    3    4    9
 10      1     1       6    7    7    9
         2     2       6    6    7    7
         3     6       5    6    7    5
 11      1     2       3    9    7    8
         2     6       3    6    4    7
         3     7       2    4    4    6
 12      1     5       4    5    6    8
         2     6       3    5    2    3
         3     6       2    5    5    6
 13      1     2       7    2    6    5
         2     5       7    2    3    4
         3     9       7    2    2    3
 14      1     6       3    9    7    6
         2     7       3    9    6    5
         3     9       3    8    6    4
 15      1     2       7    3    8   10
         2     4       6    3    5    9
         3     4       4    3    4   10
 16      1     1       2    3    8    9
         2     6       1    2    7    9
         3    10       1    2    6    8
 17      1     2       9    4    8    2
         2     9       9    4    8    1
         3    10       8    2    8    1
 18      1     0       0    0    0    0
************************************************************************
RESOURCEAVAILABILITIES:
  R 1  R 2  N 1  N 2
   19   23  106  114
************************************************************************
