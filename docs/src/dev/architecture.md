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

- **Data:** All data is held as a single `Corpus`, which contains a list of
  `Documents`, which contain a list of abstract `Layers`, such as `Text`,
  `SegmentedText`, `Tokens`, `TaggedTokens`, or `TaggedSpans`.
- **Pipeline**: The inference transformations execution is based on
  a `Pipeline`, a user-configured sequence of abstract `Operations`, such as
  `Segment`, `Tokenize`, or `Tag`.
- **Train model**: TODO.
- **I/O**: Input and output are realized via abstract `Formats`, such as LinPipe
  native `LiF`, `Text`, or `CoNLL`.

## Corpus, Document and Layers

A single `Corpus` serves as a container for a list of multiple `Documents`.
A `Document` is a collection of `Layers`. `Layers` are pure data structures. The
`Document` guarantees that layer names are unique. A `Document` may contain
several `Layers` of the same type, but the `Layer` names must be unique.

```mermaid
classDiagram
  class Corpus {
    +vector~unique_ptr~Document
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

    class PlainText
    class SegmentedText
    class Tokens
    class TaggedTokens
    class TaggedSpans

    Layer <|-- PlainText
    Layer <|-- SegmentedText
    Layer <|-- Tokens
    Layer <|-- TaggedTokens
    Layer <|-- TaggedSpans
```

## Pipeline and Operations

All pipelines in LinPipe are realized via a user-configurable sequence of
transformations over data. A `Pipeline` consists of a configurable sequence of
`Operations`.

```mermaid
classDiagram
    class Pipeline {
        +execute(Corpus)
    }

    class Operation {
        <<interface>>
        +execute(Corpus) Document
    }

    Pipeline "1" *-- "1..*" Operation

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

The operations are executed sequentially. Each operation receives the Corpus
produced by the preceding operation and enriches it with additional Layers.

For example:

```mermaid
flowchart LR
    D0["Corpus<br/>Document<br/>PlainText"]
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

## Input and Output Formats

TODO

## Design Suggestions for the Next Meeting

### Pipeline Validation

A `Pipeline` should validate the compatibility of its `Operations` before
execution. For that, each `Operation` should implement `require()` (a set of
`Layer` types) and `produce()` (a set of `Layer` types).

```
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

## Execute vs. Apply

Maybe we should rename `execute()` to `apply().

## Corpus

Do we need a single `Corpus` holder for multiple `Documents`?

---

![Architecture Overview](overview_1.png){ width=100% }
