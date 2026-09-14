# System Architecture

## Overview

The system provides two main use cases:

- **Pipeline**: load a document, apply a configurable sequence of NLP
  operations, and write the resulting document.
- **Train model**: train a new model from available training data.

```mermaid
flowchart LR
    U((User))
    P[Pipeline]
    T[Train Model]

    U --> P
    U --> T
```

An overview of the LinPipe system architecture:

- **Data:** All data is held as a single `Corpus`, which contains a list of
  `Documents`, which contain a list of abstract `Layers`, such as `Text`,
  `SegmentedText`, `Tokens`, `TaggedTokens`, or `TaggedSpans`.
- **Pipeline**: The inference transformations execution is based on
  a `Pipeline`, a user-configured sequence of abstract `Operations`, such as
  `Segment` or `Tokenize`.
- **I/O**: `Load` and `Save` are also parts of the `Pipeline` as `Operations`,
  ones that contain an abstract class `Format`, such as `Text`, `Conll`, or
  `Lif`.
- **Train model**: Python binding for the C++ code with I/O and batching
  implemented in C++, shared with the `Pipeline` use case, and exposed to Python
  via Python binding `linpipe.training`. The training itself implemented in
  Python scripts with `import linpipe.training`. Trained checkpoints saved in
  `onnx`. `ModelManager` loads trained checkpoints via `onnx`.
- **Model Management**: `Model Manager` orchestrates loading models from disk,
  access to models and rotating the models in memory.

## Corpus, Document and Layers

A single `Corpus` serves as a container for a list of multiple `Documents`.
A `Document` is a collection of `Layers`. `Layers` are pure data structures. The
`Document` guarantees that layer names are unique. A `Document` may contain
several `Layers` of the same type, but the `Layer` names must be unique.

```mermaid
classDiagram
  class Corpus {
    +documents: vector~unique_ptr~Document~~
  }

  class Document {
    +get_layer~T=Layer~(name: string_view) T&
    +add_layer(layer: unique_ptr~Layer~&&, unique_name_if_duplicate: bool) Layer&
    +del_layer(name: string_view)
    +layers() const vector~unique_ptr~Layer~~&
    +source_path() const string&
    +set_source_path(source_path: string_view)
  }

  class Layer {
    <<abstract>>
    +create(type: string, name: string) unique_ptr~Layer~
    +from_json(json: Json)
    +to_json() Json
    +to_html() string
    +name() string&
    +type() string&
  }

  Corpus "1" *-- "0..*" Document
  Document "1" *-- "0..*" Layer

```

Typical `Layer` types include:

```mermaid
classDiagram
    class Layer {
        <<abstract>>
        +create(type: string, name: string) unique_ptr~Layer~
        +from_json(json: Json)
        +to_json() Json
        +to_html() string
        +name() string&
        +type() string&
    }

    class Text
    class SegmentedText
    class Tokens
    class TaggedTokens
    class TaggedSpans

    Layer <|-- Text
    Layer <|-- SegmentedText
    Layer <|-- Tokens
    Layer <|-- TaggedTokens
    Layer <|-- TaggedSpans
```

## Pipeline and Operations

All pipelines in LinPipe are realized via a user-configurable sequence of
transformations over data. A `Pipeline` consists of a configurable sequence of
`Operations`.

The operations are executed sequentially. Each operation receives the Corpus
produced by the preceding operation and enriches it with additional Layers. The
operations also pass a `PipelineState` object which captures the `Pipeline`
instance information, in particular the pointer to `Model Manager` for access to
available models and input and output stream.

```mermaid
classDiagram
  class Pipeline {
    +execute(Corpus)
  }

  class PipelineState {
    +model_manager: ModelManager&
    +default_input: istream&
    +default_output: ostream&
  }

  class Operation {
    <<interface>>
    +reserve_models(PipelineState)
    +execute(Corpus) Document
  }

  Pipeline "1" *-- "1..*" Operation
  Pipeline o-- PipelineState

  class Segment
  class Tokenize
  class MorphologicalAnalysis
  class DependencyParse
  class NER

  Operation <|.. Segment
  Operation <|.. Tokenize
  Operation <|.. MorphologicalAnalysis
  Operation <|.. DependencyParse
  Operation <|.. NER
```

For example:

```mermaid
flowchart LR
    D0["Corpus<br/>Document<br/>Text"]
    S["Segment"]
    D1["Corpus<br/>Document<br/>+ SegmentedText"]
    T["Tokenize"]
    D2["Corpus<br/>Document<br/>+ Tokens"]
    M["MorphologicalAnalysis"]
    D3["Corpus<br/>Document<br/>+ TaggedTokens"]
    N["NER"]
    D4["Corpus<br/>Document<br/>+ TaggedSpans"]

    D0 --> S --> D1
    D1 --> T --> D2
    D2 --> M --> D3
    D3 --> N --> D4
```

The sequence of Operations is not fixed. Different Pipelines may compose
different Operations in different orders, provided that their Layer requirements
are satisfied.

## Formats

`Load` and `Save` are also `Operations` and part of the `Pipeline`. Each
instance of `Load` and `Save` contains a `Format`, such as `Text`, `Conll`, or
`Lif`.

```mermaid
classDiagram
  class Load {
  }

  Load o-- Format

  class Save {
  }

  Save o-- Format

  Operation <|-- Load
  Operation <|-- Save

  class Format {
    <<abstract>>
    +create(description: string) unique_ptr~Format~
    +load(input: istream&, source_path: string) unique_ptr~Document~
    +save(Document, output: ostream&)
    +save_corpus_start(output: ostream&)
    +save_corpus_end(output: ostream&)
  }

  class Text
  class Conll
  class Lif

  Format <|-- Text
  Format <|-- Conll
  Format <|-- Lif
```

## Model Management

```mermaid
classDiagram
  class ModelManager {
    +reserve(name: string)
    +load(name: string) unique_ptr~Model~&
    +release(name: string)
    +singleton: ModelManager$
  }
```

## Design Suggestions for the Next Meeting

### Pipeline Validation

A `Pipeline` should validate the compatibility of its `Operations` before
execution. For that, each `Operation` should implement `require()` (a set of
`Layer` types) and `produce()` (a set of `Layer` types).

```mermaid
flowchart LR
    T[Task] --> V[Validate operation sequence]

    V -->|valid| R[Run operations]
    V -->|invalid| E[Configuration error]

    R --> O1[Operation 1]
    O1 --> O2[Operation 2]
    O2 --> O3[Operation 3]
```

An operation may additionally verify its required Layers at runtime and raise
an exception if the Document does not contain them.

For example, `NER` may declare:

```
requires: Tokens
produces: TaggedSpans
```

while `Tokenize` may declare:

```
requires: SegmentedText
produces: Tokens
```

### Execute vs. Apply

Maybe we should rename `execute()` to `apply()`.

### Corpus

Do we need a single `Corpus` holder for multiple `Documents`?

### Server in PipelineState

Source code has `Server server` in `PipelineState`, why are we passing a server
along with a Pipeline?

### Train Model

Class design.

## TODO

1. Describe pipeline construction from string description.
2. Describe I/O.

---

![Architecture Overview](overview_1.png){ width=100% }
