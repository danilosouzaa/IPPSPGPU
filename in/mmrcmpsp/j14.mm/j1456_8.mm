************************************************************************
file with basedata            : md184_.bas
initial value random generator: 2131993634
************************************************************************
projects                      :  1
jobs (incl. supersource/sink ):  16
horizon                       :  116
RESOURCES
  - renewable                 :  2   R
  - nonrenewable              :  2   N
  - doubly constrained        :  0   D
************************************************************************
PROJECT INFORMATION:
pronr.  #jobs rel.date duedate tardcost  MPM-Time
    1     14      0       27       10       27
************************************************************************
PRECEDENCE RELATIONS:
jobnr.    #modes  #successors   successors
   1        1          3           2   3   4
   2        3          3           5   7  10
   3        3          3           5   7  10
   4        3          3           6  10  12
   5        3          2           8  12
   6        3          1          11
   7        3          3           9  12  15
   8        3          3          13  14  15
   9        3          2          11  14
  10        3          1          13
  11        3          1          13
  12        3          1          14
  13        3          1          16
  14        3          1          16
  15        3          1          16
  16        1          0        
************************************************************************
REQUESTS/DURATIONS:
jobnr. mode duration  R 1  R 2  N 1  N 2
------------------------------------------------------------------------
  1      1     0       0    0    0    0
  2      1     8      10    7    8    5
         2     9       9    4    5    5
         3     9      10    5    6    4
  3      1     6       8    7    8    7
         2     7       6    7    7    6
         3    10       4    6    6    4
  4      1     2       5    7    3    6
         2     4       4    7    3    5
         3    10       4    4    2    3
  5      1     8      10    9    7    6
         2     9      10    6    6    3
         3     9      10    5    7    4
  6      1     4       8    8    8    4
         2     7       7    8    6    3
         3     9       7    8    5    2
  7      1     6       8    7    5    7
         2     6       7    9    6    7
         3     8       6    4    5    7
  8      1     1       6    7    7   10
         2     5       3    6    4    8
         3     8       2    5    2    6
  9      1     4      10    7    7    6
         2     6      10    6    4    4
         3     8       9    5    3    1
 10      1     3       6    2    8    6
         2     3       7    2    7    7
         3     5       4    2    7    6
 11      1     4       7    6   10   10
         2     8       6    6   10    9
         3     9       6    5    9    9
 12      1     1       6    8    4    9
         2     1       7    9    4    8
         3     7       4    4    4    6
 13      1     5       9    9    6    7
         2     8       6    6    5    5
         3    10       5    6    3    5
 14      1     4       8    5    5    7
         2     5       4    5    5    4
         3     5       6    5    5    2
 15      1     8       9    8    9    6
         2     9       9    7    2    6
         3     9       8    8    6    6
 16      1     0       0    0    0    0
************************************************************************
RESOURCEAVAILABILITIES:
  R 1  R 2  N 1  N 2
   26   23   88   89
************************************************************************
