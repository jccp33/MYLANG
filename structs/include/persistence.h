#ifndef PERSISTENCE_H
#define PERSISTENCE_H
#include "list.h"
#include "hashtable.h"
#include "graph.h"

typedef enum {
    TYPE_LIST,      // 0
    TYPE_STACK,     // 1
    TYPE_QUEUE,     // 2
    TYPE_GRAPH,     // 3
    TYPE_HASHTABLE, // 4
    TYPE_TREE,      // 5
    TYPE_HEAP       // 6
} StructureType;

typedef struct {
    void *data;
    StructureType type;
} LoadedStructure;

typedef void* (*GenericParser)(char*);

LoadedStructure load_from_file(const char *filename, StructureType type, GenericParser parser);

#endif
