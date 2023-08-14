#!/usr/bin/python3
import sys

file_name = sys.argv[1]
file = open(file_name, "r")

tot = 0
zeros = 0
ones = 0
for line in file:
    num = int(line)
    tot += num
    if num == 0:
        zeros += 1
    elif num == 1:
        ones += 1

print("tot:", tot)
print("zeros:", zeros)
print("ones:", ones)
