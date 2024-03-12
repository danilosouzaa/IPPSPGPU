************************************************************************
file with basedata            : cr542_.bas
initial value random generator: 1848783712
************************************************************************
projects                      :  1
jobs (incl. supersource/sink ):  18
horizon                       :  113
RESOURCES
  - renewable                 :  5   R
  - nonrenewable              :  2   N
  - doubly constrained        :  0   D
************************************************************************
PROJECT INFORMATION:
pronr.  #jobs rel.date duedate tardcost  MPM-Time
    1     16      0       14        5       14
************************************************************************
PRECEDENCE RELATIONS:
jobnr.    #modes  #successors   successors
   1        1          3           2   3   4
   2        3          1           5
   3        3          3           6   8  10
   4        3          3          10  13  17
   5        3          3           7  11  16
   6        3          3           7   9  11
   7        3          2          14  17
   8        3          3           9  11  13
   9        3          3          12  16  17
  10        3          2          15  16
  11        3          1          12
  12        3          1          14
  13        3          1          14
  14        3          1          15
  15        3          1          18
  16        3          1          18
  17        3          1          18
  18        1          0        
************************************************************************
REQUESTS/DURATIONS:
jobnr. mode duration  R 1  R 2  R 3  R 4  R 5  N 1  N 2
------------------------------------------------------------------------
  1      1     0       0    0    0    0    0    0    0
  2      1     2      10    0    7    7    0    6    9
         2     8       9    2    0    6    0    6    6
         3     8       9    2    3    7    0    4    7
  3      1     2       5    5    0    0    7    9    8
         2     5       5    5    0    0    6    6    7
         3     9       0    0    0   10    5    3    6
  4      1     3       7    0    3    0    6    3    7
         2     7       0    9    0    2    5    2    5
         3     7       6    0    3    0    4    2    5
  5      1     5       7    0    8    4    0    9    7
         2     6       7    0    8    2    0    7    7
         3    10       0    0    5    0    0    6    4
  6      1     4       0    9    0    0    7    6   10
         2     6      10    0    4    0    3    6   10
         3     9      10    0    3    4    0    6    9
  7      1     1       0    0    0    4    0    8    3
         2     2       0    2    2    0    0    7    2
         3     5       4    0    2    3    0    7    1
  8      1     1       4    0    9    5    4    8    2
         2     9       3    1    0    5    3    7    2
         3     9       3    0    7    5    0    7    1
  9      1     1       8    0    0    8    0    2    8
         2     9       6    0    4    8    0    1    6
         3    10       0    6    0    0    3    1    3
 10      1     4       0    2    0    0    0    8    2
         2     9       0    0    0    0    9    6    2
         3    10       0    0    4    4    0    5    2
 11      1     1       0    0    7    0    5    6    4
         2     1       7    0    0    0    0    5    4
         3     5       7    0    0    0    5    4    3
 12      1     3       7    9    9    0    4    8    6
         2     5       0    0    8    7    0    8    6
         3     7       0    0    6    4    4    7    6
 13      1     2       0   10    0    7    4    6    5
         2     7       0    9    0    0    4    6    4
         3     9       0    9    0    0    0    3    3
 14      1     1       6    6    1    0    0    7    7
         2     4       0    0    0    0    4    7    7
         3     6       5    0    0    0    0    6    7
 15      1     2       0    0    3    8    0    8    5
         2     2       5   10    2    5    4    7    4
         3     3       0    6    0    0    2    4    4
 16      1     3       0    5    0    0    0    7    9
         2     3       0    0    0    0    5    7   10
         3     4       4    6    9    0    0    7    4
 17      1     1       9    0    1    9    4    4    7
         2     2       0    6    0    8    3    2    4
         3     2       6    7    1    6    2    4    3
 18      1     0       0    0    0    0    0    0    0
************************************************************************
RESOURCEAVAILABILITIES:
  R 1  R 2  R 3  R 4  R 5  N 1  N 2
   19   16   15   13   13   90   84
************************************************************************
