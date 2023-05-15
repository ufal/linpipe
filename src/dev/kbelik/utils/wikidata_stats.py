""" Extracts qids from wikidata stored in WIKIDATA_DIR and save them in
OUTPUT_FILE. Specify number of available PROCESSES. """
from collections import defaultdict
import json
from math import ceil
from multiprocessing import Pool
import os

PROCESSES = 16
#PROCESSES = 2
WIKIDATA_DIR = '../../../../../damuel_1.0/damuel_1.0_wikidata'
# OUTPUT_FILE = 'qids.txt'

assert PROCESSES <= os.cpu_count()


def process_file(path):
    qids = set()
    counts = defaultdict(int)
    def process_line(line):
        try: 
            d = json.loads(line)
            qids.add(d['qid'])
            if "named_entities" in d:
                net = d["named_entities"]["type"]
                for entity in net:
                    counts[entity] += 1
        except json.JSONDecodeError:
            pass

    with open(path, 'r') as f:
        text = f.read()
    for line in text.split('\n'):
        process_line(line)
    return qids, counts


def process_files(l):
    qids = set()
    counts = defaultdict(int)
    for path in l:
        file_qids, file_counts = process_file(path)
        qids = qids.union(file_qids)
        for k, v in file_counts.items():
            counts[k] += v
        print("processed file", path)
    return qids, counts


def chunks(l):
    sliced = []
    n = ceil(len(l) / PROCESSES)
    for i in range(0, len(l), n):
        sliced.append(l[i:i + n])
    return sliced

paths = [WIKIDATA_DIR + '/' +fp for fp in os.listdir(WIKIDATA_DIR)]
paths = chunks(paths) 

print("Starting pools...")

qids = set()
counts = defaultdict(int)

with Pool(PROCESSES) as p:
    res = p.map(process_files, paths)
    for s, c in res:
        qids = qids.union(s)
        for k, v in c.items():
            counts[k] += v

total_counts = sum(counts.values())

print("RESULTS:")
print("There are total of ", len(qids), f"in the {WIKIDATA_DIR}")
print("Named entities type frequencies")
for k, v in counts.items():
    print(f"type {k}, number of occurences {v}, share {round(100 * v / total_counts, 2)} %")
