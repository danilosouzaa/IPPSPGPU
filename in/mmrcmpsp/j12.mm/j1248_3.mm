************************************************************************
file with basedata            : md112_.bas
initial value random generator: 1342374914
************************************************************************
projects                      :  1
jobs (incl. supersource/sink ):  14
horizon                       :  103
RESOURCES
  - renewable                 :  2   R
  - nonrenewable              :  2   N
  - doubly constrained        :  0   D
************************************************************************
PROJECT INFORMATION:
pronr.  #jobs rel.date duedate tardcost  MPM-Time
    1     12      0       20        4       20
************************************************************************
PRECEDENCE RELATIONS:
jobnr.    #modes  #successors   successors
   1        1          3           2   3   4
   2        3          3           5   7   9
   3        3          3           5   6   7
   4        3          3          11  12  13
   5        3          1          10
   6        3          3           8  10  11
   7        3          2           8  11
   8        3          2          12  13
   9        3          2          10  13
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
  2      1     4       6    8    8    5
         2     9       5    6    5    5
         3    10       2    3    3    2
  3      1     1       4    7    9    8
         2     8       3    7    9    7
         3     9       2    7    9    7
  4      1     1       8   10    8    4
         2     5       6    7    8    3
         3     8       5    7    7    3
  5      1     1       8    8    6   10
         2     7       7    8    6    8
         3     9       5    6    2    7
  6      1     5       8    7    3    5
         2     6       7    6    2    4
         3     9       4    5    1    4
  7      1     8       7    4    8    9
         2     9       6    4    5    8
         3    10       6    2    4    7
  8      1     6       5    7    9    5
         2     8       3    7    8    5
         3    10       2    4    7    4
  9      1     1       8    8   10    8
         2     1       8    8    9    9
         3     2       7    2    8    7
 10      1     7       7    6   10    3
         2     9       7    3    9    3
         3    10       4    1    9    3
 11      1     2       5    5    4   10
         2     8       3    4    3    8
         3     9       2    4    3    5
 12      1     2       8    6   10    3
         2     2       8    7    8    3
         3     7       8    6    3    1
 13      1     2       8    9    8    9
         2    10       6    7    4    6
         3    10       6    8    5    3
 14      1     0       0    0    0    0
************************************************************************
RESOURCEAVAILABILITIES:
  R 1  R 2  N 1  N 2
   31   27   77   67
************************************************************************
