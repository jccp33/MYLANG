#ifndef GRAPH_H
#define GRAPH_H
#include "list.h"
#include <stdio.h>

typedef struct Graph {
    int num_vertices;
    LinkedList **adj_lists;
    bool is_directed;
} Graph;

Graph* graph_create(int vertices, bool directed);

void graph_add_edge(Graph *graph, int src, int dest);

void graph_print(Graph *graph);

void graph_destroy(Graph *graph);

void graph_save_txt(Graph *graph, const char *filename, char* (*get_node_name)(int));

void graph_find_path(Graph *graph, int start_node, int end_node, char* (*get_name)(int));

void graph_load_txt(Graph *graph, const char *filename, int (*get_id_by_name)(char*));

#endif
