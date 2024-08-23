#!/usr/bin/env python3
import math

with open("tex_step_table.txt", "w") as f:
  f.write("0, ") # Cannot divide by 0
  f.write("0, ") # A height of 1 won't step
  for i in range(2, 128):
    step = 64.0 / i
    integer = math.floor(step)
    fraction = step - integer
    fraction = math.floor((2 ** 8) * fraction)
    res = (integer << 8) + fraction
    f.write(str(res) + ", ")