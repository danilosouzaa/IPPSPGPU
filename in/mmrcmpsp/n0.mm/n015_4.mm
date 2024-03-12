************************************************************************
file with basedata            : me15_.bas
initial value random generator: 1885817786
************************************************************************
projects                      :  1
jobs (incl. supersource/sink ):  14
horizon                       :  100
RESOURCES
  - renewable                 :  2   R
  - nonrenewable              :  0   N
  - doubly constrained        :  0   D
************************************************************************
PROJECT INFORMATION:
pronr.  #jobs rel.date duedate tardcost  MPM-Time
    1     12      0       17        9       17
************************************************************************
PRECEDENCE RELATIONS:
jobnr.    #modes  #successors   successors
   1        1          3           2   3   4
   2        3          3           5   7   8
   3        3          3           6   7   8
   4        3          3           5  10  13
   5        3          1           6
   6        3          1           9
   7        3          3           9  10  13
   8        3          2          12  13
   9        3          2          11  12
  10        3          2          11  12
  11        3          1          14
  12        3          1          14
  13        3          1          14
  14        1          0        
************************************************************************
REQUESTS/DURATIONS:
jobnr. mode duration  R 1  R 2
------------------------------------------------------------------------
  1      1     0       0    0
  2      1     1       7    9
         2     4       7    8
         3     7       7    3
  3      1     3       4    8
         2     8       4    6
         3    10       2    6
  4      1     5       8    7
         2     6       6    5
         3     9       3    3
  5      1     1       5    3
         2     4       4    3
         3    10       4    2
  6      1     1       7    8
         2     6       4    6
         3     8       3    6
  7      1     2       6   10
         2     4       3    8
         3     8       1    7
  8      1     2       3    9
         2     5       3    8
         3     7       3    6
  9      1     5       6    9
         2     7       5    9
         3    10       4    9
 10      1     2       9   10
         2     3       9    9
         3     4       8    8
 11      1     2       5    9
         2     8       2    9
         3     9       1    8
 12      1     5       6    5
         2     8       5    3
         3     8       4    4
 13      1     1       6    3
         2     4       4    3
         3    10       1    3
 14      1     0       0    0
************************************************************************
RESOURCEAVAILABILITIES:
  R 1  R 2
   17   22
************************************************************************
