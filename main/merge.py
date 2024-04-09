import os
import re

dir = './output/block/'
file_names = os.listdir(dir)
file_names = sorted(file_names, key=lambda x: int(x))

# for file_name in file_names:
#     print(file_name)

merged_file = dir + 'out.txt'

with open(merged_file, 'w') as outfile:
    for file_name in file_names:
        with open(dir + file_name, 'r') as infile:
            outfile.write(infile.read())
        os.remove(dir + file_name)

# Hybrid SVSim

dir = './output/hybrid/'
file_names = os.listdir(dir)

def sort_by_number(file_name):
    number = re.findall(r'\d+', file_name)
    if number:
        return int(number[0])
    else:
        return 0

file_names = sorted(file_names, key=sort_by_number)

for file_name in file_names:
    print(file_name)

merged_file = dir + 'out.txt'

with open(merged_file, 'w') as outfile:
    for file_name in file_names:
        with open(dir + file_name, 'r') as infile:
            outfile.write(infile.read())
        os.remove(dir + file_name)