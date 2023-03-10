""" Extracts qids from wikidata stored in WIKIDATA_DIR and save them in
OUTPUT_FILE. Specify number of available PROCESSES. """
from multiprocessing import Pool
import json
import os
from math import ceil

PROCESSES = 16
#PROCESSES = 2
WIKIDATA_DIR = 'wikidata'
OUTPUT_FILE = 'qids.txt'

assert PROCESSES <= os.cpu_count()


def process_file(path):
    qids = set()
    def process_line(line):
        try: 
            d = json.loads(line)
            qids.add(d['qid'])
        except json.JSONDecodeError:
            pass

    with open(path, 'r') as f:
        text = f.read()
    for line in text.split('\n'):
        process_line(line)
    return qids


def process_files(l):
    qids = set()
    for path in l:
        qids = qids.union(process_file(path))
        print("processed file", path)
    return qids


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

with Pool(PROCESSES) as p:
    res = p.map(process_files, paths)
    for s in res:
        qids = qids.union(s)

with open(OUTPUT_FILE, 'w') as f:
    for qid in qids:
        f.write(qid + '\n')
