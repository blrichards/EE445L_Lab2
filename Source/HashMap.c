#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include "types.h"

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
uint32_t hashStr(String *s) {
    uint32_t h = START;

    for (int i = 0; i < s->len; i++)
        h = (h * A) ^ (s->data[i] * B);

    return (h * C) % HASH_SIZE; // return number modded by the largest prime
}

uint32_t hashNumber(uint32_t number)
{
		return (number * 2654435761) % HASH_SIZE;
}

uint32_t hashKey(TYPE type, void *key)
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

/*
 * @func: value
 * @brief: find hash value in HashMap associated with a key
 * @input: HashMap pointer & key
 * @output: true if key was found, false otherwise
 * @summary: find open hash value or matching key by using a loop and the
 *           nextHash function. return the final hash value
 */
entry* find(HashMap *dict, void *key) {
    uint32_t h = hashKey(dict->keyType, key);

    entry *current = dict->entrySet[h];

    while (current != NULL) {
        if (equals(current->key, key))
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
void put(HashMap *dict, void *key, void *value) {
    int h = hashKey(dict->keyType, key);

    if (dict->entrySet[h] == NULL) {
        dict->entrySet[h] = (entry*)new(ENTRY, 2, key, value);
        return;
    }

    entry *current = dict->entrySet[h];

    while (current->next != NULL)
        current = current->next;

    current->next = (entry*)new(ENTRY, 2, key, value);
}

/* @func: contains
 * @brief: check if word is in dictionary
 * @input: HashMap pointer & key
 * @output: true if word is in dict, false otherwise
 */
bool contains(HashMap *dict, void *key) {
    entry *anEntry = find(dict, key);
    return anEntry != NULL;
}

void *at(HashMap *dict, void *key) {
    entry *e = find(dict, key);
    assert(e != NULL);
    return (e)->value;
}

void clear(HashMap *dict) {
    entry *current;
    entry *next;

    for (uint32_t i = 0; i < HASH_SIZE; i++) {
        current = dict->entrySet[i];

        while (current != NULL) {
            next = current->next;
            delete(ENTRY, current);
            current = next;
        }
    }
    dict->size = 0;
}

entry *iterate(HashMap *map) {
		static HashMap *m = NULL;
		static size_t i = 0;
		static entry *current = NULL;
	
		if (map != NULL) {
				i = 0;
				m = map;
		}
		
		if (current != NULL && current->next != NULL) {
				current = current->next;
				return current;
		}
		
		for (; i < HASH_SIZE; ++i) {
				current = m->entrySet[i];
				return current;
		}
		
		return NULL;
}

void *getKey(entry *e) {
		switch (e->map->keyType) {
		case NUMBER:
				return &(e->key);
		default:
				return e->key;
		}
}

void *getValue(entry *e) {
		switch (e->map->valueType) {
		case NUMBER:
				return &(e->value);
		default:
				return e->value;
		}
}