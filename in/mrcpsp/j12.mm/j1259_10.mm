************************************************************************
file with basedata            : md123_.bas
initial value random generator: 2010101189
************************************************************************
projects                      :  1
jobs (incl. supersource/sink ):  14
horizon                       :  89
RESOURCES
  - renewable                 :  2   R
  - nonrenewable              :  2   N
  - doubly constrained        :  0   D
************************************************************************
PROJECT INFORMATION:
pronr.  #jobs rel.date duedate tardcost  MPM-Time
    1     12      0       20        6       20
************************************************************************
PRECEDENCE RELATIONS:
jobnr.    #modes  #successors   successors
   1        1          3           2   3   4
   2        3          2           5   7
   3        3          3           9  11  12
   4        3          3           8   9  10
   5        3          3           6   9  12
   6        3          3           8  10  13
   7        3          3          10  12  13
   8        3          1          11
   9        3          1          13
  10        3          1          11
  11        3          1          14
  12        3          1          14
  13        3          1          14
  14        1          0        
************************************************************************
REQUESTS/DURATIONS:
jobnr. mode duration  R 1  R 2  N 1  N 2
------------------------------------------------------------------------
  1      1     0       0    0    0    0
  2      1     1       4    0    8    6
         2     5       0    8    6    5
         3     6       3    0    3    5
  3      1     1       5    0    6    4
         2     3       5    0    5    3
         3     5       5    0    2    3
  4      1     3       0    7    5    9
         2     5       0    6    3    6
         3    10       0    2    1    4
  5      1     2       8    0    3    6
         2     7       0    9    1    5
         3     7       0    3    2    6
  6      1     4       0    2    9    4
         2     4       0    3    8    4
         3     8       9    0    5    4
  7      1     7       0   10    4    4
         2     7       6    0    4    3
         3     9       0    9    3    2
  8      1     5       7    0    8    8
         2     7       5    0    8    5
         3     9       3    0    6    5
  9      1     2       0    6    9    7
         2     3       9    0    6    4
         3     6       6    0    4    3
 10      1     5      10    0   10    7
         2     6       0    1    7    6
         3     7       0    1    3    6
 11      1     7       7    0    9    7
         2     8       0    9    4    5
         3     8       0   10    1    1
 12      1     1      10    0   10    6
         2     4       9    0    8    6
         3     5       9    0    6    5
 13      1     6       8    0    7    4
         2     9       0    4    6    3
         3     9       8    0    6    4
 14      1     0       0    0    0    0
************************************************************************
RESOURCEAVAILABILITIES:
  R 1  R 2  N 1  N 2
   21   15   88   72
************************************************************************
