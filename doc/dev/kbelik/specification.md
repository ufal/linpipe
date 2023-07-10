# Specification to Kbelik

Podrobná specifikace - detailní popis problematiky, podle něhož by jiný programátor napsal "tentýž" program:

- Návrh struktury programu (moduly, knihovny, příp. framework, vzájemná provázanost)

## Goal

The goal is to create a knowledge base for entity linking usable with any dataset following JSON structures described in the paper [[2306.09288] DaMuEL: A Large Multilingual Dataset for Entity Linking](https://arxiv.org/abs/2306.09288) by Kubeša and Straka.

In essence, the program should work as a map between entity ids and metadata. Maps should be available for both the agnostic and the specific datasets.

A broader description can be found in the Assignment.md file in this folder.

The Kbelik is going to be a part of the much larger Linpipe project.

## Main components

### Kbeliks

These are the parts that the user will interact with. They should allow the user to build knowledge bases if they are provided with data in the specified format and should be able to efficiently answer queries.

There might be two kbeliks for the two types of JSON structures (agnostic and specific) or one general kbelik if the structure of queries is similar enough.

The public interface of a kbelik should satisfy the following. It has:

- static method to build the kbelik to a stream
- methods for loading the kbelik from a file and for answering queries

### Persistent map

The core part of the logic is in the persistent map which handles the logic of answering queries without necessarily understanding the data. It directly accesses memory, implements efficient searching over data, and uses *map_values* to deserialize chunks of memory that are then given to kbeliks.

Ideally, the persistent map is general enough that it can be used in other applications different from Kbelik.

The persistent map uses mmap to efficiently work with the large amount of data serialized to it.

Due to the nature of mmap, persistent map cannot be modified. Any changes should be done using the *dynamic map*.

The public interface of a kbelik should satisfy the following. It has:

- methods for finding and mmaping itself from a file
- it can also tell the state of the mmap (mapped/not mapped) and close it

### Dynamic map

Is map providing similar functionality as the persistent map. However, elements can be added and removed from it. This additional functionality is paid for by a great increase in space complexity because all key-value pairs are held in RAM without any attempts to decrease their size.

### Agnostic entity info

Stores the agnostic data about one entity. The data stored should be the following:

- claims
- fictional
- named entities

### Specific entity info

Stores the specific data about one entity. The data stored should be the following:

- label
- aliases
- description
- wiki/text (or part of it)

## map values

Map values are boilerplates for serialization and deserialization of datatypes (variable length integers, vectors of bytes and bools, AgnosticEntityInfo, SpecificEntityInfo,...)

## Development specific information

The program is going to be written in C++. It should run both under UNIX and Windows.
