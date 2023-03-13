""" Generates histogram from file containing qids.
    
    COMPRESS: when True, saves memory by compressing qids to have size
    approx len(qids)/GRANULARITY. Matplotlib is inefficient when working with
    large data.
"""
import matplotlib.pyplot as plt
import numpy as np
n_bins = 50

COMPRESS = True
GRANULARITY = 3

data = []
with open("qids.txt", 'r') as f:
    for line in f:
        data.append(int(line[1:]))

print(data[:5])

if COMPRESS:
    print("Starting compression")
    compressed = []
    data.sort()
    for i in range(GRANULARITY, len(data), GRANULARITY):
        compressed.append(sum(data[i-GRANULARITY:i]) // GRANULARITY)
    data = compressed
    print("Compression finished.")

print("Generating histograms...")
fig, ax = plt.subplots()
ax.hist(data, bins=n_bins)
plt.savefig('hist.png')
