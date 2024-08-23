#!/usr/bin/env python3
import math

cutoff = 20

step = 2 ** -8
x = 0

with open("atan_table.txt", "w") as f:
  while (x < cutoff):
    angle = math.atan(x)
    fixed_angle = math.floor(2 ** 16 / (2 * math.pi) * angle)
    f.write(str(fixed_angle) + ", ")
    x += step