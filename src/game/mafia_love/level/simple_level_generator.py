#!/usr/bin/env python3
import math

DIMENSION = 16

with open("slvl.txt", "w") as f:
  for i in range(0, DIMENSION):
    if i == 0 or i == DIMENSION - 1:
      for j in range(0, DIMENSION):
        f.write("1, ")
    else:
      f.write("1, ")
      for j in range(0, DIMENSION - 2):
        f.write("0, ")
      f.write("1, ")