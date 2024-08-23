#!/usr/bin/env python3
import math

palette = [
  0x472d3c, 0x5e3643, 0x7a444a, 0xa05b53, 0xbf7958, 0xeea160, 0xf4cca1, 0xb6d53c,
  0x71aa34, 0x397b44, 0x3c5956, 0x302c2e, 0x5a5353, 0x7d7071, 0xa0938e, 0xcfc6b8,
  0xdff6f5, 0x8aebf1, 0x28ccdf, 0x3978a8, 0x394778, 0x39314b, 0x564064, 0x8e487c,
  0xcd6093, 0xffaeb6, 0xf4b41b, 0xf47e1b, 0xe6482e, 0xa93b3b, 0x827094, 0x4f536b
]

with open("palette.txt", "w") as f:
  for c in palette:
    # Original 24 bit colors [8:8:8]
    r = c >> 16
    g = (c & 0x00FF00) >> 8
    b = c & 0x0000FF
    # Transform colors
    r = math.floor(r * .5)
    g = math.floor(g * .5)
    b = math.floor(b * .5)
    # Convert to 16 bit colors [5:6:5]
    r = r >> 3
    g = g >> 2
    b = b >> 3
    # Write to file
    nc = (r << 11) | (g << 5) | b
    f.write(hex(nc) + ", ")
  