************************************************************************
file with basedata            : cr112_.bas
initial value random generator: 1921283648
************************************************************************
projects                      :  1
jobs (incl. supersource/sink ):  18
horizon                       :  124
RESOURCES
  - renewable                 :  1   R
  - nonrenewable              :  2   N
  - doubly constrained        :  0   D
************************************************************************
PROJECT INFORMATION:
pronr.  #jobs rel.date duedate tardcost  MPM-Time
    1     16      0       24       14       24
************************************************************************
PRECEDENCE RELATIONS:
jobnr.    #modes  #successors   successors
   1        1          3           2   3   4
   2        3          2           5   7
   3        3          2          10  14
   4        3          3           6   9  12
   5        3          3           6   9  12
   6        3          2          10  14
   7        3          2           8  11
   8        3          2          12  13
   9        3          3          10  13  16
  10        3          1          17
  11        3          2          13  16
  12        3          2          15  16
  13        3          1          14
  14        3          2          15  17
  15        3          1          18
  16        3          1          18
  17        3          1          18
  18        1          0        
************************************************************************
REQUESTS/DURATIONS:
jobnr. mode duration  R 1  N 1  N 2
------------------------------------------------------------------------
  1      1     0       0    0    0
  2      1     7       6    4    0
         2     8       4    0    8
         3     9       3    0    6
  3      1     2       0    0    8
         2     6       0    0    1
         3     7       5    9    0
  4      1     3       9    5    0
         2     8       8    0    4
         3    10       0    0    3
  5      1     2       5    0    7
         2    10       1    0    6
         3    10       3   10    0
  6      1     1       5    0    6
         2     3       2   10    0
         3     9       0   10    0
  7      1     3       0    0   10
         2     4       0    5    0
         3     9       0    0    7
  8      1     1       0    0    4
         2     1       0    8    0
         3     2       0    5    0
  9      1     4       7    6    0
         2     6       4    0    5
         3     9       3    5    0
 10      1     1       9    0    7
         2     3       0    0    5
         3     4       0    4    0
 11      1     1       0    0    6
         2     4       0    3    0
         3     9       0    0    4
 12      1     5       2    0    6
         2     5       9    0    5
         3     8       0    6    0
 13      1     3       3    7    0
         2     4       0    4    0
         3    10       0    0    8
 14      1     5       3    0    9
         2     5       4    4    0
         3     9       0    0    9
 15      1     2       6    0    7
         2     5       0    6    0
         3     8       0    3    0
 16      1     1       0    0    7
         2     3       7    0    6
         3     4       0    6    0
 17      1     3       6    7    0
         2     3       6    0    6
         3     7       0    0    6
 18      1     0       0    0    0
************************************************************************
RESOURCEAVAILABILITIES:
  R 1  N 1  N 2
   21   50   54
************************************************************************
