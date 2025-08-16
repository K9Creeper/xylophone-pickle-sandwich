#ifndef STRING_H
#define STRING_H
#include <stdbool.h>
#include <stdint.h>

bool isletter(const char c);
bool islower(const char c);
bool isdigit(const char c);
bool ispunct(const char c);

static inline char toupper(char c) {
    if (c >= 'a' && c <= 'z')
        return (char)(c - ('a' - 'A'));
    return c;
}

int strlen(const char *str);
char *strtok(char *str, const char *delim, char *out);
int strcmp(const char *s1, const char *s2);
int memcmp(const void* s1, const void* s2, uint32_t n);

void trim(char* str);

bool equal(const char *a, const char *b);

int find(const char *str, const char *f);
void itoa(int num, char *str, int base);
void uitoa(uint32_t num, char *str, int base);
int atoi(const char *str);
uint32_t uatoi(const char *str);

#endif