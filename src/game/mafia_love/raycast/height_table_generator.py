#!/usr/bin/env python3
import math

with open("height_table.txt", "w") as f:
  f.write("127, ") # Cannot divide by 0
  for i in range(1, 2 ** 16):
    # Simple linear height table (for now)
    height = int(128 / (i * (2 ** -8)))
    if height > 127:
      height = 127
    if height == 0:
      print("Heights are 0 after and when input is " + str(i))
      break
    f.write(str(height) + ", ")