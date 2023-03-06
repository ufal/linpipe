# Specification for the Individual Software Project KB4EL

The goal is to write a C++ implementation of a _Knowledge Base for Entity
Linking (KBEL)_ for the LinPipe project – an implementation capable of creating
an optimized binary dump of a knowledge base suitable for entity linking and
offering a set of methods for working with it.

- The knowledge base will be created from the DaMuEL dataset available
  at http://hdl.handle.net/11234/1-5047, which consists of language
  agnostic knowledge base (_DaMuEL Wikidata_) and language-specific
  data. The description of the dataset is available in the paper TODO.

- Consequently, the KBEL will also consist of the
  - language-agnostic part, and
  - language-specific parts.

  The language-agnostic part is essentially a mapping from entity identifier
  to a collection of information about the entity.

  The language-specific part is a mapping from entity identifier to several
  information about the entity in the language in question.

- The size of the KBEL dump is expected to be in the order of gigabytes.
