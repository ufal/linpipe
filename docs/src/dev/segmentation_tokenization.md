# Segmentation and Tokenization

We never want to copy the strings (text) too much to save memory to be able to
scale to large corporas. For this reason, as many as possible strings in the
`Pipeline`, such as the tokens, are anchored to the original underlying plain
text with character spans.

Therefore, `PlainText` is mandatory: for corpora with real source text this is
the actual document text; for corpora loaded pre-tokenized (e.g. `CoNLL`, where
no original text ever existed) the `Load` operation must synthesize one and mark
it `PlainText::synthetic()`.

``` mermaid
classDiagram
  class PlainText {
    +text() string_view
    +synthetic() bool
  }
```

Either way, `Segmentation` and `TokenLayer` itself never store a copy of token
characters; they always read them out of the referenced `PlainText`, which keeps
per-token memory to a single `IndexSpan` (two integers) rather than a duplicated
string.

```mermaid
classDiagram
  class IndexSpan {
    +begin: size_t
    +end: size_t
    +size() size_t
    +empty() bool
    +contains(index: size_t) bool
  }
```

Segmentation (typically  sentence) can exist and be used without tokenization,
i.e, when loading line-separated sentences of raw text. Tokenization, on the
other hand, very rarely exists without prior segmentation. So we make an
assumption "no tokenization without segmentation" to simplify the analysis.

Hence, `Segmentation` is a sequence of non-overlapping `IndexSpans` marking
segment boundaries, expressed as char spans into a mandatory `PlainText`. It is
deliberately independent of tokenization.

```mermaid
classDiagram

    class Segmentation {
        +size() size_t
        +plain_text_layer_name() string_view
        +sentences() SentenceRange
        +whole_document(name: string, plain_text: PlainText&) Segmentation
      }

    class SentenceView {
        +index() size_t
        +span() IndexSpan&
        +text() string_view
      }

    class SentenceRange {
        +size() size_t
        +empty() bool
        +operator[](i: size_t) SentenceView
        +begin() SentenceIterator
        +end() SentenceIterator
    }

    Segmentation "1" o-- "1" PlainText : mandatory, slices text from
    Segmentation ..> SentenceRange : sentences()
    SentenceRange ..> SentenceView : yields
    SentenceView ..> PlainText : text()
```

`TokenLayer` is also always backed up with mandatory `PlainText` on character
level. However, just keeping `IndexSpans` into `PlainText` is not sufficient, as
we have to deal with cases that plain `IndexSpan` can't express:

1. A token's text isn't the literal substring at its own span (e.g., Unicode
   normalization).
2. A run of tokens is the syntactic decomposition of one literal surface unit,
   as with CoNLL-U multiword tokens (e.g. "zum" -> "zu" + "dem", neither of
   which occurs verbatim in the source, nor has its own independent span).

Therefore, `TokenLayer` internally stores three data structures for expressing
tokens:

1. `vector<IndexSpans> spans_`: the actual spans into `PlainText`,
2. `unordered_map<size_t, string> overrides_`: for overrides over the original
   `PlainText` surface tokens,
3. `vector<MultiwordGroup> multiword_groups_`: multiword groups.

Then for token at position `i`, `token_text(i)` returns an explicit
override if one was previously saved for `i` into `overrides_` with
`set_text_override()`, otherwise the literal substring of the referenced
`PlainText`.

Accessing the original surface text with multiwords, for example as a `CoNLL-U`
writer: The client walks the token stream (`begin()->end()`) alongside the
`multiword groups_` (accessed by `multiword_groups()`) previously saved by
`add_multiword_group()` in order  to know where to emit a surface line before
its constituent word lines.

Finally, `TokenLayer` must always be bound to prior `Segmentation`.

```mermaid
classDiagram
  class IndexSpan {
    +begin: size_t
    +end: size_t
    +size() size_t
    +empty() bool
    +contains(index: size_t) bool
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

  class PlainText {
    +text() string_view
    +synthetic() bool
  }

  class Segmentation {
    +size() size_t
    +plain_text_layer_name() string_view
    +rebind_plain_text(plain_text: PlainText&)
    +sentences() SentenceRange
    +whole_document(name: string, plain_text: PlainText&) Segmentation
  }

  class SentenceView {
    +index() size_t
    +span() IndexSpan&
    +text() string_view
  }

  class SentenceRange {
    +size() size_t
    +empty() bool
    +operator[](i: size_t) SentenceView
    +begin() SentenceIterator
    +end() SentenceIterator
  }

  class TokenLayer {
    +size() size_t
    +plain_text_layer_name() string_view
    +segmentation_layer_name() string_view
    +rebind(plain_text: PlainText&, segmentation: Segmentation&)
    +token_span(i: size_t) IndexSpan&
    +token_text(i: size_t) string_view
    +set_text_override(i: size_t, text: string)
    +add_multiword_group(token_range: IndexSpan, surface_span: IndexSpan, override: optional~string~)
    +group_containing(i: size_t) MultiwordGroup
    +multiword_groups() vector~MultiwordGroup~&
    +surface_text(group: MultiwordGroup&) string_view
    +token_range_for_char_span(char_range: IndexSpan) IndexSpan
    +begin() TokenIterator
    +end() TokenIterator
    +sentences() TokenizedSentenceRange
  }

  class MultiwordGroup {
    +token_range: IndexSpan
    +surface_span: IndexSpan
    +surface_text_override: optional~string~
  }

  class TokenRange {
    +size() size_t
    +empty() bool
    +begin() TokenIterator
    +end() TokenIterator
  }

  class TokenizedSentenceView {
    +index() size_t
    +span() IndexSpan&
    +text() string_view
    +tokens() TokenRange
  }

  class TokenizedSentenceRange {
    +size() size_t
    +empty() bool
    +operator[](i: size_t) TokenizedSentenceView
    +begin() TokenizedSentenceIterator
    +end() TokenizedSentenceIterator
  }

  Layer <|-- PlainText
  Layer <|-- Segmentation
  Layer <|-- TokenLayer

  Segmentation "1" o-- "1" PlainText : mandatory, slices text from
  Segmentation ..> SentenceRange : sentences()
  SentenceRange ..> SentenceView : yields
  SentenceView ..> PlainText : text()

  TokenLayer "1" o-- "1" PlainText : mandatory, slices text from
  TokenLayer "1" o-- "1" Segmentation : mandatory, no tokenization without sentences
  TokenLayer "1" *-- "0..*" MultiwordGroup
  TokenLayer ..> TokenizedSentenceRange : sentences()
  TokenizedSentenceRange ..> TokenizedSentenceView : yields
  TokenizedSentenceView ..> TokenRange : tokens()
  TokenRange ..> TokenLayer : views into
```
