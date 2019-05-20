#!/usr/bin/env python3

"""
Generate a data file that is conform with the PINK data format
Image data is taken from the sklearn digits set
"""
from sklearn import datasets

# Load data from the digits data set
# Each sample describes an 8x8 image
print("Loading the digits data set as demo data...")
digits = datasets.load_digits()
data = digits.data[:1000]
print("Set loaded. Data format:", data.shape)

# Data layout
dim = 2
width = 8
height = 8

# Create a file and save the data in PINK-compatible format
file_name = "digits.pink"
print("Saving the data to a file named", file_name)
print("Warning: If such a file already exists, it will be overwritten.")
file = open(file_name, 'w')
file.write("{file_version} 0 {data_type} {no_entries} 0".format(file_version=2, data_type=0, no_entries=data.shape[0]))
file.close()

file = open(file_name, 'a')
file.write(" {}".format(dim))
file.write(" {}".format(width))
file.write(" {}".format(height))

for d in data.flatten():
    file.write(" {}".format(d))

file.close()

print("Done.")