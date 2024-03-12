************************************************************************
file with basedata            : cr353_.bas
initial value random generator: 1537623741
************************************************************************
projects                      :  1
jobs (incl. supersource/sink ):  18
horizon                       :  130
RESOURCES
  - renewable                 :  3   R
  - nonrenewable              :  2   N
  - doubly constrained        :  0   D
************************************************************************
PROJECT INFORMATION:
pronr.  #jobs rel.date duedate tardcost  MPM-Time
    1     16      0       23        3       23
************************************************************************
PRECEDENCE RELATIONS:
jobnr.    #modes  #successors   successors
   1        1          3           2   3   4
   2        3          3           5  13  15
   3        3          1           7
   4        3          3           5   6  15
   5        3          3           9  10  11
   6        3          2          11  12
   7        3          3           8   9  11
   8        3          3          13  14  15
   9        3          1          17
  10        3          2          14  16
  11        3          1          16
  12        3          2          13  14
  13        3          2          16  17
  14        3          1          17
  15        3          1          18
  16        3          1          18
  17        3          1          18
  18        1          0        
************************************************************************
REQUESTS/DURATIONS:
jobnr. mode duration  R 1  R 2  R 3  N 1  N 2
------------------------------------------------------------------------
  1      1     0       0    0    0    0    0
  2      1     4       7    7    3    2   10
         2     6       7    5    2    2    9
         3     8       4    4    1    1    9
  3      1     2       3    9    1    6    4
         2     2       3    9    1    9    3
         3    10       3    7    1    1    1
  4      1     8       2    7    7    8    5
         2     9       2    2    7    7    5
         3     9       2    4    4    7    5
  5      1     2       8    4    7    4    5
         2     4       6    4    6    3    5
         3     9       4    4    3    3    4
  6      1     3       6    8    8    8    8
         2     7       4    5    6    8    7
         3     8       1    3    6    8    6
  7      1     2       5    9    6    5    5
         2     4       4    7    3    4    5
         3    10       4    4    2    3    3
  8      1     2       9    7    6    5    9
         2     2       8    8    5    5    8
         3    10       2    6    5    4    8
  9      1     2       9    4    9    9    3
         2     5       7    3    8    7    3
         3     8       6    2    5    4    3
 10      1     5       7    6    9    4    7
         2     7       7    3    9    4    6
         3     9       7    2    9    2    3
 11      1     3       8    7    4    8   10
         2     3       7    8    3    4   10
         3     3       7    8    4    3    6
 12      1     2       9   10    7    9    7
         2     4       8    9    7    9    6
         3     8       6    7    6    7    5
 13      1     3       2    9    6    8    6
         2     3       2    8    6    7    7
         3     8       2    4    4    7    1
 14      1     5       6    6    6    5    9
         2     5       6    6    4    7   10
         3     6       6    4    4    3    2
 15      1     2       6    4   10    6    4
         2     9       3    2   10    6    1
         3     9       5    1    9    5    2
 16      1     4       3    7    6    6    9
         2     4       4    7    5    6    8
         3     5       1    6    5    5    6
 17      1     3       7    7    3    9    4
         2     7       7    7    2    8    3
         3    10       6    6    2    8    2
 18      1     0       0    0    0    0    0
************************************************************************
RESOURCEAVAILABILITIES:
  R 1  R 2  R 3  N 1  N 2
   14   12   14   98   97
************************************************************************
