#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "types.h"

void doubleSize(String *s) {
    s->data = (char*)realloc(s->data, (size_t)s->cap * 2 + 1);
    s->cap *= 2;
}

void concat(String *s, const String *o) {
    while (s->cap < s->len + o->len)
        doubleSize(s);
    for (int i = 0; i < o->len; i++)
        s->data[i + s->len] = o->data[i];
    s->len += o->len;
    s->data[s->len] = '\0';
}

void addChar(String *s, const char c) {
    if (s->len == s->cap)
        doubleSize(s);
    s->data[s->len++] = c;
    s->data[s->len] = '\0';
}

void toLower(String *s) {
    for (int i = 0; i < s->len; i++)
        if (s->data[i] <= 'Z' && s->data[i] >= 'A')
            s->data[i] += 0x20;
}

void toUpper(String *s) {
    for (int i = 0; i < s->len; i++)
        if (s->data[i] <= 'z' && s->data[i] >= 'a')
            s->data[i] -= 0x20;
}

bool equals(String* s, String* o) {
    if (s->len != o->len) return false;
    for (int i = 0; i < s->len; i++)
        if (s->data[i] != o->data[i])
            return false;
    return true;
}

void capitalize(String *word) {
    if (word->data[0] >= 'a' && word->data[0] <= 'z')
        word->data[0] -= 0x20;
}
