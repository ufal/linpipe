# Specification for the Individual Software Project KBELik

The goal is to write a C++ implementation of a _Knowledge Base for Entity
Linking (KBELik)_ for the LinPipe project – a module capable of creating
an optimized binary dump of a knowledge base suitable for entity linking and
offering a set of methods for working with it.

- The knowledge base will be created from the DaMuEL dataset available
  at http://hdl.handle.net/11234/1-5047, which consists of language
  agnostic knowledge base (_DaMuEL Wikidata_) and language-specific
  data. The description of the dataset is available in the paper TODO.

- Consequently, the KBELik will also consist of the
  - language-agnostic part, and
  - language-specific parts.

  The language-agnostic part is essentially a mapping from entity identifier
  to a collection of structured information about the entity.

  The language-specific part is a mapping from entity identifier to several
  information about the entity in the language in question, including
  a collection of textual mentions of the given entity.

- The size of the KBELik dump is expected to be in the order of gigabytes.

The goal is to implement an efficient persistent mmapable map with the
following properties:
- the map dumps should be reasonably small, reasonably efficient,
  and loadable via mmap to avoid reading the whole dump during loading;
- the keys will be entity identifiers and arbitrary strings;
- the values will be structured information with a JSON-like structure.

The whole KBELik will consist of:
- language-agnostic KBELik, a single file containing a persistent map
  from entity identifiers to entity structured information;
- language-specific KBELik, a single file for every language containing:
  - a persistent map from entity identifiers to language-specific information
    about the entity,
  - a persistent map from textual entity mentions to a set of entity
    identifiers with such a mention.
