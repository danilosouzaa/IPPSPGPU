************************************************************************
file with basedata            : md87_.bas
initial value random generator: 150151851
************************************************************************
projects                      :  1
jobs (incl. supersource/sink ):  14
horizon                       :  113
RESOURCES
  - renewable                 :  2   R
  - nonrenewable              :  2   N
  - doubly constrained        :  0   D
************************************************************************
PROJECT INFORMATION:
pronr.  #jobs rel.date duedate tardcost  MPM-Time
    1     12      0       20        9       20
************************************************************************
PRECEDENCE RELATIONS:
jobnr.    #modes  #successors   successors
   1        1          3           2   3   4
   2        3          3           5   7   8
   3        3          2           7  10
   4        3          2           6   7
   5        3          3          11  12  13
   6        3          3           8   9  11
   7        3          2           9  11
   8        3          2          10  13
   9        3          2          12  13
  10        3          1          12
  11        3          1          14
  12        3          1          14
  13        3          1          14
  14        1          0        
************************************************************************
REQUESTS/DURATIONS:
jobnr. mode duration  R 1  R 2  N 1  N 2
------------------------------------------------------------------------
  1      1     0       0    0    0    0
  2      1     5       8    4    5    0
         2     8       7    4    5    0
         3     9       7    4    0    6
  3      1     4       7    6    7    0
         2     8       7    5    0    3
         3     9       7    4    0    3
  4      1     4       8    7   10    0
         2     6       7    6    0   10
         3    10       6    4    0    4
  5      1     7       9    4    0    8
         2     9       7    3    7    0
         3    10       4    3    4    0
  6      1     3       4    7    0    7
         2     9       3    6    0    4
         3    10       1    6    0    2
  7      1     5       7    6    0    4
         2     6       6    6    0    3
         3     7       5    6    0    1
  8      1     5       5    8    7    0
         2     6       3    6    7    0
         3     8       1    2    0    8
  9      1     6       7    2    0    9
         2     7       7    2    0    3
         3    10       6    2    6    0
 10      1     2       7    7    0    7
         2     7       5    7    1    0
         3    10       3    6    0    6
 11      1     1       7    4    0    9
         2     9       6    3    0    6
         3    10       6    3    6    0
 12      1     3       7    3    6    0
         2     4       6    3    2    0
         3    10       6    2    0    3
 13      1     4       6    9    5    0
         2     8       4    7    5    0
         3    10       3    4    5    0
 14      1     0       0    0    0    0
************************************************************************
RESOURCEAVAILABILITIES:
  R 1  R 2  N 1  N 2
   19   15   46   56
************************************************************************
