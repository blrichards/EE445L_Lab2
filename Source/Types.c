#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>

#include "Types.h"

String *newString(int num, va_list args) {
		String *str = (String*)malloc(sizeof(String));

		const char *src = va_arg(args, const char *);
		int len = (int)strlen(src);

		str->cap = len ? len : 1;
		str->len = len;
		str->data = (num == 0) ? (char*)malloc(1) : (char*)malloc((size_t)str->cap + 1);

		for (int i = 0; i < len; i++)
				str->data[i] = src[i];
		str->data[len] = '\0';
	
		return str;
}

entry *newEntry(int num, va_list args) {
		assert(num == 3);
		entry *e = (entry*)malloc(sizeof(entry));

		HashMap *map = va_arg(args, HashMap*);
		void *key = va_arg(args, void*);
		switch (map->keyType) {
		case STRING:
				e->key = new(STRING, 1, ((String*)key)->data);
				break;
		case NUMBER:
		default:
				e->key = (void*)(*(uint32_t*)key);
				break;
		}
				
		void *value = va_arg(args, void*);
		switch (map->valueType) {
		case STRING:
				e->value = new(STRING, 1, ((String*)value)->data);
				break;
		case NUMBER:
		default:
				e->value = (void*)(*(uint32_t*)value);
				break;
		}
		e->next = NULL;
		e->map = map;
		return e;
}

HashMap *newHashMap(int num, va_list args) {
		assert(num == 0);
		HashMap *m = (HashMap*)malloc(sizeof(HashMap));

		m->entrySet = (entry**)malloc(sizeof(entry*) * HASH_SIZE);
		m->keySet = (void**)malloc(sizeof(String*) * HASH_SIZE);
		for (int i = 0; i < HASH_SIZE; i++) {
				m->keySet[i] = NULL;
				m->entrySet[i] = NULL;
		}
		
		return m;
}

void *new(TYPE t, int num, ...) {
    void *new_t;
    va_list args;
    va_start(args, num);

    switch (t) {
		case (STRING):
				new_t = newString(num, args);
				break;
		case (ENTRY):
				new_t = newEntry(num, args);
				break;
		case (HASHMAP):
				new_t = newHashMap(num, args);
				break;
		default:
				new_t = NULL;
				break;
    }

    va_end(args);
    return new_t;
}

void delete(TYPE t, void *d) {
    switch(t) {
		case (STRING):
				free(((String*)d)->data);
				break;
		case (ENTRY):
				delete(((entry*)d)->map->keyType, ((entry*)d)->key);
				delete(((entry*)d)->map->valueType, ((entry*)d)->value);
				break;
		case (HASHMAP):
				clear((HashMap*)d);
				free(((HashMap*)d)->entrySet);
				free(((HashMap*)d)->keySet);
				break;
		default:
				return;
    }
		free(d);
}
