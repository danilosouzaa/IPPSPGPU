************************************************************************
file with basedata            : me31_.bas
initial value random generator: 868291391
************************************************************************
projects                      :  1
jobs (incl. supersource/sink ):  18
horizon                       :  131
RESOURCES
  - renewable                 :  2   R
  - nonrenewable              :  0   N
  - doubly constrained        :  0   D
************************************************************************
PROJECT INFORMATION:
pronr.  #jobs rel.date duedate tardcost  MPM-Time
    1     16      0       20        4       20
************************************************************************
PRECEDENCE RELATIONS:
jobnr.    #modes  #successors   successors
   1        1          3           2   3   4
   2        3          3           7   9  12
   3        3          3           7  13  16
   4        3          3           5   8  12
   5        3          3           6   7  13
   6        3          2          11  14
   7        3          2          14  15
   8        3          1           9
   9        3          2          10  15
  10        3          2          11  17
  11        3          1          16
  12        3          2          16  17
  13        3          2          14  15
  14        3          1          17
  15        3          1          18
  16        3          1          18
  17        3          1          18
  18        1          0        
************************************************************************
REQUESTS/DURATIONS:
jobnr. mode duration  R 1  R 2
------------------------------------------------------------------------
  1      1     0       0    0
  2      1     2       5    6
         2     4       4    6
         3     8       2    5
  3      1     3       6    3
         2     3       8    2
         3    10       4    2
  4      1     3       7    2
         2     7       5    2
         3     9       4    1
  5      1     4       7    9
         2     5       6    9
         3     8       5    8
  6      1     3       6    4
         2     3       8    3
         3     4       4    3
  7      1     3       7    6
         2     4       5    6
         3     7       2    5
  8      1     1       6    7
         2     7       5    7
         3    10       3    6
  9      1     1       5    9
         2     2       5    8
         3     8       4    8
 10      1     6      10    2
         2     7       8    1
         3     8       5    1
 11      1     5       7    1
         2     6       5    1
         3     9       4    1
 12      1     3       5    5
         2     4       4    3
         3     5       4    1
 13      1     8       9    2
         2     8       7    3
         3     8       3    4
 14      1     3       8   10
         2     7       6    6
         3     7       5    7
 15      1     1       7    7
         2     3       5    5
         3    10       5    4
 16      1     3       7    8
         2     9       6    7
         3    10       6    6
 17      1     2       8   10
         2     6       8    5
         3    10       5    4
 18      1     0       0    0
************************************************************************
RESOURCEAVAILABILITIES:
  R 1  R 2
   27   19
************************************************************************
