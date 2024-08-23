#!/usr/bin/env python3
import math

def sec(x):
  return 1.0 / math.cos(x)

trig_fn = math.tan

with open("tan_table.txt", "w") as f:
  for i in range(0, 2 ** 14 + 1):
    # calculate real tangent
    trig_in = 2 * math.pi / (2 ** 16) * i
    y_real = trig_fn(trig_in)
    # extract integer and fraction
    integer = math.floor(y_real)
    fraction = y_real - integer
    # fix integer bounds
    if integer > 255:
      integer = 255
    # fixed fraction magic
    fixed_fraction = math.floor(256 * fraction)
    # create the final result
    res = (integer << 8) + fixed_fraction
    #print(f"{res:#0{6}x}")
    if (res >= 2 ** 16):
      print("uh oh")
    f.write(str(res) + ", ")