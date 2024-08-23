#!/usr/bin/env python3
import math

'''
Specialized sin table for [15:0] fraction fixed point
'''

trig_fn = math.sin

with open("sin_table.txt", "w") as f:
  for i in range(0, 2 ** 14 + 1):
    # calculate real tangent
    trig_in = 2 * math.pi / (2 ** 16) * i
    y_real = trig_fn(trig_in)
    fixed_fraction = math.floor((2 ** 16) * y_real)
    if (fixed_fraction >= 2 ** 16):
      fixed_fraction = 2 ** 16 - 1
    f.write(str(fixed_fraction) + ", ")