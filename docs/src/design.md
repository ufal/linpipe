# LinPipe Design and Philosophy

## Target Audience

Our expected target audience are:
- fellow NLP and digital humanities researchers who want fast and reliable text
  processing (tagging, parsing, etc.), without the need to study these tasks;
- industry, especially when needing efficient on-site text processing.

## Goals

- multilingual state-of-the-art text processing pipeline;
- easy to use and deploy services for text processing, allowing using
  both on-site and server-side processing;
- natural evolution of popular shared-task-winning tools: UDPipe, NameTag,
  MorphoDiTa, etc.;
- seamless integration of various subtasks of NLP processing, including
  tokenization, segmentation, POS tagging, lemmatization, dependency parsing,
  semantic parsing, named entity recognition, named entity linking, privacy
  filtering, coreference resolution.

## Design Choices

Handling training and inference separately:

- inference:
  - linpipe binary implemented in C++ with Python API in `linpipe` package;
  - no public C++ API;
  - entry point is commadline/webservice/Python API;
  - provides on-site and server-side processing;
  - using onnx as backend distributed by us;
- training:
  - Python implementation in `linpipe.training` package;

LinPipe native format `lif`, based on JSON, is used by all LinPipe components to
exchange data, and import/export is provided for popular formats such as
CoNLL-U, tei, PML, and others.
