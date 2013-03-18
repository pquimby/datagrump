#!/usr/bin/python

import sys
import os
import itertools
import subprocess

experiment_values = []

TOTAL_PARAMS = 4

# Generate the ranges for each set of 3 inputs
i = 1
while i < len(sys.argv):
    start = float(sys.argv[i])
    step = float(sys.argv[i+1])
    end = float(sys.argv[i+2])

    values = []
    j = start
    while j < end:
        values.append(j)
        j += step
    if abs(values[-1] - end) > .000000001:
        values.append(end)

    experiment_values.append(values)
    i += 3
# We now have a list of lists. Each sublist is the range of values for that parameter.

# Generate the combinations of values for that make up the experimental conditions
combos = list(itertools.product(*experiment_values))

for combo in combos:
    combo = list(combo)
    print "Running trial with", combo
    while len(combo) < TOTAL_PARAMS:
        combo.append(0)

    os.environ['ALPHA'] = str(combo[0])
    os.environ['BETA'] = str(combo[1])
    os.environ['GAMMA'] = str(combo[2])
    os.environ['DELTA'] = str(combo[3])
    
    subprocess.call("./run-trial.py pquimby", shell=True)

    
