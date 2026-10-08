#ifndef HASHTABLE_H
#define HASHTABLE_H
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "list.h"

#define TABLE_SIZE 100

typedef struct HTEntry {
    char *key;
    void *value;
} HTEntry;

typedef struct HashTable {
    LinkedList **buckets;
    size_t size;
    size_t count;
    uint32_t (*hash_func)(const char*);
} HashTable;

HashTable* ht_create(uint32_t (*h_func)(const char*));

void ht_insert(HashTable *ht, const char *key, void *data);

void* ht_get(HashTable *ht, const char *key, int (*compare)(void*, void*));

void ht_destroy(HashTable *ht, void (*destroy_data)(void*));

uint32_t hash_djb2(const char *str);

void ht_save_txt(HashTable *ht, const char *filename, void (*write_value)(FILE*, void*));

void ht_load_txt(HashTable *ht, const char *filename, void* (*parse_value)(char*));

#endif
