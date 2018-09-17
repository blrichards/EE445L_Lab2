#include "Types.h"

#include "heap.h"
#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

String* newString(int num, va_list args)
{
    String* str = (String*)Heap_Malloc(sizeof(String));

    const char* src = va_arg(args, const char*);
    int len = (int)strlen(src);

    str->cap = len ? len : 1;
    str->len = len;
    str->data = (num == 0) ? (char*)Heap_Malloc(1) : (char*)Heap_Malloc((size_t)str->cap + 1);

    for (int i = 0; i < len; i++)
        str->data[i] = src[i];
    str->data[len] = '\0';

    return str;
}

entry* newEntry(int num, va_list args)
{
    assert(num == 3);
    entry* e = (entry*)Heap_Malloc(sizeof(entry));

    HashMap* map = va_arg(args, HashMap*);
    void* key = va_arg(args, void*);
    switch (map->keyType) {
    case STRING:
        e->key = new (STRING, 1, ((String*)key)->data);
        break;
    case NUMBER:
    default:
        e->key = (void*)(*(uint32_t*)key);
        break;
    }

    void* value = va_arg(args, void*);
    switch (map->valueType) {
    case STRING:
        e->value = new (STRING, 1, ((String*)value)->data);
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

HashMap* newHashMap(int num, va_list args)
{
    assert(num == 2);
    HashMap* m = (HashMap*)Heap_Malloc(sizeof(HashMap));

    // m->entrySet = (entry**)Heap_Malloc(sizeof(entry*) * HASH_SIZE);
    // m->keySet = (void**)Heap_Malloc(sizeof(void*) * HASH_SIZE);
    for (int i = 0; i < HASH_SIZE; i++) {
        m->keySet[i] = NULL;
        m->entrySet[i] = NULL;
    }

    m->keyType = va_arg(args, TYPE);
    m->valueType = va_arg(args, TYPE);

    return m;
}

void* new (TYPE t, int num, ...)
{
    void* new_t;
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

void delete (TYPE t, void* d)
{
    switch (t) {
    case (STRING):
        Heap_Free(((String*)d)->data);
        break;
    case (ENTRY):
        delete (((entry*)d)->map->keyType, ((entry*)d)->key);
        delete (((entry*)d)->map->valueType, ((entry*)d)->value);
        Heap_Free(d);
        break;
    case (HASHMAP):
        clear((HashMap*)d);
        Heap_Free(((HashMap*)d)->entrySet);
        Heap_Free(((HashMap*)d)->keySet);
        break;
    default:
        return;
    }
    free(d);
}
