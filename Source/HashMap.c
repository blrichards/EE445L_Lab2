#include "types.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define A 54059
#define B 76963
#define C 86969
#define START 37

/* @func: hashStr
 * @brief: used to hash Strings for "HashMap" struct
 * @input: String
 * @output: 32 bit unsigned hash value
 * @summary: generic hash function used for hashing Strings
 */
uint32_t hashStr(String* s)
{
    uint32_t h = START;

    for (int i = 0; i < s->len; i++)
        h = (h * A) ^ (s->data[i] * B);

    return (h * C) % HASH_SIZE; // return number modded by the largest prime
}

uint32_t hashNumber(uint32_t number)
{
    return (number * 2654435761) % HASH_SIZE;
}

uint32_t hashKey(TYPE type, void* key)
{
    switch (type) {
    case NUMBER:
        return hashNumber(*(uint32_t*)key);
    case STRING:
        return hashStr((String*)key);
    default:
        return hashNumber((uint32_t)key);
    }
}

static bool typed_equals(TYPE type, void* a, void* b)
{
    switch (type) {
    case STRING:
        return equals(b, b);
    case NUMBER:
        return *(uint32_t*)a == *(uint32_t*)b;
    default:
        return a == b;
    }
}

/*
 * @func: value
 * @brief: find hash value in HashMap associated with a key
 * @input: HashMap pointer & key
 * @output: true if key was found, false otherwise
 * @summary: find open hash value or matching key by using a loop and the
 *           nextHash function. return the final hash value
 */
entry* find(HashMap* dict, void* key)
{
    uint32_t h = hashKey(dict->keyType, key);

    entry* current = dict->entrySet[h];

    while (current != NULL) {
        if (typed_equals(dict->keyType, &current->key, key))
            return current;
        current = current->next;
    }

    return current;
}

/* @func: put
 * @brief: sets the key value pair in the specified HashMap instance
 * @input: HashMap pointer, key, & desired key value
 * @summary: checks if the key exists in the dictionary, add it if it doesn't,
 *           and assigns the desired value in the _values array
 */
void* put(HashMap* dict, void* key, void* value)
{
    int h = hashKey(dict->keyType, key);

    if (dict->entrySet[h] == NULL) {
        dict->entrySet[h] = (entry*)new (ENTRY, 3, dict, key, value);
        return getValue(dict->entrySet[h]);
    }

    entry* current = dict->entrySet[h];

    while (current->next != NULL)
        current = current->next;

    current->next = (entry*)new (ENTRY, 3, dict, key, value);
    return getValue(current->next);
}

/* @func: contains
 * @brief: check if word is in dictionary
 * @input: HashMap pointer & key
 * @output: true if word is in dict, false otherwise
 */
bool contains(HashMap* dict, void* key)
{
    entry* anEntry = find(dict, key);
    return anEntry != NULL;
}

void* at(HashMap* dict, void* key)
{
    entry* e = find(dict, key);
    assert(e != NULL);
    return e->value;
}

void* getOrDefault(HashMap* dict, void* key, void* defaultValue)
{
    entry* e = find(dict, key);
    if (e == NULL)
        return put(dict, key, defaultValue);
    return getValue(e);
}

void clear(HashMap* dict)
{
    entry* current;
    entry* next;

    for (uint32_t i = 0; i < HASH_SIZE; i++) {
        current = dict->entrySet[i];

        while (current != NULL) {
            next = current->next;
            delete (ENTRY, current);
            current = next;
        }

        dict->entrySet[i] = NULL;
    }
    dict->size = 0;
}

static HashMap* _m = NULL;
static size_t _i = 0;
static entry* _current = NULL;

entry* iterate(HashMap* map)
{
    if (map != NULL) {
        _i = 0;
        _m = map;
    }

    if (_current != NULL && _current->next != NULL) {
        _current = _current->next;
        return _current;
    }

    for (; _i < HASH_SIZE; ++_i) {
        _current = _m->entrySet[_i];
        if (_current != NULL) {
            ++_i;
            return _current;
        }
    }

    return NULL;
}

void* getKey(entry* e)
{
    switch (e->map->keyType) {
    case NUMBER:
        return &(e->key);
    default:
        return e->key;
    }
}

void* getValue(entry* e)
{
    switch (e->map->valueType) {
    case NUMBER:
        return &e->value;
    default:
        return e->value;
    }
}
