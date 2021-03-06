#include <stdbool.h>

#ifndef TYPES_H
#define TYPES_H

#define HASH_SIZE 1000

typedef enum {
    STRING,
    NUMBER,
    ENTRY,
    HASHMAP
} TYPE;

typedef struct {
    char* data;
    int len;
    int cap;
} String;

struct HashMap;

/* @struct: entry
 * @brief: node for linked list to store collided entries
 */
typedef struct entry {
    struct entry* next;
    void* key;
    void* value;
    struct HashMap* map;
} entry;

/* @struct: Dict
 * @brief: Essentially a set that uses a hash table to quickly check
 *         if words exist in the "dictionary"
 */
typedef struct HashMap {
    entry* entrySet[HASH_SIZE];
    void* keySet[HASH_SIZE];
    int size;
    TYPE keyType;
    TYPE valueType;
} HashMap;

void* new (TYPE, int, ...);

void delete (TYPE, void*);

/**
 * String functions
 */
void concat(String*, const String*);

void addChar(String*, const char);

void toLower(String*);

void toUpper(String*);

bool equals(String*, String*);

void capitalize(String*);

/**
 * HashMap functions
 */
void* put(HashMap* map, void* key, void* value);

bool contains(HashMap* dict, void* key);

void* at(HashMap* dict, void* key);

void* getOrDefault(HashMap* dict, void* key, void* defaultValue);

void clear(HashMap* dict);

entry* iterate(HashMap* dict);

void* getKey(entry*);

void* getValue(entry*);

#endif // TYPES_H
