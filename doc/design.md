# LinPipe Design and Philosophy

## Target Audience

Our expected target audience are fellow NLP and digital humanities researchers
who want fast and reliable text processing (tagging, parsing, etc.), without the
need to study these tasks.

Our intented user has basic computer skills (run a script from a commandline or
query a webservice) but not necessarilly programming/scripting experience.

Our secondary goal is to provide multilingual solutions (all UD languages
whenever possible, in tens of languages).

## Goals

- multilingual state-of-the-art text processing pipeline;
- integrates previous successful tools: UDPipe, NameTag, MorphoDiTa, etc.;
- includes: tokenization, segmentation, POS tagging, lemmatization, dependency
  parsing, morphological dictionary, named entity recognition, semantic parsing,
  named entity linking, coreference resolution;
- provides our shared task contributions of the above mentioned tasks in one
  place.

## Typical Use Cases

- tagging and parsing for a NLP shared task;
- NLPer working on a higher level task who needs text preprocessing;
- NLP Msc. or PhD. student who needs quick POS tagging;
- digital humanities researcher who needs to find named entities in data.

## Design Choices:

Handling training and inference separately:

- inference:
  - linpipe binary implemented in C++ with Python API;
  - no public C++ API;
  - entry point is commadline/webservice/Python API;
  - using libtensorflow as backend distributed by us;
- training:
  - Python implementation only;
  - using Python TensorFlow.
